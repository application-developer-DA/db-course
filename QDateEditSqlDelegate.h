#pragma once

#include <QObject>
#include <QString>
#include <QSqlRelationalDelegate>
#include <QDateEdit>
#include <QDate>

class QDateEditSqlDelegate : public QSqlRelationalDelegate
{
    Q_OBJECT
public:
    QDateEditSqlDelegate(int dateEditColumn, QObject* parent = nullptr) : QSqlRelationalDelegate(parent), dateEditColumn(dateEditColumn)
    { }

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
    {
        if (index.column() == dateEditColumn) {
            QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor);
            QDate date = dateEdit->date();
            QString dateStr = date.toString("yyyy-MM-dd");
            model->setData(index, dateStr);
        } else {
            QSqlRelationalDelegate::setModelData(editor, model, index);
        }
    }

private:
    int dateEditColumn;
};
