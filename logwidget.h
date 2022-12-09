#ifndef LOGWIDGET_H
#define LOGWIDGET_H
//登录界面
//申请账号，直接填入，在按申请即可，用户名不能重复
#include "data.h"
#include <QWidget>
#include <QString>
#include <QMessageBox>
namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);
    QString str1;//用户名
    Data *data = new Data();
    ~LogWidget();

signals:
    void login(); //登录主界面信号
    void close_window(); //关闭登录界面信号

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::LogWidget *ui;
    QString str2;
};

#endif // LOGWIDGET_H
