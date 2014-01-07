#include "CompetitionEditForm.h"

#include <QtWidgets>
#include <QtSql>

static const QString kCompetitionParticipantsQuery =
        "SELECT [Participant Id], [Firstname] + ' ' + [Lastname] + ' ' + [Middlename] AS Participant, [Club], [Scores] FROM AllParticipants WHERE AllParticipants.[Competition Id] = %1";

CompetitionEditForm::CompetitionEditForm(int id, const QString &tableName, const QVector<Relation> relations, const QVector<WidgetMapping> &mappings, QWidget *parent)
    : BaseEditForm(id, tableName, relations, mappings, parent)
{
    if (id == -1)
        id = model->record(mapper->currentIndex()).value(0).toInt();

    participantsModel = new QSqlQueryModel(this);
    participantsModel->setQuery(QString(kCompetitionParticipantsQuery).arg(id));

    participantsView = new QTableView(this);
    participantsView->setModel(participantsModel);
    participantsView->setColumnHidden(0, true);
    participantsView->resizeColumnsToContents();
    participantsView->horizontalHeader()->setStretchLastSection(true);
    participantsView->setSelectionMode(QAbstractItemView::SingleSelection);
    participantsView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QPushButton* editParticipantBtn = new QPushButton(tr("&Edit Participant"));
    connect(editParticipantBtn, SIGNAL(clicked()), SLOT(onParticipantEdit()));

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(participantsView, 1, 0, 1, 3);
    layout->addWidget(editParticipantBtn, 2, 2);

    mainLayout->addSpacing(20);
    mainLayout->addLayout(layout);

    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateParticipants()));
}

void CompetitionEditForm::onParticipantEdit()
{
    QModelIndex index = participantsView->currentIndex();
    int id = -1;
    if (index.isValid()) {
        QSqlRecord record = participantsModel->record(index.row());
        id = record.value(0).toInt();
    }

    QVector<WidgetMapping> mappings {
        { "Participant:", ComboBox, QVariant("Name"), 1 },
        { "Competition:", ComboBox, QVariant("name"), 2 },
        { "Club:",        ComboBox, QVariant("name"), 3 },
        { "Results:",     LineEdit, QVariant(),       4 }
    };

    QVector<Relation> relations {
        { 1, QSqlRelation("PersonFullNames", "Id", "Name") },
        { 2, QSqlRelation("Competition", "id", "name") },
        { 3, QSqlRelation("Club", "id", "name") }
    };

    BaseEditForm form(id, "Participant", relations, mappings, this);
    form.exec();

    updateParticipants();
}

void CompetitionEditForm::updateParticipants()
{
    int id = model->record(mapper->currentIndex()).value(0).toInt();
    participantsModel->setQuery(QString(kCompetitionParticipantsQuery).arg(id));
}
