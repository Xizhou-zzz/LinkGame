#ifndef DATA_H
#define DATA_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <iostream>
#include <vector>
using namespace std;
class Data
{
public:
    Data();
    // 打开数据库
    void openDb();
    void closeDb();
    void creatUserTable();
    void creatUsersTable();//建立用户得分数据库
    void insertUser(QString str1,QString str2);//插入用户账号和密码
    void insertPoint(QString str,QString str2);//插入对应用户名的得分
    // 查询全部数据
    bool queryUsreTable(QString str1,QString str2);
    //查找有没有相等用户名
    bool search(QString str1);//有相等的返回ture
    //查询用户名为str的用户得分情况
    vector<int> searchScore(QString str,vector<int> v,int score1,int score2);
    //查询所有用户scor1-scor2的分数
    vector<int> searchAll(vector<int> v,int score1,int score2);
private:
     QSqlDatabase database;// 用于建立和数据库的连接
};
#endif // DATA_H
