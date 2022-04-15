#include "viniciusql.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>

QStringList Viniciusql::getColumns()
{
    return this->selectColumns;
}

Viniciusql::Viniciusql(QString table)
{
    this->table = table;
    setColumns();
}

void Viniciusql::setColumns()
{
    this->query = "pragma table_info ('" + this->table + "')";
    QSqlQuery q(this->query);
    bool result = q.exec();
    if(result){
        while(q.next()){
            this->columns.append(q.record().value(1).toString());
        }
        qDebug() << "columns = " + this->columns.join(",");
    }
    query.clear();
    q.~QSqlQuery();
}

int Viniciusql::insert(QVariantMap maps)
{
    int cont = 0;
    QStringList columns;
    QVariantList values;
    columns = maps.keys();
    values = maps.values();
    this->query =  "insert into " + this->table + "(";
    this->query += columns.join(",") + ") values(";
    while(cont < values.size()){
        this->query += "?";
        if(cont != values.size() -1)
            this->query += ",";
        else
            this->query += ")";
        cont++;
    }
    qDebug() << "query: " <<query;
    QSqlQuery q(this->query);
    foreach(QVariant value, values){
        q.addBindValue(value);
    }
    if(q.exec()){
        this->query.clear();
        return q.lastInsertId().toInt();
        q.~QSqlQuery();

    }
    else{
        this->query.clear();
        q.~QSqlQuery();
        return -1;

    }
    return -1;
}
Viniciusql* Viniciusql::select(QStringList columns)
{
    if(columns.size() == 1 && columns.at(0) == "*"){
        qDebug() << "query1 " + this->query;
        this->query = "select * from " + this->table;
        this->selectColumns = this->columns;
    }
    else if(columns.size() >= 1 && !columns.count("*")){
        this->query = "select " + columns.join(",") + " from " + this->table;
        qDebug() << "query2 " + this->query;
        this->selectColumns = columns;
    }
    else
        qDebug() << "Dados de columns são inválidos.";
    return this;

}

Viniciusql* Viniciusql::equals(QString column, QVariant value)
{
    if(this->query.contains("where"))
        this->query += column + " = " + value.toString();
    else
        this->query += " where " + column + " = " + value.toString();
    qDebug() << "query com where " + this->query;
    return this;

}

Viniciusql *Viniciusql::like(QString column, QVariant value)
{
    if(this->query.contains("where"))
        this->query += column + " like " + "%" +  value.toString() + "%";
    else
        this->query += " where " + column + " like " + "%" +  value.toString() + "%";
    qDebug() << "query com where " + this->query;
    return this;
}

Viniciusql* Viniciusql::_and()
{
    this->query += " and ";
    return this;
}

Viniciusql* Viniciusql::_or()
{
    this->query += " or ";
    return this;
}

QList<QVariantMap> Viniciusql::finishSelect()
{
    QSqlQuery q;
    QList<QVariantMap> list;
    QVariantMap results;
    if(q.prepare(this->query)){
        qDebug() << "Select bem escrito";
        q.exec();
        while(q.next()){
            qDebug() << "criando a lista final";
            qDebug() << "selected columns = " << this->selectColumns;
            foreach(QString column, this->selectColumns){
                results.insert(column, q.record().value(q.record().indexOf(column)));
                list.append(results);
                results.clear();
            }
        }
        return list;
    }
    else{
        qDebug() << "select falhou";
    }
    return {};

}
