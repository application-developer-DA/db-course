#include "personeditform.h"

enum {
    Person_Id,
    Person_Firstname,
    Person_Lastname,
    Person_Middlename,
    Person_Birthdate
};

PersonEditForm::PersonEditForm(int id, bool isCoach, QWidget* parent)
    : QDialog(parent)
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
    setLayout(personLayout);

    setWindowTitle(tr("Edit Person Information"));
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
}

void PersonEditForm::deletePerson()
{
    int row = mapper->currentIndex();
    personModel->removeRow(row);
    mapper->submit();
    mapper->setCurrentIndex(qMin(row, personModel->rowCount() - 1));
}
