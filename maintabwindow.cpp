#include "ui_maintabwindow.h"

#include "BaseEditForm.h"
#include "CompetitionEditForm.h"
#include "MainTabWindow.h"
#include "PersonEditForm.h"

#include <QtSql>
#include <QMessageBox>

enum {
    Sport_Id = 0,
    Sport_Name
};

static const QString kAllSportsmenQuery           = "SELECT id AS Id, firstname AS Firstname, lastname AS Lastname, middlename AS Middlename, birthdate AS Birthdate FROM Person";
static const QString kAllSportConstructionsQuery  = "SELECT [Building Id], [Building Name], [Competition Name], [Sport Name], [Organization Name] FROM AllCompetitions";
static const QString kAllCompetitionsQuery        = "SELECT [Competition Id], [Competition Name], [Competition Date], [Sport Name], [Building Name], [Organization Name] FROM AllCompetitions";
static const QString kAllCompetitionOrganizations = "SELECT [Organization Id], [Organization Name], [Competition Name], [Sport Name] FROM AllCompetitions";
static const QString kAllCoachesNames             = "SELECT DISTINCT [Coach Firstname] + ' ' + [Coach Lastname] + ' ' + [Coach Middlename] AS Coach FROM SportsmenWithCoaches";

static inline int getFirstRecordId(QTableView* view, QSqlQueryModel* model)
{
    int id = -1;
    QModelIndex index = view->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = model->record(index.row());
        id = record.value(0).toInt();
    }
    return id;
}

static inline void updateModel(QSqlQueryModel* model)
{
    model->setQuery(model->query().lastQuery());
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
    fillSportsmen();
    fillConstructionsAndOrganizations();
    fillCompetitions();
}

void MainTabWindow::on_addSportBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Sport Name:", BaseEditForm::LineEdit, QVariant(), 1 }
    };

    int id = getFirstRecordId(ui->sportsView, sportsModel);
    BaseEditForm form(id, "Sport", QVector<BaseEditForm::Relation>(), mappings, this);
    form.exec();

    sportsModel->select();
}

void MainTabWindow::on_deleteSportBtn_clicked()
{
    QModelIndex index = ui->sportsView->currentIndex();
    if (!index.isValid())
        return;

    QSqlRecord record = sportsModel->record(index.row());
    QString name = record.value(Sport_Name).toString();
    int r = QMessageBox::warning(this, tr("Delete Sport"), tr("Delete %1 and all connected tables?").arg(name),
                                 QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::No)
        return;

    sportsModel->removeRow(index.row());
    sportsModel->submitAll();

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
}

void MainTabWindow::updateSportCoachesView()
{
    QModelIndex index = ui->sportsView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsModel->record(index.row());
        QString sportName = record.value(Sport_Name).toString();

        coachesModel->setQuery(QString("EXEC CoachesSport @sportName = '%1'").arg(sportName));
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

    experienceModel = new QSqlQueryModel(this);
    ui->sportsmenQualificationCombobox->setModel(experienceModel);
    connect(ui->sportsmenQualificationCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanQualificationFilter()));

    ui->sportsmenSportCombobox->setModel(sportsModel);
    ui->sportsmenSportCombobox->setModelColumn(Sport_Name);
    connect(ui->sportsmenSportCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applySportsmanSportFilter()));

    allCoaches = new QSqlQueryModel(this);
    ui->sportsmenCoachCombobox->setModel(allCoaches);
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
    }
}

void MainTabWindow::fillConstructionsAndOrganizations()
{
    constructionsModel = new QSqlQueryModel(this);
    constructionsModel->setQuery(kAllSportConstructionsQuery);
    ui->buildingsView->setModel(constructionsModel);
    setDefaultViewParameters(ui->buildingsView);

    organizationsModel = new QSqlQueryModel(this);
    organizationsModel->setQuery(kAllCompetitionOrganizations);
    ui->organizationsView->setModel(organizationsModel);
    setDefaultViewParameters(ui->organizationsView);

    constructionType = new QSqlQueryModel(this);
    ui->constructionTypeFilterCombobox->setModel(constructionType);
    connect(ui->constructionTypeFilterCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applyConstructionTypeFilter()));

    ui->constructionPlacesFilterEdit->setValidator(new QIntValidator(1, 9999, this));
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

    constructions = new QSqlQueryModel(this);
    ui->competitionConstructionFilterCombobox->setModel(constructions);
    connect(ui->competitionConstructionFilterCombobox, SIGNAL(currentIndexChanged(int)), SLOT(applyCompetitionConstructionFilter()));

    organizers = new QSqlQueryModel(this);
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
    }
}

void MainTabWindow::applySportsmanSportFilter()
{
    if (!ui->sportsmenSportFilter->isChecked())
        return;
    QString sportName = ui->sportsmenSportCombobox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithParticularSport @sportName = '%1'").arg(sportName));
}

void MainTabWindow::applySportsmanCoachFilter()
{
    if (!ui->sportsmenCoachFilter->isChecked())
        return;
    QString record = ui->sportsmenCoachCombobox->currentText();

    if (record.isNull() || record.isEmpty())
        return;

    QStringList coach = record.split(' ');
    Q_ASSERT(coach.size() == 3);
    sportsmenModel->setQuery(QString("EXEC SportsmenOfCoach @firstname = '%1', @lastname = '%2', @middlename = '%3'")
                             .arg(coach[0]).arg(coach[1]).arg(coach[2]));
}

void MainTabWindow::applySportsmanQualificationFilter()
{
    if (!ui->sportsmenQualificationFilter->isChecked())
        return;
    QString title = ui->sportsmenQualificationCombobox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithQualification @sportTitle = '%1'").arg(title));
}

void MainTabWindow::applySportsmanDateFilter()
{
    QDate startDate = ui->sportsmenStartDateEdit->date();
    QDate endDate = ui->sportsmenEndDateEdit->date();
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
    if (!ui->constructionTypeFilter->isChecked())
        return;
    constructionsModel->setQuery(QString("EXEC BuildingByType @buildingType = '%1'").arg(ui->constructionTypeFilterCombobox->currentText()));
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
    if (!ui->competitionConstructionFilter->isChecked())
        return;
    QString constructionName = ui->competitionConstructionFilterCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsInBuilding @buildingName = '%1'").arg(constructionName));
}

void MainTabWindow::applyCompetitionOrganizerFilter()
{
    if (!ui->competitionOrganizerFilter->isChecked())
        return;
    QString organizationName = ui->competitionOrganizersCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsByOrganization @organizationName = '%1'").arg(organizationName));
}

void MainTabWindow::applyCompetitionSportFilter()
{
    if (!ui->competitionSportFilter->isChecked())
        return;
    QString sport = ui->competitionSportFilterCombobox->currentText();
    competitionsModel->setQuery(QString("EXEC CompetitionsSport @sportName = '%1'").arg(sport));
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

void MainTabWindow::on_enableSportsmenFilters_stateChanged(int state)
{
    if (state != Qt::Checked)
        sportsmenModel->setQuery(kAllSportsmenQuery);

    foreach (QWidget* widget, sportsmenFilterWidgets)
        widget->setVisible(state == Qt::Checked);
}

void MainTabWindow::on_sportsmenSportFilter_toggled(bool checked)
{
    applySportsmanSportFilter();
    ui->sportsmenSportCombobox->setEnabled(checked);
}

void MainTabWindow::on_sportsmenQualificationFilter_toggled(bool checked)
{
    if (checked)
        experienceModel->setQuery("SELECT DISTINCT title FROM Experience");
    ui->sportsmenQualificationCombobox->setEnabled(checked);
    applySportsmanQualificationFilter();
}

void MainTabWindow::on_sportsmenCoachFilter_toggled(bool checked)
{
    if (checked) {
        allCoaches->setQuery(kAllCoachesNames);
    }
    ui->sportsmenCoachCombobox->setEnabled(checked);
    applySportsmanCoachFilter();
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
        constructionType->setQuery("SELECT DISTINCT building_type FROM Building");
    ui->constructionTypeFilterCombobox->setEnabled(checked);
    applyConstructionTypeFilter();
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
        organizers->setQuery("SELECT name FROM Organization");
    ui->competitionOrganizersCombobox->setEnabled(checked);
    applyCompetitionOrganizerFilter();
}

void MainTabWindow::on_competitionSportFilter_toggled(bool checked)
{
    ui->competitionSportFilterCombobox->setEnabled(checked);
    applyCompetitionSportFilter();
}

void MainTabWindow::on_competitionConstructionFilter_toggled(bool checked)
{
    if (checked)
        constructions->setQuery("SELECT name FROM Building");
    ui->competitionConstructionFilterCombobox->setEnabled(checked);
    applyCompetitionConstructionFilter();
}

void MainTabWindow::on_editCoachBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Firstname",  BaseEditForm::LineEdit, QVariant(), 1 },
        { "Lastname",   BaseEditForm::LineEdit, QVariant(), 2 },
        { "Middlename", BaseEditForm::LineEdit, QVariant(), 3 },
        { "Birthdate",  BaseEditForm::DateEdit, QVariant(), 4 }
    };

    int id = getFirstRecordId(ui->coachesView, coachesModel);
    PersonEditForm form(id, "Person", QVector<BaseEditForm::Relation>(), mappings, this);
    form.exec();

    updateSportCoachesView();
}

void MainTabWindow::on_editSportsmanBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Firstname",  BaseEditForm::LineEdit, QVariant(), 1 },
        { "Lastname",   BaseEditForm::LineEdit, QVariant(), 2 },
        { "Middlename", BaseEditForm::LineEdit, QVariant(), 3 },
        { "Birthdate",  BaseEditForm::DateEdit, QVariant(), 4 }
    };

    int id = getFirstRecordId(ui->sportsmenView, sportsmenModel);
    PersonEditForm form(id, "Person", QVector<BaseEditForm::Relation>(), mappings, this);
    form.exec();

    updateModel(sportsmenModel);
    updateSportsmanCoachesView();
}

void MainTabWindow::on_editSportConstructionBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Owner Organization:", BaseEditForm::ComboBox, QVariant("name"), 1 },
        { "Constuction Name:",   BaseEditForm::LineEdit, QVariant(),       2 },
        { "Address:",            BaseEditForm::LineEdit, QVariant(),       3 },
        { "Type:",               BaseEditForm::LineEdit, QVariant(),       4 },
        { "Places Amount:",      BaseEditForm::LineEdit, QVariant(),       5 },
        { "Area:",               BaseEditForm::LineEdit, QVariant(),       6 }
    };

    QVector<BaseEditForm::Relation> relations {
        { 1, QSqlRelation("Organization", "id", "name") }
    };

    int id = getFirstRecordId(ui->buildingsView, constructionsModel);
    BaseEditForm form(id, "Building", relations, mappings);
    form.exec();

    updateModel(constructionsModel);
}

void MainTabWindow::on_organizationEditBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Name:",    BaseEditForm::LineEdit, QVariant(), 1 },
        { "Address:", BaseEditForm::LineEdit, QVariant(), 2 }
    };

    int id = getFirstRecordId(ui->organizationsView, organizationsModel);
    BaseEditForm form(id, "Organization", QVector<BaseEditForm::Relation>(), mappings);
    form.exec();

    updateModel(organizationsModel);
}

void MainTabWindow::on_sportsmanCoachEditBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Firstname",  BaseEditForm::LineEdit, QVariant(), 1 },
        { "Lastname",   BaseEditForm::LineEdit, QVariant(), 2 },
        { "Middlename", BaseEditForm::LineEdit, QVariant(), 3 },
        { "Birthdate",  BaseEditForm::DateEdit, QVariant(), 4 }
    };

    int id = getFirstRecordId(ui->sportsmanCoachesView, sportsmanCoachesModel);
    PersonEditForm form(id, "Person", QVector<BaseEditForm::Relation>(), mappings, this);
    form.exec();

    updateModel(coachesModel);
}

void MainTabWindow::on_editClubsBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "Organization Name:", BaseEditForm::ComboBox, QVariant("name"), 1 },
        { "Name:",              BaseEditForm::LineEdit, QVariant(),       2 }
    };

    QVector<BaseEditForm::Relation> relations {
        { 1, QSqlRelation("Organization", "id", "name") }
    };

    int id = getFirstRecordId(ui->clubsView, competitionsClubs);
    BaseEditForm form(id, "Club", relations, mappings);
    form.exec();

    updateModel(competitionsClubs);
}

void MainTabWindow::on_editCompetitionsBtn_clicked()
{
    QVector<BaseEditForm::WidgetMapping> mappings {
        { "SportConstruction:", BaseEditForm::ComboBox, QVariant("name"), 1 },
        { "Sport:",             BaseEditForm::ComboBox, QVariant("name"), 2 },
        { "Competition Name:",  BaseEditForm::LineEdit, QVariant(),       3 },
        { "Competition Date:",  BaseEditForm::DateEdit, QVariant(),       4 }
    };

    QVector<BaseEditForm::Relation> relations {
        { 1, QSqlRelation("Building", "id", "name") },
        { 2, QSqlRelation("Sport", "id", "name") }
    };

    int id = getFirstRecordId(ui->competitionsView, competitionsModel);
    CompetitionEditForm form(id, "Competition", relations, mappings);
    form.exec();

    updateModel(competitionsModel);
}

void MainTabWindow::setDefaultViewParameters(QTableView* view)
{
    view->setColumnHidden(0, true);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->horizontalHeader()->setStretchLastSection(true);
    view->resizeColumnsToContents();

    connect(view->model(), &QAbstractItemModel::modelReset, [=]() {
        QString header = view->model()->headerData(0, Qt::Horizontal).toString();
        if (header.indexOf("Id") >= 0)
            view->setColumnHidden(0, true);
        view->resizeColumnsToContents();
        view->horizontalHeader()->setStretchLastSection(true);
    } );
}
