#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
class MainTabWindow;
}

class QSqlTableModel;
class QSqlQueryModel;
class QSqlRelationalTableModel;

class MainTabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainTabWindow(QWidget* parent = nullptr);
    ~MainTabWindow();

    void loggedIn(const QString& username);

private slots:
    void on_addSportBtn_clicked();
    void on_deleteSportBtn_clicked();
    void on_addConstruction_clicked();
    void on_deleteConstruction_clicked();

    void updateSportCoachesView();
    void updateSportsmanCoachesView();
    void updateWinnersView();

    void on_sportFilterCheckbox_stateChanged(int state);
    void on_qualificationFilterCheckbox_stateChanged(int state);
    void on_coachFilterCheckbox_stateChanged(int state);
    void on_competitionsFilterCheckbox_stateChanged(int state);
    void on_multipleSportsFilterCheckbox_stateChanged(int state);
    void on_sportsmenResetFilters_clicked();
    void on_enableBuildingFilters_stateChanged(int state);

    /* Sportsmen filters */
    void applySportFilter();
    void applyQualificationFilter();
    void applyCoachFilter();
    void applyDateFilter();

    /* Construction filters */
    void applyPlacesFilter();
    void applyTypeFilter();

    /* Competition filters */
    void applyCompetitionConstructionFilter();
    void applyCompetitionSportFilter();
    void applyCompetitionOrganizerFilter();
    void applyCompetitionDateFilter();

private:
    /* Tabs filling functions */
    void fillSports();                           // Configure 1 tab
    void fillSportsmen();                        // Configure 2 tab
    void fillConstructionsAndOrganizations();    // Configure 3 tab
    void fillCompetitions();                     // Configure 4 tab

    Ui::MainTabWindow* ui;

    QSqlTableModel* sportsModel;
    QSqlQueryModel* coachesModel;
    QSqlQueryModel* sportsmenModel;
    QSqlQueryModel* sportsmanCoachesModel;
    QSqlRelationalTableModel* constructionsModel;
    QSqlQueryModel* competitionsModel;
    QSqlQueryModel* competitionWinnersModel;

    /* Auxiliary models */
    QSqlQueryModel* allCoaches;
};

