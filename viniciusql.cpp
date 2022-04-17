#include "viniciusql.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>

QStringList Viniciusql::getColumns()
{
    return this->selectColumns;
}

void Viniciusql::setSql(QString sql)
{
    this->query += sql;
}

Viniciusql::Viniciusql(QString table)
{
    this->table = table;
    setColumns();
}

void Viniciusql::setSql(QString sql)
{
    this->query += sql;
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
    this->values = maps.values();
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

Viniciusql *Viniciusql::update(QVariantMap maps)
{
    QStringList columns = maps.keys();
    this->values = maps.values();
    int cont = 0;

    this->query += "update " + this->table + "set ";
    while(cont < columns.size()){
        this->query += columns[cont] + " = ? ";
        if(cont != columns.size() - 1)
            this->query += ", ";
        cont++;
    }
    return this;
}

Viniciusql *Viniciusql::destroy()
{
    this->query += "delete from " + this->table;
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

bool Viniciusql::finish()
{
    QSqlQuery query(this->query);
    bool isInsert = this->query.contains("insert");
    if(this->values.size() > 0){
        foreach(QVariant value, values){
            query.addBindValue(value);
        }
    }
    bool result = query.exec();
    this->query.clear();
    this->values.clear();
    return isInsert? query.lastInsertId().toInt() : result;
}

