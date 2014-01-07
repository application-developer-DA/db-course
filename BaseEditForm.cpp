#include "BaseEditForm.h"
#include "QDateEditSqlDelegate.h"

#include <QtWidgets>
#include <QtSql>

#include <QVector>

BaseEditForm::BaseEditForm(int id, const QString& tableName, const QVector<Relation> relations, const QVector<WidgetMapping> &mappings, QWidget* parent)
    : QDialog(parent)
{
    model = new QSqlRelationalTableModel(this);
    model->setTable(tableName);

    foreach (Relation relation, relations) {
        model->setRelation(relation.column, relation.relation);
    }
    model->select();

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    foreach (WidgetMapping mapping, mappings) {
        QLabel* label = new QLabel(mapping.title);
        QWidget* widget;
        switch (mapping.type) {
        case LineEdit:
            widget = new QLineEdit;
            break;
        case ComboBox: {
            auto relationModel = model->relationModel(mapping.mapToColumn);
            widget = new QComboBox;
            ((QComboBox*)widget)->setModel(relationModel);
            ((QComboBox*)widget)->setModelColumn(relationModel->fieldIndex(mapping.value.toString()));
        }
            break;
        case DateEdit:
            widget = new QDateEdit;
            ((QDateEdit*)widget)->setCalendarPopup(true);
            mapper->setItemDelegate(new QDateEditSqlDelegate(mapping.mapToColumn, this));
            break;
        default:
            Q_ASSERT(0);
        }
        widgets << label << widget;

        mapper->addMapping(widget, mapping.mapToColumn);
    }

    Q_ASSERT(widgets.size() % 2 == 0);
    QGridLayout* widgetsLayout = new QGridLayout;
    for (int i = 0; i < widgets.size(); i += 2) {
        widgetsLayout->addWidget(widgets.at(i), i+1, 0);
        widgetsLayout->addWidget(widgets.at(i+1), i+1, 1);
    }

    QPushButton* addButton = new QPushButton(tr("&Add"));
    QPushButton* deleteButton = new QPushButton(tr("&Delete"));
    QPushButton* closeButton = new QPushButton(tr("&Close"));

    QDialogButtonBox* buttonBox = new QDialogButtonBox;
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::AcceptRole);

    connect(addButton, SIGNAL(clicked()), SLOT(onAddButton()));
    connect(deleteButton, SIGNAL(clicked()), SLOT(onDeleteButton()));
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));

    widgetsLayout->addWidget(buttonBox, widgets.size() + 1, 0, 1, 3);

    /* Set mapper to display entry with chosen id */
    if (id != -1) {
        for (int row = 0; row < model->rowCount(); ++row) {
            QSqlRecord record = model->record(row);
            if (record.value(0).toInt() == id) {
                mapper->setCurrentIndex(row);
                break;
            }
        }
    } else {
        mapper->toFirst();
    }

    /* Create navigation buttons */
    QPushButton* firstButton = new QPushButton(tr("<< &First"));
    QPushButton* previousButton = new QPushButton(tr("< &Previous"));
    QPushButton* nextButton = new QPushButton(tr("&Next >"));
    QPushButton* lastButton = new QPushButton(tr("&Last >>"));

    connect(firstButton, SIGNAL(clicked()), mapper, SLOT(toFirst()));
    connect(previousButton, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
    connect(nextButton, SIGNAL(clicked()), mapper, SLOT(toNext()));
    connect(lastButton, SIGNAL(clicked()), mapper, SLOT(toLast()));

    QHBoxLayout* topButtonLayout = new QHBoxLayout;
    topButtonLayout->setContentsMargins(20, 0, 20, 5);
    topButtonLayout->addStretch();
    topButtonLayout->addWidget(firstButton);
    topButtonLayout->addWidget(previousButton);
    topButtonLayout->addWidget(nextButton);
    topButtonLayout->addWidget(lastButton);
    topButtonLayout->addStretch();

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topButtonLayout);
    mainLayout->addLayout(widgetsLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("Edit Dialog"));
}

void BaseEditForm::done(int result)
{
    mapper->submit();
    QDialog::done(result);
}

void BaseEditForm::onAddButton()
{
    int row = 0;
    if (model->rowCount() > 0) {
        row = mapper->currentIndex();
        mapper->submit();
    }
    model->insertRow(row);
    mapper->setCurrentIndex(row);

    clearWidgets();
}

void BaseEditForm::onDeleteButton()
{
    int row = mapper->currentIndex();
    model->removeRow(row);
    mapper->submit();
    mapper->setCurrentIndex(qMin(row, model->rowCount() - 1));

    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Deletion", "Nothing to remove");
        return;
    }

    QMessageBox::information(this, "Deletion", "Deletion has been completed.");
}

void BaseEditForm::clearWidgets()
{
    foreach (QWidget* widget, widgets) {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
        if (lineEdit) {
            lineEdit->setText("");
            continue;
        }
        QDateEdit* dateEdit = qobject_cast<QDateEdit*>(widget);
        if (dateEdit) {
            dateEdit->setDate(QDate::currentDate());
            continue;
        }
        QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
        if (comboBox) {
            comboBox->setCurrentIndex(0);
            continue;
        }
    }
}
