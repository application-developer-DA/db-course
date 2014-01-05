#include "personeditform.h"

enum {
    Person_Id,
    Person_Firstname,
    Person_Lastname,
    Person_Middlename,
    Person_Birthdate
};

enum {
    Experience_Id,
    Experience_PersonId,
    Experience_SportId,
    Experience_Title
};

PersonEditForm::PersonEditForm(int id, bool isCoach, QWidget* parent)
    : QDialog(parent)
    , displayCoachWidgets(isCoach)
{
    firstnameEdit = new QLineEdit;
    QLabel* firstnameLabel = new QLabel(tr("Name:"));

    lastnameEdit = new QLineEdit;
    QLabel* lastnameLabel = new QLabel(tr("Last Name:"));

    middlenameEdit = new QLineEdit;
    QLabel* middlenameLabel = new QLabel(tr("Middle Name:"));

    birthEdit = new QDateEdit;
    birthEdit->setCalendarPopup(true);
    QLabel* birthLabel = new QLabel(tr("Birth Date:"));

    QPushButton* addButton = new QPushButton(tr("&Add"));
    QPushButton* deleteButton = new QPushButton(tr("&Delete"));
    QPushButton* closeButton = new QPushButton(tr("&Close"));

    QDialogButtonBox* buttonBox = new QDialogButtonBox;
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::AcceptRole);

    personModel = new QSqlTableModel(this);
    personModel->setTable("Person");
    personModel->select();

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(personModel);
    mapper->addMapping(firstnameEdit, Person_Firstname);
    mapper->addMapping(lastnameEdit, Person_Lastname);
    mapper->addMapping(middlenameEdit, Person_Middlename);
    mapper->addMapping(birthEdit, Person_Birthdate);

    if (id != -1) {
        for (int row = 0; row < personModel->rowCount(); ++row) {
            QSqlRecord record = personModel->record(row);
            if (record.value(Person_Id).toInt() == id) {
                mapper->setCurrentIndex(row);
                break;
            }
        }
    } else {
        mapper->toFirst();
    }

    connect(addButton, SIGNAL(clicked()), SLOT(addPerson()));
    connect(deleteButton, SIGNAL(clicked()), SLOT(deletePerson()));
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));

    QGridLayout* personLayout = new QGridLayout;
    personLayout->addWidget(firstnameLabel, 1, 0);
    personLayout->addWidget(firstnameEdit, 1, 1);
    personLayout->addWidget(lastnameLabel, 2, 0);
    personLayout->addWidget(lastnameEdit, 2, 1);
    personLayout->addWidget(middlenameLabel, 3, 0);
    personLayout->addWidget(middlenameEdit, 3, 1);
    personLayout->addWidget(birthLabel, 4, 0);
    personLayout->addWidget(birthEdit, 4, 1);
    personLayout->addWidget(buttonBox, 6, 0, 1, 3);

    experienceModel = new QSqlRelationalTableModel(this);
    experienceModel->setTable("Experience");
    experienceModel->setRelation(Experience_SportId, QSqlRelation("Sport", "id", "name"));
    experienceModel->setHeaderData(Experience_SportId, Qt::Horizontal, "Sport");
    experienceModel->setHeaderData(Experience_Title, Qt::Horizontal, "Title");
    updateExperienceModel();

    // connect(experienceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(updateExperienceModel()));

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

    QSpacerItem* spacer = new QSpacerItem(10, 20);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(personLayout);
    mainLayout->addItem(spacer);
    mainLayout->addLayout(experienceLayout);

    setLayout(mainLayout);

    setWindowTitle(tr("Edit Person Information"));
}

void PersonEditForm::updateExperienceModel()
{
    experienceModel->setFilter(QString("person_id = %1").arg(personModel->record(mapper->currentIndex()).value(Person_Id).toInt()));
    experienceModel->select();
}

void PersonEditForm::done(int result)
{
    mapper->submit();
    QDialog::done(result);
}

void PersonEditForm::addPerson()
{
    int row = mapper->currentIndex();
    mapper->submit();
    personModel->insertRow(row);
    mapper->setCurrentIndex(row);

    firstnameEdit->clear();
    lastnameEdit->clear();
    middlenameEdit->clear();
    birthEdit->setDate(QDate::currentDate());
    firstnameEdit->setFocus();

    updateExperienceModel();
}

void PersonEditForm::deletePerson()
{
    int row = mapper->currentIndex();
    personModel->removeRow(row);
    mapper->submit();
    mapper->setCurrentIndex(qMin(row, personModel->rowCount() - 1));

    updateExperienceModel();

    if (displayCoachWidgets)
        accept();
}

void PersonEditForm::addExperience()
{
    int row = experienceModel->rowCount();
    experienceModel->insertRow(row);

    QModelIndex index = experienceModel->index(row, Experience_PersonId);
    experienceModel->setData(index, personModel->record(mapper->currentIndex()).value(Person_Id).toInt());

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
