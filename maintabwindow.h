#pragma once

#include <memory>

#include <QList>
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

    void login();

private slots:
    void on_addSportBtn_clicked();
    void on_deleteSportBtn_clicked();

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
    void on_constructionTypeFilter_toggled(bool checked);
    void on_constructionPlacesFilter_toggled(bool checked);
    void on_constructionCompetitionsFilter_toggled(bool checked);

    void on_enableCompetitionFilters_stateChanged(int state);
    void on_competitionDateFilter_toggled(bool checked);
    void on_competitionOrganizerFilter_toggled(bool checked);
    void on_competitionSportFilter_toggled(bool checked);
    void on_competitionConstructionFilter_toggled(bool checked);

    /* Sportsmen filters */
    void applySportsmanSportFilter();
    void applySportsmanQualificationFilter();
    void applySportsmanCoachFilter();
    void applySportsmanDateFilter();

    /* Construction filters */
    void applyConstructionPlacesFilter();
    void applyConstructionTypeFilter();
    void applyConstructionCompetitionFilter();

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

    QList<QWidget*> competitionFilterWidgets;
    QList<QWidget*> constructionFilterWidgets;

    Ui::MainTabWindow* ui;

    QSqlTableModel* sportsModel;
    QSqlQueryModel* coachesModel;

    QSqlQueryModel* sportsmenModel;
    QSqlQueryModel* sportsmanCoachesModel;

    QSqlQueryModel* constructionsModel;
    QSqlQueryModel* organizationsModel;

    QSqlQueryModel* competitionsModel;
    QSqlQueryModel* competitionWinnersModel;
    QSqlQueryModel* competitionsClubs;

    /* Auxiliary models */
    QSqlQueryModel* allCoaches;
};

