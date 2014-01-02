#pragma once

#include <QWidget>

namespace Ui {
class MainTabWindow;
}

class QSqlTableModel;
class QSqlQueryModel;

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

    void updateSportCoachesView();
    void updateSportsmanCoachesView();

    void on_sportFilterCheckbox_stateChanged(int state);
    void on_qualificationFilterCheckbox_stateChanged(int state);
    void on_coachFilterCheckbox_stateChanged(int state);
    void on_competitionsFilterCheckbox_stateChanged(int state);
    void on_multipleSportsFilterCheckbox_stateChanged(int state);

    void applySportFilter();
    void applyQualificationFilter();
    void applyCoachFilter();
    void applyDateFilter();

    void on_sportsmenResetFilters_clicked();

private:
    /* Tabs filling functions */
    void fillSports();                      // Configure 1 tab
    void fillSportsmen();                   // Configure 2 tab
    void fillBuildingAndOrganizations();    // Configure 3 tab
    void fillCompetitions();                // Configure 4 tab

    Ui::MainTabWindow* ui;

    QSqlTableModel* sportsModel;
    QSqlQueryModel* coachesModel;
    QSqlQueryModel* sportsmenModel;
    QSqlQueryModel* sportsmanCoachesModel;

    /* Auxiliary models */
    QSqlQueryModel* allCoaches;
};

