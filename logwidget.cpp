#include "logwidget.h"
#include "ui_logwidget.h"

LogWidget::LogWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWidget)
{
    ui->setupUi(this);
    // 触发登录按钮的信号槽连接
    (ui->pushButton_2,SIGNAL(clicked()),this,SLOT(on_pushButton_2_clicked()));
    // 发出信号后关闭登录窗口的信号槽连接
    connect(this,SIGNAL(close_window()),this,SLOT(close()));
    data->openDb();
    data->creatUserTable();
    data->creatUsersTable();
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::on_pushButton_clicked()
{
    //注册
    str1.clear();
    str2.clear();
    str1 = ui->lineEdit->text();
    str2 = ui->lineEdit_2->text();
    bool t = data->search(str1);
    if(t){
        QMessageBox::about(NULL, "erro", "用户名已存在");
    }else{
        data->insertUser(str1,str2);
        QMessageBox::about(NULL, "yes", "注册成功");
    }
}


void LogWidget::on_pushButton_2_clicked()
{
    //登录
    str1.clear();
    str2.clear();
    str1 = ui->lineEdit->text();
    str2 = ui->lineEdit_2->text();
    bool t = data->queryUsreTable(str1,str2);
    if(t){
        // 发出登录信号
        emit(login());
        // 发出关闭窗口信号
        emit(close_window());
    }else{
        QMessageBox::about(NULL, "erro", "用户名或密码不正确");
    }
}

