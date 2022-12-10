#include "dialog.h"
#include "ui_dialog.h"
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    usestr.clear();
    scorestr1.clear();
    scorestr2.clear();
    v.clear();
    vec.clear();
    ui->textEdit->clear();
    usestr = ui->lineEdit->text();
    scorestr1 = ui->lineEdit_2->text();
    scorestr2 = ui->lineEdit_3->text();

    score1 = scorestr1.toInt();
    score2 = scorestr2.toInt();
    v = data.searchScore(usestr,v,score1,score2);
    int mark = 0;//标志位
    for(int i = 0; i < v.size();i++){
        for(int j = 0;j < vec.size();j++){
            if(v[i] == vec[j][0]) {
                vec[j].push_back(v[i]);
                mark = 1;
                break;
            }
        }
        if(mark == 1) mark = 0;
        if(mark == 0) {
            vector<int> a;
            a.push_back(v[i]);
            vec.push_back(a);
        }
    }

    ui->textEdit->append("总共游戏了" + QString::number(v.size()) +"次");
    ui->textEdit->append("总共有" + QString::number(vec.size()) +"种得分");
    for(int i =0 ;i < vec.size();i++){
        ui->textEdit->append( QString::number(vec[i][0]) + "分"+QString::number(vec[i].size()) + "次");
    }
}

