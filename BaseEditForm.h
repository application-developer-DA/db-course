#pragma once

#include <QDialog>
#include <QSqlRelation>
#include <QVector>

class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QWidget;

class BaseEditForm : public QDialog
{
    Q_OBJECT
public:
    enum WidgetType {
        LineEdit = 0,
        ComboBox,
        DateEdit
    };

    struct WidgetMapping {
        QString     title;
        WidgetType  type;
        QVariant    value;
        int         mapToColumn;
    };

    struct Relation {
        int          column;
        QSqlRelation relation;
    };

    explicit BaseEditForm(int id, const QString& tableName, const QVector<Relation> relations,
                          const QVector<WidgetMapping>& mappings, QWidget* parent = nullptr);

    void done(int result);

private slots:
    void onAddButton();
    void onDeleteButton();

private:
    QSqlRelationalTableModel* model;
    QDataWidgetMapper* mapper;
    QVector<QWidget*> widgets;
};

