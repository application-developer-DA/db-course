#pragma once

#include <memory>

#include <QDebug>
#include <QList>
#include <QWidget>

namespace Ui {
class MainTabWindow;
}

class QSqlTableModel;
class QSqlQueryModel;
class QSqlRelationalTableModel;
class QTableView;

class MainTabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainTabWindow(QWidget* parent = nullptr);
    ~MainTabWindow();

    void login();

private slots:
    void updateSportCoachesView();
    void updateSportsmanCoachesView();
    void updateWinnersView();

    void on_addSportBtn_clicked();
    void on_deleteSportBtn_clicked();
    void on_editCoachBtn_clicked();

    void on_enableSportsmenFilters_stateChanged(int state);
    void on_sportsmenSportFilter_toggled(bool checked);
    void on_sportsmenQualificationFilter_toggled(bool checked);
    void on_sportsmenCoachFilter_toggled(bool checked);
    void on_sportsmenCompetitionFilter_toggled(bool checked);
    void on_sportsmenMultipleSportsFilter_toggled(bool checked);
    void on_editSportsmanBtn_clicked();
    void on_sportsmanCoachEditBtn_clicked();

    void on_enableBuildingFilters_stateChanged(int state);
    void on_constructionTypeFilter_toggled(bool checked);
    void on_constructionPlacesFilter_toggled(bool checked);
    void on_constructionCompetitionsFilter_toggled(bool checked);
    void on_editSportConstructionBtn_clicked();
    void on_organizationEditBtn_clicked();

    void on_enableCompetitionFilters_stateChanged(int state);
    void on_competitionDateFilter_toggled(bool checked);
    void on_competitionOrganizerFilter_toggled(bool checked);
    void on_competitionSportFilter_toggled(bool checked);
    void on_competitionConstructionFilter_toggled(bool checked);
    void on_editClubsBtn_clicked();
    void on_editCompetitionsBtn_clicked();

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

    void lol()
    {
        qDebug() << "lol";
    }

private:
    /* Tabs filling functions */
    void fillSports();                           // Configure 1 tab
    void fillSportsmen();                        // Configure 2 tab
    void fillConstructionsAndOrganizations();    // Configure 3 tab
    void fillCompetitions();                     // Configure 4 tab

    void setDefaultViewParameters(QTableView* view);

    QList<QWidget*> competitionFilterWidgets;
    QList<QWidget*> constructionFilterWidgets;
    QList<QWidget*> sportsmenFilterWidgets;

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
    QSqlQueryModel* experienceModel;
    QSqlQueryModel* constructionType;
    QSqlQueryModel* constructions;
    QSqlQueryModel* organizers;
};

