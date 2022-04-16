#ifndef VINICIUSQL_H
#define VINICIUSQL_H
#include <QString>
#include <QVariantMap>

#include "viniciusql_global.h"

class VINICIUSQL_EXPORT Viniciusql
{
public:
    //Attributes
    QString table;

    //Getters e Setters
    QStringList getColumns();

    //Constructors
    Viniciusql(QString table);

    //Sql Methods
    int insert(QVariantMap maps);
    Viniciusql* select(QStringList columns);
    Viniciusql* update(QVariantMap maps);
    Viniciusql* destroy();

    //Specifiers
    Viniciusql* equals(QString column, QVariant value);
    Viniciusql* like(QString column, QVariant value);

    //Logical Operators
    Viniciusql* _and();
    Viniciusql* _or();

    //finisher
    QList<QVariantMap> finishSelect();
    bool finish();

private:
    //Attributes
    QStringList columns;
    QStringList selectColumns;
    QVariantList values;
    QString query;

    //Getters e Setters
    void setColumns();

};

#endif // VINICIUSQL_H
