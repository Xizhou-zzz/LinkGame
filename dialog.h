#ifndef DIALOG_H
#define DIALOG_H
//查询界面
#include <QDialog>
#include <iostream>
#include <vector>
#include "data.h"
#include <QChart>
using namespace std;
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QString usestr;
    QString scorestr1;
    QString scorestr2;
    int score1;
    int score2;
    vector<int> v;
    vector<vector<int>> vec;
    Data data;
};

#endif // DIALOG_H
