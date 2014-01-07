#include "PersonEditForm.h"

#include <QtWidgets>
#include <QtSql>

enum {
    Experience_Id,
    Experience_PersonId,
    Experience_SportId,
    Experience_Title
};

PersonEditForm::PersonEditForm(int id, const QString &tableName, const QVector<Relation> relations, const QVector<WidgetMapping> &mappings, QWidget *parent)
    : BaseEditForm(id, tableName, relations, mappings, parent)
{
    /* Experience table editing etc. */
    experienceModel = new QSqlRelationalTableModel(this);
    experienceModel->setTable("Experience");
    experienceModel->setRelation(Experience_SportId, QSqlRelation("Sport", "id", "name"));
    experienceModel->setHeaderData(Experience_SportId, Qt::Horizontal, "Sport");
    experienceModel->setHeaderData(Experience_Title, Qt::Horizontal, "Title");
    updateExperienceModel();

    experienceView = new QTableView;
    experienceView->setModel(experienceModel);
    experienceView->setItemDelegate(new QSqlRelationalDelegate(this));
    experienceView->setSelectionMode(QAbstractItemView::SingleSelection);
    experienceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    experienceView->setColumnHidden(Experience_Id, true);
    experienceView->setColumnHidden(Experience_PersonId, true);
    experienceView->horizontalHeader()->setStretchLastSection(true);

    QPushButton* addExperienceButton = new QPushButton(tr("Add"));
    QPushButton* deleteExperienceButton = new QPushButton(tr("Remove"));

    connect(addExperienceButton, SIGNAL(clicked()), SLOT(addExperience()));
    connect(deleteExperienceButton, SIGNAL(clicked()), SLOT(deleteExperience()));

    QGridLayout* experienceLayout = new QGridLayout;
    experienceLayout->addWidget(experienceView, 1, 0, 1, 3);
    experienceLayout->addWidget(deleteExperienceButton, 2, 1);
    experienceLayout->addWidget(addExperienceButton, 2, 2);

    mainLayout->addSpacing(20);
    mainLayout->addLayout(experienceLayout);

    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateExperienceModel()));
}

void PersonEditForm::updateExperienceModel()
{
    experienceModel->setFilter(QString("person_id = %1").arg(model->record(mapper->currentIndex()).value(0).toInt()));
    experienceModel->select();
}

void PersonEditForm::addExperience()
{
    int row = experienceModel->rowCount();
    experienceModel->insertRow(row);

    QModelIndex index = experienceModel->index(row, Experience_PersonId);
    experienceModel->setData(index, model->record(mapper->currentIndex()).value(0).toInt());

    index = experienceModel->index(row, Experience_SportId);
    experienceView->setCurrentIndex(index);
    experienceView->edit(index);
}

void PersonEditForm::deleteExperience()
{
    int row = experienceView->currentIndex().row();
    experienceModel->removeRow(row);

    updateExperienceModel();
}
