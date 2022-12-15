//最新更新时间：2022/12/8
//作者：张凯歌，田佳文，王彦博，杨文娟
/*作用：
 * 图形界面；
   背景音乐，消除音效，可控制背景音乐的开关；
   随机生成游戏；
   得分统计：消除成功，奖励分数10；
   限次提示：陷入困境时，按提示按钮可以显示当前可消除的一对图片，每一局有使用次数限制为2
   奖励时间：成功消除6对图片后，对应就会增加一定的奖励时间3s；
   手动重置：将剩下图片重置游戏，重置次数限制为2，如果重置前有解则需要扣减5分；
   游戏控制：开始前选择难度、结束后显示分数并可选择继续新的游戏、游戏可暂停；
 * */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QSoundEffect>
#include <QTransform>
#include <vector>
#include <iostream>
#include "logwidget.h"
#include "dialog.h"
#include "clientwidget.h"
#include "greenteach.h"
#include <QtConcurrent>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getDifficulty();

    void sendSig()
    {
        emit remainsChanged();     //通过emit关键字发射信号
    }

signals:                    //使用signals声明信号函数，访问级别为protected
    void remainsChanged();     //信号只能声明不能定义


private:
    //分数
    int score;
    //时间
    int gameTime;
    //时间奖励,成功6次，奖励3秒
    int success;
    //重置机会
    int reset ;
    //提示机会
    int hint;

    bool stop;

    bool isLogin = false;

    bool online = false;

    QString difficulty;

    Ui::MainWindow *ui;
    //高多少个格子，宽多少个格子
    int hb, wb;
    //多少种图片
    int figure;
    //按钮数组
    QPushButton ***buttons;
    //格子的类型的数组
    int **types;
    //图片数组
    QPixmap icons[16];
    //上一个点击的坐标
    int lastClickedH, lastClickedW;
    //剩余多少个格子
    int remains = 1;

    //连线的图片的数组
    QPixmap imageLine[2], imageTurn[4];
    //计时器
    QTimer *pTimer;

    //点击音效
    QSoundEffect clickSound;
    QSoundEffect bgm;

   //游戏开始标记
    bool isStarted = false;



    //点击两个配对点后，配对点和中间端点构成的结构体
    struct LinkPoints
    {
        //起点和终点间间隔中的端点数
        int between;
        //0是第一个端点，between+1是第二个端点，其他是中间点
        int point[4][2];
    };

    //设置游戏高、宽、图片类型数量
    void setDifficulty(int,int,int);

    //设置游戏场景
    void setStage();

    //判定能否不超过三条线连接
    bool oneLine(int,int,int,int,LinkPoints&);
    bool twoLine(int,int,int,int,LinkPoints&);
    bool threeLine(int,int,int,int,LinkPoints&);

    //清场
    void clearStage();

    //获得一个点到另一个点的经过的中间点的方向
    int getDirection(int,int,int,int,int,int);

    //画一条线和一组线
    void drawALine(int,int,int,int, bool);
    void drawLines(LinkPoints );

    //判断是否有解
    bool judge(LinkPoints&,bool);

    // 登录界面类的对象作为指针
    LogWidget * m_log;

    // 指导界面类的对象作为指针
    greenTeach * t_log;

    //打开查询界面
    Dialog *dia = new Dialog();

    clientWidget *client;

    void sendRemains();

    Data *data = new Data();
private slots:
    //游戏初始化
    void init();
    //游戏判定
    void btnsClicked();
    //时间判定
    void onTimeOut();
    //新的游戏
    void newGame();
    void on_closeBGM_clicked();
    void on_openBGM_clicked();
    void on_stopGame_clicked();
    void on_continueGame_clicked();
    void on_reset_clicked();
    void on_hint_clicked();
    void on_pushButton_clicked();
    void on_action_triggered();
    void on_actionlog_triggered();
    void on_action_2_triggered();
    //状态改变
    void change();
    void on_action_3_triggered();
    void on_action_4_triggered();    
    void on_autoSolve_clicked();
    void on_pushButton_2_clicked();
    //新手指导
    void on_pushButton_3_clicked();
    //数据分析
    void on_pushButton_4_clicked();
};
#endif // MAINWINDOW_H
