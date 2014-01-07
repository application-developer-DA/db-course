#include "PersonEditForm.h"

#include <QtWidgets>
#include <QtSql>

enum {
    Experience_Id,
    Experience_PersonId,
    Experience_SportId,
    Experience_Title
};

static const QString kSportsmenOfCoach =
        "SELECT [Id], [Learner Firstname] + ' ' + [Learner Lastname] + ' ' + [Learner Middlename] AS [Learner Name], [Sport] FROM SportsmenWithCoaches WHERE SportsmenWithcoaches.[Coach Id] = %1";

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

    /* Learner table editing etc. */
    learnerModel = new QSqlQueryModel(this);
    learnerModel->setQuery(QString(kSportsmenOfCoach).arg(id));

    learnerView = new QTableView;
    learnerView->setModel(learnerModel);
    learnerView->setSelectionMode(QAbstractItemView::SingleSelection);
    learnerView->setSelectionBehavior(QAbstractItemView::SelectRows);
    learnerView->horizontalHeader()->setStretchLastSection(true);
    learnerView->setColumnHidden(0, true);

    QPushButton* editLearnerButton = new QPushButton("Edit Learner");
    connect(editLearnerButton, SIGNAL(clicked()), SLOT(editLearner()));

    QGridLayout* experienceLayout = new QGridLayout;
    experienceLayout->addWidget(experienceView, 1, 0, 1, 3);
    experienceLayout->addWidget(deleteExperienceButton, 2, 1);
    experienceLayout->addWidget(addExperienceButton, 2, 2);

    QGridLayout* learnerLayout = new QGridLayout;
    learnerLayout->addWidget(learnerView, 1, 0, 1, 3);
    learnerLayout->addWidget(editLearnerButton, 2, 2);

    mainLayout->addSpacing(20);
    mainLayout->addLayout(experienceLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(learnerLayout);

    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateExperienceModel()));
    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateLearnerModel()));
}

void PersonEditForm::updateExperienceModel()
{
    experienceModel->setFilter(QString("person_id = %1").arg(model->record(mapper->currentIndex()).value(0).toInt()));
    experienceModel->select();
}

void PersonEditForm::updateLearnerModel()
{
    int id = model->record(mapper->currentIndex()).value(0).toInt();
    learnerModel->setQuery(QString(kSportsmenOfCoach).arg(id));
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

void PersonEditForm::editLearner()
{
    QModelIndex index = learnerView->currentIndex();
    int id = -1;
    if (index.isValid()) {
        QSqlRecord record = learnerModel->record(index.row());
        id = record.value(0).toInt();
    }

    QVector<WidgetMapping> mappings {
        { "Coach:",    ComboBox, QVariant("Name"), 1 },
        { "Learner:",  ComboBox, QVariant("Name"), 2 },
        { "Sport:",    ComboBox, QVariant("name"), 3 },
        { "Club:",     ComboBox, QVariant("name"), 4 }
    };

    QVector<Relation> relations {
        { 1, QSqlRelation("PersonFullNames", "Id", "Name") },
        { 2, QSqlRelation("PersonFullNames", "Id", "Name") },
        { 3, QSqlRelation("Sport", "id", "name") },
        { 4, QSqlRelation("Club", "id", "name") }
    };

    BaseEditForm form(id, "Learner", relations, mappings, this);
    form.exec();

    updateLearnerModel();
}
