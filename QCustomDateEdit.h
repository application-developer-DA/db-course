#pragma once

#include <QDateEdit>

class QCustomDateEdit : public QDateEdit
{
    Q_OBJECT
    Q_PROPERTY(QString date READ date WRITE setDate NOTIFY userDateChanged USER true)

public:
    QCustomDateEdit(QWidget* parent = nullptr) : QDateEdit(parent)
    { }

    QCustomDateEdit(const QDate& date, QWidget* parent = nullptr) : QDateEdit(date, parent)
    { }

    void setDate(const QString& date)
    {
        QDate dateRepresentation;
        dateRepresentation.fromString(date, "yyyy-MM-dd");
        QDateEdit::setDate(dateRepresentation);
        emit userDateChanged(date);
    }

    QString date() const
    {
        return QDateEdit::date().toString("yyyy-MM-dd");
    }

Q_SIGNALS:
    void userDateChanged(const QString& date);
};
