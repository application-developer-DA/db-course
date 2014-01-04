#include "maintabwindow.h"
#include "ui_maintabwindow.h"

#include <QtSql>
#include <QMessageBox>

enum {
    Sport_Id,
    Sport_Name
};

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
    QSqlQuery userQuery("SELECT CURRENT_USER");
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
}

void MainTabWindow::fillSports()
{
    sportsModel = new QSqlTableModel(this);
    sportsModel->setTable("Sport");
    sportsModel->setHeaderData(Sport_Name, Qt::Horizontal, tr("Sport name"));
    sportsModel->select();

    ui->sportsView->setModel(sportsModel);
    ui->sportsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sportsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sportsView->setColumnHidden(Sport_Id, true);
    ui->sportsView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->sportsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateSportCoachesView()));

    coachesModel = new QSqlQueryModel(this);
    ui->coachesView->setModel(coachesModel);
    ui->coachesView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->coachesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->coachesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->coachesView->horizontalHeader()->setStretchLastSection(true);
}

void MainTabWindow::updateSportCoachesView()
{
    QModelIndex index = ui->sportsView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsModel->record(index.row());
        QString sportName = record.value(Sport_Name).toString();

        coachesModel->setQuery(QString("EXEC CoachesSport @sportName = %1").arg(sportName));
    }
}

void MainTabWindow::fillSportsmen()
{
    sportsmenModel = new QSqlQueryModel(this);
    sportsmenModel->setQuery("SELECT DISTINCT Firstname, Lastname, Middlename, Birthdate FROM SportsmenWithSports");

    ui->sportsmenView->setModel(sportsmenModel);
    ui->sportsmenView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sportsmenView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sportsmenView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->sportsmenView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateSportsmanCoachesView()));

    sportsmanCoachesModel = new QSqlQueryModel();
    sportsmanCoachesModel->setQuery("SELECT DISTINCT [Coach Firstname] AS Firstname, [Coach Lastname] AS Lastname FROM SportsmenWithCoaches");
    ui->sportsmanCoachesView->setModel(sportsmanCoachesModel);
    ui->sportsmanCoachesView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sportsmanCoachesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sportsmanCoachesView->horizontalHeader()->setStretchLastSection(true);

    ui->sportComboBox->setModel(sportsModel);
    ui->sportComboBox->setModelColumn(Sport_Name);
    connect(ui->sportComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanSportFilter()));

    QSqlQueryModel* experienceModel = new QSqlQueryModel(this);
    experienceModel->setQuery("SELECT title FROM Experience GROUP BY title");
    ui->qualificationComboBox->setModel(experienceModel);
    ui->qualificationComboBox->setModelColumn(0);
    connect(ui->qualificationComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanQualificationFilter()));

    allCoaches = new QSqlQueryModel(this);
    allCoaches->setQuery("SELECT DISTINCT [CoachId] AS Id, [Coach Firstname] AS Firstname, [Coach Lastname] AS Lastname FROM SportsmenWithCoaches");
    ui->coachFilterComboBox->setModel(allCoaches);
    ui->coachFilterComboBox->setModelColumn(2);
    connect(ui->coachFilterComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanCoachFilter()));

    QDate today = QDate::currentDate();
    ui->startDateEdit->setCalendarPopup(true);
    ui->startDateEdit->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    ui->endDateEdit->setCalendarPopup(true);
    ui->endDateEdit->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    connect(ui->startDateEdit, SIGNAL(dateChanged(QDate)), SLOT(applySportsmanDateFilter()));
    connect(ui->endDateEdit, SIGNAL(dateChanged(QDate)), SLOT(applySportsmanDateFilter()));

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->sportFilterCheckbox);
    buttonGroup->addButton(ui->qualificationFilterCheckbox);
    buttonGroup->addButton(ui->coachFilterCheckbox);
    buttonGroup->addButton(ui->competitionsFilterCheckbox);
    buttonGroup->addButton(ui->multipleSportsFilterCheckbox);
}

void MainTabWindow::updateSportsmanCoachesView()
{
    QModelIndex index = ui->sportsmenView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsmenModel->record(index.row());

        sportsmanCoachesModel->setQuery(QString("EXEC CoachesOfSportsman @firstname = %1, @lastname = %2")
                                        .arg(record.value("Firstname").toString())
                                        .arg(record.value("Lastname").toString()));
    }
}

void MainTabWindow::fillConstructionsAndOrganizations()
{
    constructionsModel = new QSqlQueryModel(this);
    constructionsModel->setQuery("SELECT BuildingName, CompetitionName, OrganizationName, SportName FROM AllCompetitions");

    ui->buildingsView->setModel(constructionsModel);
    ui->buildingsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->buildingsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->buildingsView->horizontalHeader()->setStretchLastSection(true);

    organizationsModel = new QSqlQueryModel(this);
    organizationsModel->setQuery("SELECT OrganizationName, CompetitionName, SportName FROM AllCompetitions");

    ui->organizationsView->setModel(organizationsModel);
    ui->organizationsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->organizationsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->organizationsView->horizontalHeader()->setStretchLastSection(true);

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
    competitionsModel->setQuery("SELECT DISTINCT CompetitionName AS Name, CompetitionDate AS Date, SportName AS Sport, BuildingName AS Construction, OrganizationName AS Organization FROM AllCompetitions");

    ui->competitionsView->setModel(competitionsModel);
    ui->competitionsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->competitionsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->competitionsView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->competitionsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateWinnersView()));

    competitionWinnersModel = new QSqlQueryModel(this);
    ui->winnersView->setModel(competitionWinnersModel);
    ui->winnersView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->winnersView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->winnersView->horizontalHeader()->setStretchLastSection(true);

    competitionsClubs = new QSqlQueryModel(this);
    ui->clubsView->setModel(competitionsClubs);
    ui->clubsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->clubsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->clubsView->horizontalHeader()->setStretchLastSection(true);

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

        competitionWinnersModel->setQuery(QString("EXEC CompetitionWinners @competitionName = %1")
                                        .arg(record.value("Name").toString()));
    }
}

void MainTabWindow::applySportsmanSportFilter()
{
    QString sportName = ui->sportComboBox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithParticularSport @sportName = %1").arg(sportName));
}

void MainTabWindow::applySportsmanCoachFilter()
{
    int row = ui->coachFilterComboBox->currentIndex();
    QString firstname = allCoaches->record(row).value("Firstname").toString();
    QString lastname = allCoaches->record(row).value("Lastname").toString();
    sportsmenModel->setQuery(QString("EXEC SportsmenOfCoach @firstname = %1, @lastname = %2").arg(firstname).arg(lastname));
}

void MainTabWindow::applySportsmanQualificationFilter()
{
    QString title = ui->qualificationComboBox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithQualification @sportTitle = %1").arg(title));
}

void MainTabWindow::applySportsmanDateFilter()
{
    QDate startDate = ui->startDateEdit->date();
    QDate endDate = ui->endDateEdit->date();
    sportsmenModel->setQuery(QString("EXEC ThoseWhoDidntTakePartInCompetitions @from = '%1', @to = '%2'")
                             .arg(startDate.toString("yyyy-MM-dd"))
                             .arg(endDate.toString("yyyy-MM-dd")));
}

void MainTabWindow::applyConstructionPlacesFilter()
{
    int quantity = ui->constructionPlacesFilterEdit->text().toInt();
    constructionsModel->setQuery(QString("EXEC BuildingWithPlaces @placesAmount = %1").arg(quantity));
}

void MainTabWindow::applyConstructionTypeFilter()
{
    constructionsModel->setQuery(QString("EXEC BuildingByType @buildingType = %1").arg(ui->constructionTypeFilterCombobox->currentText()));
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
}

void MainTabWindow::applyCompetitionConstructionFilter()
{
    QString constructionName = ui->competitionConstructionFilterCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsInBuilding @buildingName = '%1'").arg(constructionName));
}

void MainTabWindow::applyCompetitionOrganizerFilter()
{
    QString organizationName = ui->competitionOrganizersCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsByOrganization @organizationName = '%1'").arg(organizationName));
}

void MainTabWindow::applyCompetitionSportFilter()
{
    QString sport = ui->competitionSportFilterCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsSport @sportName = %1").arg(sport));
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
}

void MainTabWindow::on_sportFilterCheckbox_stateChanged(int state)
{
    if (state)
        applySportsmanSportFilter();
    ui->sportComboBox->setEnabled(state);
}

void MainTabWindow::on_qualificationFilterCheckbox_stateChanged(int state)
{
    if (state)
        applySportsmanQualificationFilter();
    ui->qualificationComboBox->setEnabled(state);
}

void MainTabWindow::on_coachFilterCheckbox_stateChanged(int state)
{
    if (state)
        applySportsmanCoachFilter();
    ui->coachFilterComboBox->setEnabled(state);
}

void MainTabWindow::on_competitionsFilterCheckbox_stateChanged(int state)
{
    ui->startDateEdit->setEnabled(state);
    ui->endDateEdit->setEnabled(state);
}

void MainTabWindow::on_sportsmenResetFilters_clicked()
{
    sportsmenModel->setQuery("SELECT DISTINCT Firstname, Lastname, Middlename, Birthdate FROM SportsmenWithSports");
}

void MainTabWindow::on_multipleSportsFilterCheckbox_stateChanged(int state)
{
    if (state == Qt::Checked)
        sportsmenModel->setQuery("EXEC ThoseWhoStudyMoreThanOneSport");
}

void MainTabWindow::on_enableBuildingFilters_stateChanged(int state)
{
    if (state != Qt::Checked)
        constructionsModel->setQuery("SELECT BuildlingName, CompetitionName, OrganizationName, SportName FROM AllCompetitions");

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
        competitionsModel->setQuery("SELECT DISTINCT CompetitionName AS Name, CompetitionDate AS Date, SportName AS Sport, BuildingName AS Construction, OrganizationName AS Organization FROM AllCompetitions");

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
