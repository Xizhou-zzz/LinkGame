//最新更新时间：2022/12/8
//作者：张凯歌
/*作用：
 * 2022/12/5
 * 图形界面；
   背景音乐，消除音效，可控制背景音乐的开关；
   随机生成游戏；
   得分统计：消除成功，奖励分数10；
   限次提示：陷入困境时，按提示按钮可以显示当前可消除的一对图片，每一局有使用次数限制为2
   奖励时间：成功消除6对图片后，对应就会增加一定的奖励时间3s；
   手动重置：将剩下图片重置游戏，重置次数限制为2，如果重置前有解则需要扣减5分；
   游戏控制：开始前选择难度、结束后显示分数并可选择继续新的游戏、游戏可暂停；
   2022/12/8
   分数统计：将剩余时间转换为分数并加到总分数中；
   游戏逻辑完善：暂停游戏后，不可选择重置和提示;

 * */
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置标题
    setWindowTitle("连连看");

    //初始化难度
    getDifficulty();

    //初始化图片和音效
    buttons = nullptr;
    types = nullptr;
    for(int i =0;i<16;i++)
    {
        icons[i].load(QString::asprintf(":/new/prefix1/resources/images/%d.png",i));
    }
    //加载连线
    imageLine[0].load(":/new/prefix1/resources/images/line.png");
    QTransform transform;
    transform.rotate(90.0);
    //连线旋转九十度
    imageLine[1]=imageLine[0].transformed(transform,Qt::FastTransformation);
    //折线
    imageTurn[0].load(":/new/prefix1/resources/images/turn.png");
    for(int i =1;i<4;i++)
    {
        imageTurn[i] = imageTurn[i-1].transformed(transform,Qt::FastTransformation);
    }

    //点击音效

    clickSound.setSource(QUrl::fromLocalFile(":/new/prefix1/resources/sound/click.wav"));
    clickSound.setVolume(0.5f);
    bgm.setSource(QUrl::fromLocalFile(":/new/prefix1/resources/sound/background.wav"));
    bgm.setVolume(0.5f);
    bgm.setLoopCount(QSoundEffect::Infinite);
    bgm.play();



    //设置grid布局
   ui->gridLayout->setHorizontalSpacing(1);
   ui->gridLayout->setVerticalSpacing(1);
   ui->gridLayout->setSizeConstraint(QLayout::SetFixedSize);
   ui->gridLayout->activate();

   //清理场景
   clearStage();

   //设置场景
   setStage();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//难度设置
void MainWindow::getDifficulty()
{
    //难度种类
    QStringList difficulty;
    difficulty<<"简单"<<"普通"<<"困难";
    //难度选择
    QString title = "难度选择";
    QString text = "选择您想挑战的难度：";
    bool sure =  false;

    //默认为普通难度
    QString selection = QInputDialog::getItem(this,title,text,difficulty,1,false,&sure);

    if(!sure)
        exit(0);

    if(selection=="简单")
    {
        setDifficulty(6,6,8);
    }else if(selection =="困难")
    {
        setDifficulty(10,14,15);
    }else
    {
        setDifficulty(8,10,11);
    }
}

//设置参数:高、宽、图片数量
void MainWindow::setDifficulty(int setHb, int setWb, int setFigure)
{
    hb = setHb;
    wb = setWb;
    figure = setFigure;
}

//清除
void MainWindow::clearStage()
{
    lastClickedH = lastClickedW =0;
    if(types == nullptr||buttons == nullptr)
        return ;
    //清除按钮和格子的类型
    for(int i =0;i<= hb+1;i++)
    {
        if(buttons[i]!=nullptr)
        {
            for(int j =0;j<=wb+1;j++)
            {
                if(buttons[i][j]!=nullptr)
                    delete buttons[i][j];
            }
        }
        if(types[i]!=nullptr)
            delete types[i];
    }
    delete types;
    delete buttons;

    //设置游戏未开始
    isStarted = false;
    //游戏未暂停
    stop = false;
    //设置时间进度条和分数显示
    gameTime = 100;
    ui->progressBar->setValue(gameTime);
    score = 0;
    ui->lcdNumber->display(score);

    //时间
    if(pTimer !=nullptr) delete pTimer;
}

//设置游戏
void MainWindow::setStage()
{
    //游戏未暂停
    stop = false;
    //初始化时间
    gameTime = 100;
    ui->progressBar->setValue(gameTime);
    success =0;

    //初始化分数及其显示
    score = 0;
    ui->lcdNumber->display(score);
    //重置剩余块
    remains = hb*wb;

    //初始化重置次数
    reset = 2;
    ui->resetChoice->setText(QString::number(reset));
    ui->resetChoice->setEnabled(false);

    //初始化提示次数
    hint = 2;
    ui->hintChoice->setText(QString::number(hint));
    ui->hintChoice->setEnabled(false);

    //初始化计时器
    pTimer = new QTimer(this);
    //设置间隔为1s
    pTimer->setInterval(1000);
    //连接
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    //开始计时
    pTimer->start();

    //初始化按钮数组和每个按钮的图片类型
    buttons = new QPushButton** [hb+2];
    types = new int*[hb+2];
    for(int i =0;i<=hb+1;i++)
    {
        types[i] = new int[wb+2];
        buttons[i] = new QPushButton*[wb+2];
    }
    //相邻两个设置为相同
    int * typeList = new int[hb*wb];
    for(int i =0;i<hb*wb;i+=2)
    {
        typeList[i] = typeList[i+1] = i/2%figure+1;
    }

    //生成随机地图
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i =0;i<hb*wb;i++)
    {
        //随机两个进行交换
        int temp = rand()%(hb*wb-i)+i;
        int t = typeList[temp];
        typeList[temp] = typeList[i];
        typeList[i] = t;
        //赋值
        types[i/wb+1][i%wb+1] =typeList[i];
    }

    //设置地图
    for(int i =0;i<=hb+1;i++)
    {
        for(int j =0;j<=wb+1;j++)
        {
            buttons[i][j] = new QPushButton;
            //边界设置为空白
            if(i==0||j==0||i==hb+1||j==wb+1)
            {
                types[i][j] = 0;
                buttons[i][j]->setIcon(icons[types[i][j]]);
            }else
            {
                //设置图片
                buttons[i][j]->setIcon(icons[types[i][j]]);
                //连接信号与槽
                connect(buttons[i][j],SIGNAL(clicked()),this,SLOT(btnsClicked()));
            }
            //设置按钮和图片大小
            buttons[i][j]->setMinimumWidth(60);
            buttons[i][j]->setMinimumHeight(60);
            buttons[i][j]->setMaximumWidth(60);
            buttons[i][j]->setMaximumHeight(60);
            buttons[i][j]->setIconSize(QSize(60,60));
            //布局按钮
            ui->gridLayout->addWidget(buttons[i][j],i, j, 1, 1,Qt::AlignCenter | Qt::AlignHCenter);
        }
    }

}

//时间进度条
void MainWindow::onTimeOut()
{
    if(gameTime>0)
    {
       gameTime-- ;
       ui->progressBar->setValue(gameTime);
    }
    if(gameTime<=0)
    {
        gameTime = 0;
        ui->progressBar->setValue(gameTime);
        QString scoreStr = QString::number(score);
        QString tip = "很遗憾您失败了！并获得"+scoreStr+"分\n要再来一局吗？";
        QMessageBox::StandardButton result =
                QMessageBox::question(this, "落败",
                              tip,
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::NoButton);
        if(result==QMessageBox::No)
            exit(0);
        newGame();

    }

}


void MainWindow::btnsClicked()
{
    //获取被点击的按钮的坐标
    QObject *obj = qobject_cast<QPushButton *>(sender());
    int thisH=0, thisW=0;
    //遍历确定具体坐标
    for(int i=1; i<=hb; i++)
        for(int j=1; j<=wb; j++)
            if(buttons[i][j]==obj)
            {
                thisH = i;
                thisW = j;
                break;
            }

    //点击已经消除的无效
    if(types[thisH][thisW]==0)
        return ;

    //如果没有已经点击的按钮，就记录当前按钮为第一个点击的按钮
    if (!lastClickedH && !lastClickedW)
    {
        lastClickedH = thisH;
        lastClickedW = thisW;
        return ;
    }

    //点击已经点过的无效
    if(lastClickedH==thisH && lastClickedW==thisW)
        return ;

    //点击不同类型的无效，并将前一个点击的更新
    if(types[lastClickedH][lastClickedW]!=types[thisH][thisW])
    {
        lastClickedH = thisH;
        lastClickedW = thisW;
        return ;
    }

    //判定这次点击配对的合法性
    LinkPoints lp;
      if(     oneLine(lastClickedH, lastClickedW, thisH, thisW, lp)
            || twoLine(lastClickedH, lastClickedW, thisH, thisW, lp)
            || threeLine(lastClickedH, lastClickedW, thisH, thisW, lp))
    {
        //如果没有开始游戏，就开始游戏。同时开始计时
        if(!isStarted)
        {
            isStarted = true;
        }

        //点击音效
        clickSound.play();

        //分数加10
        score +=10;
        ui->lcdNumber->display(score);

        //时间奖励
        success++;
        //成功六次，奖励三秒
        if(success == 6)
        {
            gameTime +=3;
            if(gameTime>100)
                gameTime =100;
            success =0;
        }

        //画线连接两张图片
        drawLines(lp);

        //起点和终点设置为空图片，并且更新组件
        types[lastClickedH][lastClickedW] = types[thisH][thisW] = 0;

        buttons[thisH][thisW]->setIcon(icons[0]);
        ui->gridLayout->addWidget(buttons[thisH][thisW],
                                  thisH, thisW, 1, 1,
                                  Qt::AlignCenter | Qt::AlignHCenter);

        buttons[lastClickedH][lastClickedW]->setIcon(icons[0]);
        ui->gridLayout->addWidget(buttons[lastClickedH][lastClickedW],
                                  lastClickedH, lastClickedW, 1, 1,
                                  Qt::AlignCenter | Qt::AlignHCenter);

        //重置点击按钮
        lastClickedH = lastClickedW = 0;

        //修改进度
        remains -= 2;

        //游戏结束
        if (remains<=0) {
            //暂停时间
            pTimer->stop();
            //剩余时间算到总分中
            score+=gameTime;
            ui->lcdNumber->display(score);
            //提示信息
            QString scoreStr = QString::number(score);
            QString tip = "恭喜你通过本关！并获得"+scoreStr+"分\n要再来一局吗？";
            QMessageBox::StandardButton result =
                    QMessageBox::question(this, "成功",
                                  tip,
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::NoButton);
            if(result==QMessageBox::No)
                exit(0);
            newGame();
        }
    }
    else {
        //不合法的配对
        lastClickedH = lastClickedW = 0;
    }
}

//两个点是否能通过一条直线相连
bool MainWindow::oneLine(int lstH, int lstW, int thisH, int thisW, LinkPoints &lp)
{
    //间隔的端点数量为0
    lp.between = 0;
    //起点的坐标
    lp.point[0][0] = lstH; lp.point[0][1] = lstW;
    //终点的坐标
    lp.point[1][0] = thisH; lp.point[1][1] = thisW;
    //宽的坐标相同
    if (lstW==thisW)
    {
        //获取两个点中高度小的值
        int minH = lstH<thisH?lstH:thisH;
        //获取两个点中高度大的值
        int maxH = lstH + thisH - minH;
        //从低遍历到高
        for(int i=minH+1; i<=maxH-1; i++)
            //路中有图片相隔，则返回false
            if(types[i][lstW])
                return false;
        //无图片相隔，返回true
        return true;

        //高的坐标相同
    }else if (lstH==thisH)
    {
        //获取两个点中宽坐标小的和大的值
        int minW = lstW<thisW?lstW:thisW;
        int maxW = lstW + thisW - minW;
        //遍历
        for(int i=minW+1; i<=maxW-1; i++)
            //有图片相隔返回false
            if(types[lstH][i])
                return false;
        //否则返回true
        return true;
    }
    //两个点高和宽坐标都不相等，返回false
    return false;
}

//两个点能否通过两条直线相连
bool MainWindow::twoLine(int lstH, int lstW, int thisH, int thisW, LinkPoints &lp)
{
    //中间间隔的端点数为1
    lp.between = 1;
    //起点坐标
    lp.point[0][0] = lstH; lp.point[0][1] = lstW;
    //终点坐标
    lp.point[2][0] = thisH; lp.point[2][1] = thisW;
    //高和宽有坐标相同时，不能用两条直线相连
    if(lstH==thisH || lstW==thisW)
         return false;

    LinkPoints temp;
    //中间的端点无图片
    if(types[lstH][thisW]==0)
    {
        //设置中间端点的坐标
        lp.point[1][0] = lstH; lp.point[1][1] = thisW;
        //分别判断两条直线的可行性
        if(oneLine(lstH, lstW, lstH, thisW, temp)
                && oneLine(lstH, thisW, thisH, thisW, temp))
            return true;
    }
    //中间的端点无图片
    if(types[thisH][lstW]==0)
    {
        //设置中间端点的坐标
        lp.point[1][0] = thisH; lp.point[1][1] = lstW;
        //分别判断两条直线的可行性
        if(oneLine(lstH, lstW, thisH, lstW, temp)
                && oneLine(thisH, lstW, thisH, thisW, temp))
            return true;
    }
    //未找到返回false
    return false;
}

//两个点能否通过三条直线相连
bool MainWindow::threeLine(int lstH, int lstW, int thisH, int thisW, LinkPoints &lp)
{
    //中间端点数为2
    lp.between = 2;
    //起点坐标
    lp.point[0][0] = lstH; lp.point[0][1] = lstW;
    //终点坐标
    lp.point[3][0] = thisH; lp.point[3][1] = thisW;
    LinkPoints temp;
    //从低到高进行遍历
    for(int i=0; i<=hb+1; i++)
    {
        //起点和终点的宽坐标分别对应的相同的高度坐标
        //得到两个端点，分别进行判断
        if(i!=lstH && i!=thisH
                && !types[i][lstW] && !types[i][thisW]
                && oneLine(lstH, lstW, i, lstW, temp)
                && twoLine(i, lstW, thisH, thisW, temp))
        {
            lp.point[1][0] = i; lp.point[1][1] = lstW;
            lp.point[2][0] = i; lp.point[2][1] = thisW;
            return true;
        }

        if(i!=thisH && i!=lstH
                && !types[i][thisW] && !types[i][lstW]
                && twoLine(lstH, lstW, i, thisW, temp)
                && oneLine(i, thisW, thisH, thisW, temp))
        {
            lp.point[1][0] = i; lp.point[1][1] = thisW;
            lp.point[2][0] = i; lp.point[2][1] = lstW;
            return true;
        }
    }
    //从左到右进行遍历
    for(int i=0; i<=wb+1; i++)
    {
        //起点和终点的高度坐标分别对应的相同的宽坐标
        //得到两个端点，分别进行判断
        if(i!=lstW && i!=thisW
                && !types[lstH][i] && !types[thisH][i]
                && oneLine(lstH, lstW, lstH, i, temp)
                && twoLine(lstH, i, thisH, thisW, temp))
        {
            lp.point[1][0] = lstH; lp.point[1][1] = i;
            lp.point[2][0] = thisH; lp.point[2][1] = i;
            return true;
        }
        if(i!=thisW && i!=lstW
                && !types[thisH][i] && !types[lstH][i]
                && twoLine(lstH, lstW, thisH, i, temp)
                && oneLine(thisH, i, thisH, thisW, temp))
        {
            lp.point[1][0] = thisH; lp.point[1][1] = i;
            lp.point[2][0] = lstH; lp.point[2][1] = i;
            return true;
        }
    }
    //未找到则返回false
    return false;
}

void sleep(unsigned int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);

    while(QTime::currentTime() < reachTime)
      QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}

//画线
void MainWindow::drawLines(LinkPoints lp)
{


    buttons[lp.point[0][0]][lp.point[0][1]]->setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(0,255,0);");
    buttons[lp.point[lp.between+1][0]][lp.point[lp.between+1][1]]->setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(0,255,0);");


    //画线连接
    for(int i=0; i<=lp.between; i++)
        //每两个点之间画一条直线
        drawALine(lp.point[i][0], lp.point[i][1],
                lp.point[i+1][0], lp.point[i+1][1], false);

    //中间端点设置拐线图片
    for(int i=1; i<=lp.between; i++)
    {
        //获取拐线方向，并设置图片
        buttons[lp.point[i][0]][lp.point[i][1]]->setIcon(imageTurn[
                getDirection(lp.point[i-1][0],
                lp.point[i-1][1],
                lp.point[i+1][0], lp.point[i+1][1],
                lp.point[i][0], lp.point[i][1])]);
        //更新组件
        ui->gridLayout->addWidget(buttons[lp.point[i][0]][lp.point[i][1]],
                lp.point[i][0], lp.point[i][1],
                1, 1,
                Qt::AlignCenter | Qt::AlignHCenter);
    }

    //等待0.2s
    sleep(200);


   buttons[lp.point[0][0]][lp.point[0][1]]->setStyleSheet("");
   buttons[lp.point[lp.between+1][0]][lp.point[lp.between+1][1]]->setStyleSheet("");

    //消除连线，变为空图片
    for(int i=0; i<=lp.between; i++)
        drawALine(lp.point[i][0], lp.point[i][1],
                lp.point[i+1][0], lp.point[i+1][1], true);
    //消除拐点连线
    for(int i=1; i<=lp.between; i++)
    {
        //变为空图片
        buttons[lp.point[i][0]][lp.point[i][1]]->setIcon(icons[0]);
        //更显组件
        ui->gridLayout->addWidget(buttons[lp.point[i][0]][lp.point[i][1]],
                lp.point[i][0], lp.point[i][1],
                1, 1,
                Qt::AlignCenter | Qt::AlignHCenter);
    }
}

//画一条直线
void MainWindow::drawALine(int lstH, int lstW, int thisH, int thisW, bool isW)
{
    //宽坐标相同
    if(lstW==thisW)
    {
        int minH=lstH<thisH?lstH:thisH;
        int maxH=lstH+thisH-minH;
        //从低到高遍历
        for(int i=minH+1; i<=maxH-1; i++)
        {
            //isW为false，显示连线图片，否则显示空图片
            buttons[i][lstW]->setIcon(isW?icons[0]:imageLine[0]);
            //组件更新按钮
            ui->gridLayout->addWidget(buttons[i][lstW], i, lstW,
                                      1, 1,
                                      Qt::AlignCenter | Qt::AlignHCenter);
        }
    }
    else
    {
        int minW=lstW<thisW?lstW:thisW;
        int maxW=lstW+thisW-minW;
        //从左到右遍历
        for(int i=minW+1; i<=maxW-1; i++)
        {
            //isW为false，显示连线图片，否则显示空图片
            buttons[lstH][i]->setIcon(isW?icons[0]:imageLine[1]);
            //组件更新按钮
            ui->gridLayout->addWidget(buttons[lstH][i], lstH, i,
                                      1, 1,
                                      Qt::AlignCenter | Qt::AlignHCenter);
        }
    }
}


int MainWindow::getDirection(int lstH, int lstW, int thisH, int thisW, int midH, int midW)
{

        Q_UNUSED(midW)
       //起点的高度更高
        if(lstH>thisH)
        {
            //起点和终点高度坐标互换
            int t=lstH; lstH = thisH;   thisH = t;
            //起点和终点宽度坐标互换
            t = lstW;   lstW = thisW;   thisW = t;
        }
        if(lstW<thisW)
        {
            if(midH==lstH)  return 2;
            else    return 0;
        }
        else
        {
            if(midH==lstH)  return 1;
            else    return 3;
        }

}

void MainWindow::newGame()
{
    clearStage();
    getDifficulty();
    setStage();
}

void MainWindow::on_closeBGM_clicked()
{
    bgm.stop();
}


void MainWindow::on_openBGM_clicked()
{
    bgm.play();
}


void MainWindow::on_stopGame_clicked()
{
    //暂停游戏，图片全部设置为空白
    for(int i =0;i<=hb+1;i++)
    {
        for(int j =0;j<=wb+1;j++)
        {
            buttons[i][j] ->setIcon(icons[0]);
            //重新布局
            ui->gridLayout->addWidget(buttons[i][j],i, j, 1, 1,Qt::AlignCenter | Qt::AlignHCenter);
        }
    }
    //计时停止
    pTimer->stop();
    //设置标记
    stop = true;
}


void MainWindow::on_continueGame_clicked()
{
    //继续游戏，图片设置恢复
    for(int i =0;i<=hb+1;i++)
    {
        for(int j =0;j<=wb+1;j++)
        {
            buttons[i][j] ->setIcon(icons[types[i][j]]);
            //重新布局
            ui->gridLayout->addWidget(buttons[i][j],i, j, 1, 1,Qt::AlignCenter | Qt::AlignHCenter);
        }
    }
    //计时继续
    pTimer->start();
    //设置标记
    stop = false;

}


void MainWindow::on_reset_clicked()
{
    if(stop)
    {
        //发出提示信息
        QMessageBox::critical(this, tr("Tips"), tr("当前游戏已暂停！"));
    }else
    {
        //可重置次数大于0
        if(reset >0)
        {
            //判断当前是否有解
            LinkPoints tempPoint;
            //有解则扣分
            if(judge(tempPoint,true))
            {
                score-=5;
                ui->lcdNumber->display(score);
                //画线
                drawLines(tempPoint);

            }

            //获得并且存储每个格子的图片类型
           std::vector<int> temp;
           for(int i =0;i<=hb+1;i++)
           {
               for(int j =0;j<=wb+1;j++)
               {
                   //空白图片不变
                   if(types[i][j]!=0)
                   {
                       temp.push_back(types[i][j]);
                   }

               }
           }
           //图片类型随机排序
            random_shuffle (temp.begin(), temp.end());

            //重新设置每个格子的图片类型
            for(int i=0;i<=hb+1;i++)
            {
                for(int j=0;j<=wb+1;j++)
                {
                    if(types[i][j]!=0)
                    {
                        types[i][j] = temp[temp.size()-1];
                        temp.pop_back();
                        buttons[i][j] ->setIcon(icons[types[i][j]]);
                        //重新布局
                        ui->gridLayout->addWidget(buttons[i][j],i, j, 1, 1,Qt::AlignCenter | Qt::AlignHCenter);
                    }
                }
            }
            reset--;
            ui->resetChoice->setText(QString::number(reset));
        }else
            //无重置次数，发出提示信息
        {
            QMessageBox::critical(this, tr("Tips"), tr("重置次数已用尽！"));

        }
    }


}

//判断当前是否有解
bool MainWindow::judge(LinkPoints& temp,bool flag)
{
    if(stop)
    {
        //发出提示信息
        QMessageBox::critical(this, tr("Tips"), tr("当前游戏已暂停！"));
        //返回false
        return false;

    }else
    {
        //提示次数大于0
        //flag为重置时进行判断有解
        if(hint>0 || flag)
        {
            if(!flag)
            {
                hint--;
                ui->hintChoice->setText(QString::number(hint));
            }

            //遍历
             for(int i=0;i<=hb+1;i++)
             {
                 for(int j=0;j<=wb+1;j++)
                 {
                     //不为空图片
                     if(types[i][j]!=0)
                     {
                         //重新遍历
                         for(int x =0;x<=hb+1 ;x++)
                         {
                             for(int y=0;y<=wb+1;y++)
                             {
                                 //跳过同个图片
                                 if(x==i&&y==j)
                                     continue;
                                 //找到相同图片
                                 if(types[i][j] == types[x][y])
                                 {
                                     //判断是否有解
                                     if  (   oneLine(i, j, x, y, temp)
                                           || twoLine(i, j, x, y, temp)
                                           || threeLine(i, j, x, y, temp))
                                     {
                                         //有解返回true
                                         return true;
                                     }

                                 }
                             }
                         }
                     }
                 }
             }
             //无解返回false
             return false;
        }else
            //无提示次数
        {
            //发出提示信息
            QMessageBox::critical(this, tr("Tips"), tr("提示次数已用尽！"));
            return false;
        }

    }



}


void MainWindow::on_hint_clicked()
{
    LinkPoints temp;
    //有解
    if(judge(temp,false))
    {
        //画线
        drawLines(temp);
    }

}

