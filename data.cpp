#include "data.h"
#include <QString>
Data::Data()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
            {
                database = QSqlDatabase::database("qt_sql_default_connection");
            }else{
                // 建立和SQlite数据库的连接
                database = QSqlDatabase::addDatabase("QSQLITE");
                // 设置数据库文件的名字
                database.setDatabaseName("MyData.db");
          }
}

void Data::openDb(){
    if (!database.open())
        {
            qDebug() << "Error: Failed to connect database." << database.lastError();
        }
}

void Data::closeDb(){
    database.close();
}

void Data::creatUserTable(){
        // 用于执行sql语句的对象
         QSqlQuery sqlQuery;
         // 构建创建数据库的sql语句字符串
         QString createSql = QString("CREATE TABLE user (") +
                                  "userName TEXT NOT NULL," +
                                  "password TEXT NOT NULL)";
         // 执行sql语句
         if(!sqlQuery.exec(createSql)){
            qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
         }else{
            qDebug() << "Table created!";
         }
}

void Data::creatUsersTable(){
    // 用于执行sql语句的对象
     QSqlQuery sqlQuery;
     // 构建创建数据库的sql语句字符串
     QString createSql = QString("CREATE TABLE score (") +
                              "userName TEXT NOT NULL," +
                              "point INT NOT NULL)";
     // 执行sql语句
     if(!sqlQuery.exec(createSql)){
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
     }else{
        qDebug() << "Table created!";
     }
}

void Data::insertUser(QString str1,QString str2){
    QString str=QString("insert into user values('%1','%2')").arg(str1,str2);
    database.exec(str);
}

void Data::insertPoint(QString str,QString str2){
    QString s=QString("insert into score values('%1','%2')").arg(str,str2);
     database.exec(s);
}

bool Data::queryUsreTable(QString str1,QString str2){
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM user");
    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        int tmp1 = str1.toInt();
        int tmp2= str2.toInt();
        while(sqlQuery.next())
        {
            int a = sqlQuery.value(0).toInt();
            int b= sqlQuery.value(1).toInt();
            if(a == tmp1){
                if(b == tmp2)return 1;
            }
        }
    }
    return 0;
}

bool Data::search(QString str1){
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM user");
    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        int tmp1 = str1.toInt();
        while(sqlQuery.next())
        {
            int a = sqlQuery.value(0).toInt();
            if(a == tmp1){
                return true;
            }
        }
    }
    return 0;
}

vector<int> Data::searchScore(QString str,vector<int> v,int score1,int score2){
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM score");
    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while(sqlQuery.next())
        {
            int t = sqlQuery.value(0).toInt();
            if(t == str.toInt()){
                int tmp = sqlQuery.value(1).toInt();
                if(tmp >= score1 && tmp <= score2)
                    v.push_back(tmp);
            }
        }
    }
    return v;
}


vector<int> Data::searchAll(vector<int> v,int score1,int score2){
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM score");
    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to  " << sqlQuery.lastError();
    }
    else{
        while(sqlQuery.next())
        {
            int t = sqlQuery.value(0).toInt();
            int tmp = sqlQuery.value(1).toInt();
            if(tmp >= score1 && tmp <= score2)
                v.push_back(tmp);
        }
    }
    return v;
}
