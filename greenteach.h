#ifndef GREENTEACH_H
#define GREENTEACH_H

#include <QWidget>

#define TIMETONEXT 3*1000
namespace Ui {
class greenTeach;
}

class greenTeach : public QWidget
{
    Q_OBJECT

public:
    explicit greenTeach(QWidget *parent = nullptr);
    virtual void timerEvent(QTimerEvent *event);
    ~greenTeach();

private slots:
    void on_startButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::greenTeach *ui;
    int myTimerId;
    int picId;
};

#endif // GREENTEACH_H
