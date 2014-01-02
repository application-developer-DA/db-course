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

signals:
    void logout();

private slots:
    void on_disconnectButton_clicked();
    void on_addSportBtn_clicked();
    void on_deleteSportBtn_clicked();

    void updateSportCoachesView();

private:
    /* Tabs filling functions */
    void fillSports();                      // Configure 1 tab
    void fillSportsmen();                   // Configure 2 tab
    void fillBuildingAndOrganizations();    // Configure 3 tab
    void fillCompetitions();                // Configure 4 tab

    QSqlTableModel* sportsModel;
    QSqlQueryModel* coachesModel;

    Ui::MainTabWindow* ui;
};

