#include "maintabwindow.h"
#include "ui_maintabwindow.h"

#include <QtSql>
#include <QMessageBox>

enum {
    Sport_Id,
    Sport_Name
};

enum {
    SportsmanWithSports_Id,
    SportsmanWithSports_Firstname,
    SportsmanWithSports_Lastname,
    SportsmanWithSports_Middlename,
    SportsmanWithSports_Birthdate,
    SportsmanWithSports_SportId,
    SportsmanWithSports_Sport,
    SportsmanWithSports_ExperienceId,
    SportsmanWithSports_Title
};

MainTabWindow::MainTabWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainTabWindow)
    , sportsModel(nullptr)
    , coachesModel(nullptr)
    , sportsmenModel(nullptr)
    , sportsmanCoachesModel(nullptr)
    , allCoaches(nullptr)
{
    ui->setupUi(this);
}

MainTabWindow::~MainTabWindow()
{
    delete ui;
}

void MainTabWindow::loggedIn(const QString& username)
{
    ui->loginLabel->setText(QString("You logged in as %1").arg(username));

    fillSports();
    ui->sportsView->setCurrentIndex(sportsModel->index(0, 0));

    fillSportsmen();
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

void MainTabWindow::fillSportsmen()
{
    sportsmenModel = new QSqlQueryModel(this);
    sportsmenModel->setQuery("SELECT DISTINCT * FROM SportsmenWithSports");

    ui->sportsmenView->setModel(sportsmenModel);
    ui->sportsmenView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sportsmenView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sportsmenView->setColumnHidden(SportsmanWithSports_Id, true);
    for (int i = SportsmanWithSports_SportId; i <= SportsmanWithSports_Title; ++i)
        ui->sportsmenView->setColumnHidden(i, true);
    ui->sportsmenView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->sportsmenView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(updateSportsmanCoachesView()));

    sportsmanCoachesModel = new QSqlQueryModel();
    sportsmanCoachesModel->setQuery("SELECT DISTINCT [Coach Firstname], [Coach Lastname] FROM SportsmenWithCoaches");
    ui->sportsmanCoachesView->setModel(sportsmanCoachesModel);
    ui->sportsmanCoachesView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sportsmanCoachesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sportsmanCoachesView->horizontalHeader()->setStretchLastSection(true);

    ui->sportComboBox->setModel(sportsModel);
    ui->sportComboBox->setModelColumn(Sport_Name);
    connect(ui->sportComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applySportFilter()));

    QSqlQueryModel* experienceModel = new QSqlQueryModel(this);
    experienceModel->setQuery("SELECT title FROM Experience GROUP BY title");
    ui->qualificationComboBox->setModel(experienceModel);
    ui->qualificationComboBox->setModelColumn(0);
    connect(ui->qualificationComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applyQualificationFilter()));

    allCoaches = new QSqlQueryModel(this);
    allCoaches->setQuery("SELECT DISTINCT [CoachId], [Coach Firstname], [Coach Lastname] FROM SportsmenWithCoaches");
    ui->coachFilterComboBox->setModel(allCoaches);
    ui->coachFilterComboBox->setModelColumn(2);
    connect(ui->coachFilterComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applyCoachFilter()));

    QDate today = QDate::currentDate();
    ui->startDateEdit->setCalendarPopup(true);
    ui->startDateEdit->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    ui->endDateEdit->setCalendarPopup(true);
    ui->endDateEdit->setDateRange(today.addDays(-365*2), today.addDays(365*2));
    connect(ui->startDateEdit, SIGNAL(dateChanged(QDate)), SLOT(applyDateFilter()));
    connect(ui->endDateEdit, SIGNAL(dateChanged(QDate)), SLOT(applyDateFilter()));

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->sportFilterCheckbox);
    buttonGroup->addButton(ui->qualificationFilterCheckbox);
    buttonGroup->addButton(ui->coachFilterCheckbox);
    buttonGroup->addButton(ui->competitionsFilterCheckbox);
    buttonGroup->addButton(ui->multipleSportsFilterCheckbox);
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

void MainTabWindow::updateSportsmanCoachesView()
{
    QModelIndex index = ui->sportsmenView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = sportsmenModel->record(index.row());

        sportsmanCoachesModel->setQuery(QString("EXEC CoachesOfSportsman @firstname = %1, @lastname = %2")
                                        .arg(record.value(SportsmanWithSports_Firstname).toString())
                                        .arg(record.value(SportsmanWithSports_Lastname).toString()));
    }
}

void MainTabWindow::applySportFilter()
{
    QString sportName = ui->sportComboBox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithParticularSport @sportName = %1").arg(sportName));
}

void MainTabWindow::applyCoachFilter()
{
    int row = ui->coachFilterComboBox->currentIndex();
    QString firstname = allCoaches->record(row).value(1).toString();
    QString lastname = allCoaches->record(row).value(2).toString();
    sportsmenModel->setQuery(QString("EXEC SportsmenOfCoach @firstname = %1, @lastname = %2").arg(firstname).arg(lastname));
}

void MainTabWindow::applyQualificationFilter()
{
    QString title = ui->qualificationComboBox->currentText();
    sportsmenModel->setQuery(QString("EXEC SportsmenWithQualification @sportTitle = %1").arg(title));
}

void MainTabWindow::applyDateFilter()
{
    QDate startDate = ui->startDateEdit->date();
    QDate endDate = ui->endDateEdit->date();
    sportsmenModel->setQuery(QString("EXEC ThoseWhoDidntTakePartInCompetitions @from = %1, @to = %2")
                             .arg(startDate.toString("yyyy-MM-dd"))
                             .arg(endDate.toString("yyyy-MM-dd")));

}

void MainTabWindow::on_sportFilterCheckbox_stateChanged(int state)
{
    ui->sportComboBox->setEnabled(state);
}

void MainTabWindow::on_qualificationFilterCheckbox_stateChanged(int state)
{
    ui->qualificationComboBox->setEnabled(state);
}

void MainTabWindow::on_coachFilterCheckbox_stateChanged(int state)
{
    ui->coachFilterComboBox->setEnabled(state);
}

void MainTabWindow::on_competitionsFilterCheckbox_stateChanged(int state)
{
    ui->startDateEdit->setEnabled(state);
    ui->endDateEdit->setEnabled(state);
}

void MainTabWindow::on_sportsmenResetFilters_clicked()
{
    ui->sportFilterCheckbox->setChecked(false);
    ui->coachFilterCheckbox->setChecked(false);
    ui->competitionsFilterCheckbox->setChecked(false);
    ui->qualificationFilterCheckbox->setChecked(false);
    ui->multipleSportsFilterCheckbox->setChecked(false);
    sportsmenModel->setQuery("SELECT DISTINCT * FROM SportsmenWithSports");
}

void MainTabWindow::on_multipleSportsFilterCheckbox_stateChanged(int state)
{
    if (state == Qt::Checked)
        sportsmenModel->setQuery("EXEC ThoseWhoStudyMoreThanOneSport");
}
