#include "greenteach.h"
#include "ui_greenteach.h"

greenTeach::greenTeach(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::greenTeach)
{
    ui->setupUi(this);
    picId = 1;
}

greenTeach::~greenTeach()
{
    delete ui;
}

void greenTeach::on_startButton_clicked()
{
    //开启计时器
    myTimerId = this->startTimer(TIMETONEXT);

}

void greenTeach::timerEvent(QTimerEvent *event){
    if(event->timerId() != myTimerId)
        return;

    QString path(":/resources/teachresources/");
    path += QString::number(picId);
    path += ".jpg";
    QImage image;
    image.load(path);
    QImage resultImg=image.scaled(ui->label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->label->setPixmap(QPixmap::fromImage(resultImg));
    picId++;
    if(picId > 4) picId = 1;

}

void greenTeach::on_closeButton_clicked()
{
    close();
}

