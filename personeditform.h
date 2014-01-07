#pragma once

#include "BaseEditForm.h"

class QTableView;

class PersonEditForm : public BaseEditForm
{
    Q_OBJECT

public:
    explicit PersonEditForm(int id, const QString &tableName, const QVector<Relation> relations,
                            const QVector<WidgetMapping> &mappings, QWidget *parent = nullptr);

private slots:
    void addExperience();
    void deleteExperience();

    void updateExperienceModel();

private:
    QSqlRelationalTableModel* experienceModel;
    QTableView* experienceView;
};

