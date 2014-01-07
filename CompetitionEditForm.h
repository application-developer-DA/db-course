#pragma once

#include "BaseEditForm.h"

class QTableView;

class CompetitionEditForm : public BaseEditForm
{
    Q_OBJECT

public:
    CompetitionEditForm(int id, const QString &tableName, const QVector<Relation> relations,
                        const QVector<WidgetMapping> &mappings, QWidget *parent = nullptr);

private slots:
    void onParticipantEdit();

private:
    QSqlQueryModel* participantsModel;
    QTableView*     participantsView;
};
