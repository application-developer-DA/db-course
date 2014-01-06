#include "ui_maintabwindow.h"

#include "MainTabWindow.h"
#include "PersonEditForm.h"

#include <QtSql>
#include <QMessageBox>

enum {
    Sport_Id,
    Sport_Name
};

static const QString kAllSportsmenQuery           = "SELECT id AS Id, firstname AS Firstname, lastname AS Lastname, middlename AS Middlename, birthdate AS Birthdate FROM Person";
static const QString kAllSportConstructionsQuery  = "SELECT [Building Name], [Competition Name], [Sport Name], [Organization Name] FROM AllCompetitions";
static const QString kAllCompetitionsQuery        = "SELECT [Competition Id], [Competition Name], [Competition Date], [Sport Name], [Building Name], [Organization Name] FROM AllCompetitions";

static inline void setDefaultViewParameters(QTableView* view)
{
    view->setColumnHidden(0, true);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->horizontalHeader()->setStretchLastSection(true);
}

MainTabWindow::MainTabWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainTabWindow)
    , sportsModel(nullptr)
    , coachesModel(nullptr)
    , sportsmenModel(nullptr)
    , sportsmanCoachesModel(nullptr)
    , constructionsModel(nullptr)
    , competitionsModel(nullptr)
    , competitionWinnersModel(nullptr)
    , allCoaches(nullptr)
{
    ui->setupUi(this);
}

MainTabWindow::~MainTabWindow()
{
    delete ui;
}

void MainTabWindow::login()
{
    QSqlQuery userQuery("SELECT User_Name()");
    while (userQuery.next()) {
        QString userName = userQuery.value(0).toString();
        ui->loginLabel->setText(QString("You logged in as %1").arg(userName));
    }

    fillSports();
    ui->sportsView->setCurrentIndex(sportsModel->index(0, 0));

    fillSportsmen();
    fillConstructionsAndOrganizations();
    fillCompetitions();
}

void MainTabWindow::on_addSportBtn_clicked()
{
    int row = sportsModel->rowCount();
    sportsModel->insertRow(row);
    QModelIndex index = sportsModel->index(row, Sport_Name);
    ui->sportsView->setCurrentIndex(index);
    ui->sportsView->edit(index);

    // connect(ui->sportsView, &QTableView::entered, [=]() { sportsModel->select(); });
}

void MainTabWindow::on_deleteSportBtn_clicked()
{
    QModelIndex index = ui->sportsView->currentIndex();
    if (!index.isValid())
        return;

    QSqlDatabase::database().transaction();
    QSqlRecord record = sportsModel->record(index.row());
    QString name = record.value(Sport_Name).toString();
    int r = QMessageBox::warning(this, tr("Delete Sport"), tr("Delete %1 and all connected tables?").arg(name),
                                 QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::No) {
        QSqlDatabase::database().rollback();
        return;
    }
    sportsModel->removeRow(index.row());
    sportsModel->submitAll();
    QSqlDatabase::database().commit();

    updateSportCoachesView();
    ui->sportsView->setFocus();
    sportsModel->select();
}

void MainTabWindow::fillSports()
{
    sportsModel = new QSqlTableModel(this);
    sportsModel->setTable("Sport");
    sportsModel->setHeaderData(Sport_Name, Qt::Horizontal, tr("Sport name"));
    sportsModel->select();

    ui->sportsView->setModel(sportsModel);
    setDefaultViewParameters(ui->sportsView);

    connect(ui->sportsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateSportCoachesView()));

    coachesModel = new QSqlQueryModel(this);
    ui->coachesView->setModel(coachesModel);
    setDefaultViewParameters(ui->coachesView);
    ui->coachesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainTabWindow::updateSportCoachesView()
{
    QModelIndex index = ui->sportsView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsModel->record(index.row());
        QString sportName = record.value(Sport_Name).toString();

        coachesModel->setQuery(QString("EXEC CoachesSport @sportName = %1").arg(sportName));
        setDefaultViewParameters(ui->coachesView);
    }
}

void MainTabWindow::fillSportsmen()
{
    sportsmenModel = new QSqlQueryModel(this);
    sportsmenModel->setQuery(kAllSportsmenQuery);

    ui->sportsmenView->setModel(sportsmenModel);
    setDefaultViewParameters(ui->sportsmenView);

    connect(ui->sportsmenView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateSportsmanCoachesView()));

    sportsmanCoachesModel = new QSqlQueryModel();
    ui->sportsmanCoachesView->setModel(sportsmanCoachesModel);
    setDefaultViewParameters(ui->sportsmanCoachesView);

    ui->sportsmenSportCombobox->setModel(sportsModel);
    ui->sportsmenSportCombobox->setModelColumn(Sport_Name);
    connect(ui->sportsmenSportCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanSportFilter()));

    QSqlQueryModel* experienceModel = new QSqlQueryModel(this);
    experienceModel->setQuery("SELECT DISTINCT title FROM Experience");
    ui->sportsmenQualificationCombobox->setModel(experienceModel);
    ui->sportsmenQualificationCombobox->setModelColumn(0);
    connect(ui->sportsmenQualificationCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanQualificationFilter()));

    allCoaches = new QSqlQueryModel(this);
    allCoaches->setQuery("SELECT DISTINCT [Coach Firstname] + ' ' + [Coach Lastname] + ' ' + [Coach Middlename] AS Coach FROM SportsmenWithCoaches");
    ui->sportsmenCoachCombobox->setModel(allCoaches);
    ui->sportsmenCoachCombobox->setModelColumn(0);
    connect(ui->sportsmenCoachCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanCoachFilter()));

    QDate today = QDate::currentDate();
    ui->sportsmenStartDateEdit->setCalendarPopup(true);
    ui->sportsmenStartDateEdit->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    ui->sportsmenEndDateEdit->setCalendarPopup(true);
    ui->sportsmenEndDateEdit->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    connect(ui->sportsmenStartDateEdit, SIGNAL(dateChanged(QDate)), SLOT(applySportsmanDateFilter()));
    connect(ui->sportsmenEndDateEdit, SIGNAL(dateChanged(QDate)), SLOT(applySportsmanDateFilter()));

    sportsmenFilterWidgets << ui->sportsmenSportFilter << ui->sportsmenSportCombobox
                           << ui->sportsmenCoachFilter << ui->sportsmenCoachCombobox
                           << ui->sportsmenQualificationFilter << ui->sportsmenQualificationCombobox
                           << ui->sportsmenCompetitionFilter << ui->sportsmenStartDateEdit << ui->sportsmenEndDateEdit
                           << ui->sportsmenMultipleSportsFilter;
    foreach (QWidget* widget, sportsmenFilterWidgets)
        widget->hide();
}

void MainTabWindow::updateSportsmanCoachesView()
{
    QModelIndex index = ui->sportsmenView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsmenModel->record(index.row());
        QString query = QString("EXEC CoachesOfSportsman @firstname = '%1', @lastname = '%2', @middlename = '%3'")
                .arg(record.value("Firstname").toString())
                .arg(record.value("Lastname").toString())
                .arg(record.value("Middlename").toString());
        sportsmanCoachesModel->setQuery(query);
        setDefaultViewParameters(ui->sportsmanCoachesView);
    }
}

void MainTabWindow::fillConstructionsAndOrganizations()
{
    constructionsModel = new QSqlQueryModel(this);
    constructionsModel->setQuery(kAllSportConstructionsQuery);

    ui->buildingsView->setModel(constructionsModel);
    setDefaultViewParameters(ui->buildingsView);

    organizationsModel = new QSqlQueryModel(this);
    organizationsModel->setQuery("SELECT [Organization Id], [Organization Name], [Competition Name], [Sport Name] FROM AllCompetitions");

    ui->organizationsView->setModel(organizationsModel);
    setDefaultViewParameters(ui->organizationsView);

    QSqlQueryModel* constructionType = new QSqlQueryModel(this);
    constructionType->setQuery("SELECT DISTINCT building_type FROM Building");
    ui->constructionTypeFilterCombobox->setModel(constructionType);
    ui->constructionPlacesFilterEdit->setValidator(new QIntValidator(1, 9999, this));
    connect(ui->constructionTypeFilterCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applyConstructionTypeFilter()));
    connect(ui->constructionPlacesFilterEdit, SIGNAL(editingFinished()), SLOT(applyConstructionPlacesFilter()));

    QDate today = QDate::currentDate();
    ui->constructionStartCompetition->setCalendarPopup(true);
    ui->constructionStartCompetition->setDateRange(today.addDays(-365*4), today.addDays(365*2));
    ui->constructionEndCompetition->setCalendarPopup(true);
    ui->constructionEndCompetition->setDateRange(today.addDays(-365*4), today.addDays(365*2));
    connect(ui->constructionStartCompetition, SIGNAL(dateChanged(QDate)), SLOT(applyConstructionCompetitionFilter()));
    connect(ui->constructionEndCompetition, SIGNAL(dateChanged(QDate)), SLOT(applyConstructionCompetitionFilter()));

    constructionFilterWidgets << ui->constructionTypeFilter << ui->constructionTypeFilterCombobox
                              << ui->constructionPlacesFilter << ui->constructionPlacesFilterEdit
                              << ui->constructionCompetitionsFilter << ui->constructionStartCompetition << ui->constructionEndCompetition;
    foreach (QWidget* widget, constructionFilterWidgets)
        widget->hide();
}

void MainTabWindow::fillCompetitions()
{
    competitionsModel = new QSqlQueryModel(this);
    competitionsModel->setQuery(kAllCompetitionsQuery);

    ui->competitionsView->setModel(competitionsModel);
    setDefaultViewParameters(ui->competitionsView);

    connect(ui->competitionsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateWinnersView()));

    competitionWinnersModel = new QSqlQueryModel(this);
    ui->winnersView->setModel(competitionWinnersModel);
    setDefaultViewParameters(ui->winnersView);

    competitionsClubs = new QSqlQueryModel(this);
    ui->clubsView->setModel(competitionsClubs);
    setDefaultViewParameters(ui->clubsView);

    QSqlQueryModel* constructions = new QSqlQueryModel(this);
    constructions->setQuery("SELECT name FROM Building");
    ui->competitionConstructionFilterCombobox->setModel(constructions);
    connect(ui->competitionConstructionFilterCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applyCompetitionConstructionFilter()));

    QSqlQueryModel* organizers = new QSqlQueryModel(this);
    organizers->setQuery("SELECT name FROM Organization");
    ui->competitionOrganizersCombobox->setModel(organizers);
    connect(ui->competitionOrganizersCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applyCompetitionOrganizerFilter()));

    ui->competitionSportFilterCombobox->setModel(sportsModel);
    ui->competitionSportFilterCombobox->setModelColumn(Sport_Name);
    connect(ui->competitionSportFilterCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applyCompetitionSportFilter()));

    QDate today = QDate::currentDate();
    ui->startCompetitionDate->setCalendarPopup(true);
    ui->startCompetitionDate->setDateRange(today.addDays(-365*4), today.addDays(365*2));
    ui->endCompetitionDate->setCalendarPopup(true);
    ui->endCompetitionDate->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    connect(ui->startCompetitionDate, SIGNAL(dateChanged(QDate)), SLOT(applyCompetitionDateFilter()));
    connect(ui->endCompetitionDate, SIGNAL(dateChanged(QDate)), SLOT(applyCompetitionDateFilter()));

    competitionFilterWidgets << ui->competitionConstructionFilter << ui->competitionConstructionFilterCombobox
                             << ui->competitionOrganizerFilter << ui->competitionOrganizersCombobox
                             << ui->competitionSportFilter << ui->competitionSportFilterCombobox
                             << ui->competitionDateFilter << ui->startCompetitionDate << ui->endCompetitionDate;
    foreach (QWidget* widget, competitionFilterWidgets)
        widget->hide();
}

void MainTabWindow::updateWinnersView()
{
    QModelIndex index = ui->competitionsView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = competitionsModel->record(index.row());

        competitionWinnersModel->setQuery(QString("EXEC CompetitionWinners @competitionName = '%1'")
                                        .arg(record.value("Competition Name").toString()));
        setDefaultViewParameters(ui->winnersView);
    }
}

void MainTabWindow::applySportsmanSportFilter()
{
    QString sportName = ui->sportsmenSportCombobox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithParticularSport @sportName = '%1'").arg(sportName));
    setDefaultViewParameters(ui->sportsmenView);
}

void MainTabWindow::applySportsmanCoachFilter()
{
    QString record = ui->sportsmenCoachCombobox->currentText();
    QStringList coach = record.split(' ');

    Q_ASSERT(coach.size() == 3);
    sportsmenModel->setQuery(QString("EXEC SportsmenOfCoach @firstname = '%1', @lastname = '%2', @middlename = '%3'")
                             .arg(coach[0]).arg(coach[1]).arg(coach[2]));
    setDefaultViewParameters(ui->sportsmenView);
}

void MainTabWindow::applySportsmanQualificationFilter()
{
    QString title = ui->sportsmenQualificationCombobox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithQualification @sportTitle = '%1'").arg(title));
    setDefaultViewParameters(ui->sportsmenView);
}

void MainTabWindow::applySportsmanDateFilter()
{
    QDate startDate = ui->sportsmenStartDateEdit->date();
    QDate endDate = ui->sportsmenEndDateEdit->date();
    sportsmenModel->setQuery(QString("EXEC ThoseWhoDidntTakePartInCompetitions @from = '%1', @to = '%2'")
                             .arg(startDate.toString("yyyy-MM-dd"))
                             .arg(endDate.toString("yyyy-MM-dd")));
    setDefaultViewParameters(ui->sportsmenView);
}

void MainTabWindow::applyConstructionPlacesFilter()
{
    int quantity = ui->constructionPlacesFilterEdit->text().toInt();
    constructionsModel->setQuery(QString("EXEC BuildingWithPlaces @placesAmount = %1").arg(quantity));
    setDefaultViewParameters(ui->buildingsView);
}

void MainTabWindow::applyConstructionTypeFilter()
{
    constructionsModel->setQuery(QString("EXEC BuildingByType @buildingType = '%1'").arg(ui->constructionTypeFilterCombobox->currentText()));
    setDefaultViewParameters(ui->buildingsView);
}

void MainTabWindow::applyConstructionCompetitionFilter()
{
    QDate startDate = ui->constructionStartCompetition->date();
    QDate endDate = ui->constructionEndCompetition->date();
    constructionsModel->setQuery(QString("EXEC CompetitionsOnBuildingBetweenDate @from = '%1', @to = '%2'")
                                 .arg(startDate.toString("yyyy-MM-dd"))
                                 .arg(endDate.toString("yyyy-MM-dd")));
    organizationsModel->setQuery(QString("EXEC CompetitonsByOrganizationBetweenDate @from = '%1', @to = '%2'")
                                 .arg(startDate.toString("yyyy-MM-dd"))
                                 .arg(endDate.toString("yyyy-MM-dd")));
    setDefaultViewParameters(ui->buildingsView);
    setDefaultViewParameters(ui->organizationsView);
}

void MainTabWindow::applyCompetitionConstructionFilter()
{
    QString constructionName = ui->competitionConstructionFilterCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsInBuilding @buildingName = '%1'").arg(constructionName));
    setDefaultViewParameters(ui->buildingsView);
}

void MainTabWindow::applyCompetitionOrganizerFilter()
{
    QString organizationName = ui->competitionOrganizersCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsByOrganization @organizationName = '%1'").arg(organizationName));
    setDefaultViewParameters(ui->competitionsView);
}

void MainTabWindow::applyCompetitionSportFilter()
{
    QString sport = ui->competitionSportFilterCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsSport @sportName = '%1'").arg(sport));
    setDefaultViewParameters(ui->competitionsView);
}

void MainTabWindow::applyCompetitionDateFilter()
{
    QDate startDate = ui->startCompetitionDate->date();
    QDate endDate = ui->endCompetitionDate->date();
    competitionsModel->setQuery(QString("EXEC CompetitionsBetweenDate @from = '%1', @to = '%2'")
                                .arg(startDate.toString("yyyy-MM-dd"))
                                .arg(endDate.toString("yyyy-MM-dd")));
    competitionsClubs->setQuery(QString("EXEC ClubSportsmensAmountInCompetitions @from = '%1', @to = '%2'")
                                .arg(startDate.toString("yyyy-MM-dd"))
                                .arg(endDate.toString("yyyy-MM-dd")));
    setDefaultViewParameters(ui->competitionsView);
    setDefaultViewParameters(ui->clubsView);
}

void MainTabWindow::on_enableSportsmenFilters_stateChanged(int state)
{
    if (state != Qt::Checked)
        sportsmenModel->setQuery(kAllSportsmenQuery);

    foreach (QWidget* widget, sportsmenFilterWidgets)
        widget->setVisible(state == Qt::Checked);
}

void MainTabWindow::on_sportsmenSportFilter_toggled(bool checked)
{
    if (checked)
        applySportsmanSportFilter();
    ui->sportsmenSportCombobox->setEnabled(checked);
}

void MainTabWindow::on_sportsmenQualificationFilter_toggled(bool checked)
{
    if (checked)
        applySportsmanQualificationFilter();
    ui->sportsmenQualificationCombobox->setEnabled(checked);
}

void MainTabWindow::on_sportsmenCoachFilter_toggled(bool checked)
{
    if (checked)
        applySportsmanCoachFilter();
    ui->sportsmenCoachCombobox->setEnabled(checked);
}

void MainTabWindow::on_sportsmenCompetitionFilter_toggled(bool checked)
{
    ui->sportsmenStartDateEdit->setEnabled(checked);
    ui->sportsmenEndDateEdit->setEnabled(checked);
}

void MainTabWindow::on_sportsmenMultipleSportsFilter_toggled(bool checked)
{
    if (checked)
        sportsmenModel->setQuery("EXEC ThoseWhoStudyMoreThanOneSport");
}

void MainTabWindow::on_enableBuildingFilters_stateChanged(int state)
{
    if (state != Qt::Checked)
        constructionsModel->setQuery(kAllSportConstructionsQuery);

    foreach (QWidget* widget, constructionFilterWidgets)
        widget->setVisible(state == Qt::Checked);
}

void MainTabWindow::on_constructionTypeFilter_toggled(bool checked)
{
    if (checked)
        applyConstructionTypeFilter();
    ui->constructionTypeFilterCombobox->setEnabled(checked);
}

void MainTabWindow::on_constructionPlacesFilter_toggled(bool checked)
{
    if (checked)
        applyConstructionPlacesFilter();
    ui->constructionPlacesFilterEdit->setEnabled(checked);
}

void MainTabWindow::on_constructionCompetitionsFilter_toggled(bool checked)
{
    if (checked)
        applyConstructionCompetitionFilter();
    ui->constructionStartCompetition->setEnabled(checked);
    ui->constructionEndCompetition->setEnabled(checked);
}

void MainTabWindow::on_enableCompetitionFilters_stateChanged(int state)
{
    if (state != Qt::Checked)
        competitionsModel->setQuery(kAllCompetitionsQuery);

    foreach (QWidget* widget, competitionFilterWidgets)
        widget->setVisible(state == Qt::Checked);
}

void MainTabWindow::on_competitionDateFilter_toggled(bool checked)
{
    if (checked)
        applyCompetitionDateFilter();
    ui->startCompetitionDate->setEnabled(checked);
    ui->endCompetitionDate->setEnabled(checked);
}

void MainTabWindow::on_competitionOrganizerFilter_toggled(bool checked)
{
    if (checked)
        applyCompetitionOrganizerFilter();
    ui->competitionOrganizersCombobox->setEnabled(checked);
}

void MainTabWindow::on_competitionSportFilter_toggled(bool checked)
{
    if (checked)
        applyCompetitionSportFilter();
    ui->competitionSportFilterCombobox->setEnabled(checked);
}

void MainTabWindow::on_competitionConstructionFilter_toggled(bool checked)
{
    if (checked)
        applyCompetitionConstructionFilter();
    ui->competitionConstructionFilterCombobox->setEnabled(checked);
}

void MainTabWindow::on_editCoachBtn_clicked()
{
    int coachId = -1;
    QModelIndex index = ui->coachesView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = coachesModel->record(index.row());
        coachId = record.value(0).toInt();
    }

    PersonEditForm form(coachId, true, this);
    form.exec();
    updateSportCoachesView();
}

void MainTabWindow::on_editSportsmanBtn_clicked()
{
    int sportsmanId = -1;
    QModelIndex index = ui->sportsmenView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsmenModel->record(index.row());
        sportsmanId = record.value(0).toInt();
    }

    PersonEditForm form(sportsmanId, false, this);
    form.exec();
    updateSportsmanCoachesView();
    // Update Sportsman View
}
