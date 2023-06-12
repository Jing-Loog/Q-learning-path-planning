#include "mainwindow.h"
#include "q_learning.h"
#include "ui_mainwindow.h"
#include "messagecontrol.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QRegExp>
#include<string.h>
#include<QFile>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
       ui->setupUi(this);
       this->setWindowTitle("基于强化学习算法的机器人路径规划 ysy");
      //初始化栅格
       ui->tableWidget->setColumnCount(TABLE_COLUMN+1);                        //设置栅格列数
       ui->tableWidget->setRowCount(TABLE_ROW+1);                            //设置栅格行数
       ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置不能修改栅格内容
       ui->tableWidget->verticalHeader()->setVisible(false);           //隐藏列表头
       ui->tableWidget->horizontalHeader()->setVisible(false);         //隐藏行表头
       ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去掉水平滚动条
       ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //去掉垂直滚动条


       for(int i=0;i<=TABLE_ROW;i++)
          {
              ui->tableWidget->setRowHeight(i,HEIGHT);         //设置栅格高度
              for(int j=0;j<=TABLE_COLUMN;j++)
              {
                  Q_learning * t_q_learning = new Q_learning();
                  MessageControl*  t_MessageControl =new MessageControl(this,t_q_learning,i,j);   //给每一个栅格设置一个messagecontrol对象
                  ui->tableWidget->setColumnWidth(j,WIDTH);           //设置栅格宽度
                  ui->tableWidget->setCellWidget(i,j,t_MessageControl);
                  connect(t_MessageControl,&MessageControl::selectedCellIndex,this,&MainWindow::selectedCellIndex);     //把每个格子与选格函数相连
                  connect(t_MessageControl,&MessageControl::selectedCancelIndex,this,&MainWindow::selectedCancelIndex);   //把每个格子与选格函数相连
              }
          }

        ui->tableWidget->setFocusProxy(this);   //设置焦点获取

       //UI设置
        ui->label->setToolTip("本程序基于强化学习算法");
        ui->pushButton_find->setToolTip("开始");
        ui->pushButton_barrier->setToolTip("设置障碍物，灰色，可多选");
        ui->pushButton_clear->setToolTip("清空二维地图");


        //设置起点、终点颜色
       MessageControl* t_MessageControl =(MessageControl *)(ui->tableWidget->cellWidget(start_y,start_x));
       t_MessageControl->setCellType(MessageControl::Start);

        MessageControl* t_MessageControl1 =(MessageControl *)(ui->tableWidget->cellWidget(end_y,end_x));
       t_MessageControl1->setCellType(MessageControl::End);

}

MainWindow::~MainWindow()
{
    delete ui;
}


    Q_learning Q;   //创建一个学习算法对象


//选格函数，鼠标点击格子就选中
void MainWindow::selectedCellIndex(int x, int y)
{


    QPoint t =QPoint(x,y);
    if(!selected_xy.contains(t))
    {
        selected_xy.append(t);
    }

    if(Q.barrier_xy.contains(t))    //再次点击取消原来保存的障碍物记录
    {
        Q.barrier_xy.removeOne(t);
    }
}


//再次点击已选中的格子就能取消
void  MainWindow::selectedCancelIndex(int x,int y)
{
    QPoint t = QPoint(x,y);
    if(Q.barrier_xy.contains(t))      //再次点击取消原来保存的障碍物记录
    {
        Q.barrier_xy.removeOne(t);
    }


    if(selected_xy.contains(t))
    {
        selected_xy.removeOne(t);
    }
}

//清空Qtable
void MainWindow::on_pushButton_cliQ_clicked()
{
    for(int i=0;i<=TABLE_ROW;i++)
    {
        for(int j=0;j<=TABLE_COLUMN;j++)
        {
            for(int k=1;k<=8;k++)
            Q.Qtable[j][i][k]=0.0;
        }
    }

}


//设置障碍按钮槽函数
void MainWindow::on_pushButton_barrier_clicked()
{

    if(selected_xy.size() == 0)
        {
            QMessageBox::critical(this,"错误","请先选择一个障碍物！");    //消息框弹窗，带警告
        }
        else if(selected_xy.size() >= 1)
        {
            for(auto t:selected_xy)
            {
                if(!Q.barrier_xy.contains(t))       //如果该栅格坐标不在障碍物坐标对内，追加
                {
                    Q.barrier_xy.append(t);

                }
            }
            for(auto t:Q.barrier_xy)
            {
                MessageControl* t_MessageControl =(MessageControl *)(ui->tableWidget->cellWidget(t.y(),t.x()));
                t_MessageControl->setCellType(MessageControl::Barrier);
            }
            selected_xy.clear();
        }
}


//清空障碍物和轨迹的函数
void MainWindow::on_pushButton_clear_clicked()
{
    selected_xy.clear();
    Q.barrier_xy.clear();

    for(int i=0;i<=TABLE_ROW;i++)
    {
        for(int j=0;j<=TABLE_COLUMN;j++)
        {
            MessageControl*  t_MessageControl =( MessageControl*) ui->tableWidget->cellWidget(i,j);
            t_MessageControl->setCellType(MessageControl::Null);

            //不把起点、终点清空
            if(i==0 && j==0)
                t_MessageControl->setCellType(MessageControl::Start);
            if(i==TABLE_ROW && j==TABLE_COLUMN)
                 t_MessageControl->setCellType(MessageControl::End);

        }
    }
}


//寻路按钮槽函数
void MainWindow::on_pushButton_find_clicked()
{

    if(ui->pushButton_find->text() == "开始寻路 (Enter)")
    {


        ui->pushButton_barrier->setEnabled(false);
        ui->pushButton_clear->setEnabled(false);
        ui->lineEdit->setEnabled(false);
        ui->pushButton_find->setText("停止寻路 (Enter)");      //按钮文字变为"停止寻路 (Enter)"

        //清空path
        for(int i=0;i<=TABLE_ROW;i++)
        {
            for(int j=0;j<=TABLE_COLUMN;j++)
            {
                MessageControl* t_MessageControl =(MessageControl*) ui->tableWidget->cellWidget(i,j);
                if((t_MessageControl->type == MessageControl::Path))
                {
                    t_MessageControl->setCellType(MessageControl::Null);
                }
            }
        }

        //运行算法
        Reinforce();

    }
    else if(ui->pushButton_find->text() == "停止寻路 (Enter)")
    {
        //控件还原
        ui->tableWidget->setEnabled(true);
        ui->pushButton_barrier->setEnabled(true);
        ui->pushButton_clear->setEnabled(true);
        ui->lineEdit->setEnabled(true);
        ui->pushButton_find->setText("开始寻路 (Enter)");     //按钮文字变为"开始寻路 (Enter)"

    }
}



//清空本地记录的函数
void MainWindow::on_pushButton_clicked()
{
    QFile file("D:/Qt/project/robotpath/algorithmdata.txt");
       //对文件进行写操作
         file.open(QIODevice::WriteOnly|QIODevice::Truncate);
         file.close();
         QFile file1("D:/Qt/project/robotpath/windata.txt");
         //对文件进行写操作
         file1.open(QIODevice::WriteOnly|QIODevice::Truncate);
         file1.close();
}



//强化学习寻径算法
void MainWindow::Reinforce()
{

     Q.actpunish();         //初始化边界惩罚值
     int wintimes=0;            //定义成功次数变量
    int traintimes = QInputDialog::getInt(this,
                                             "QInputdialog_Name",
                                             "请输入要训练的次数",
                                             QLineEdit::Normal,
                                             1,
                                             5000);

    for(int i=1;i<=traintimes;i++)     //训练循环，traintimes为次数
 {
         Q.over=0;     //成功与否判定变量
         Q.length=0;
         Q.initplace(start_x,start_y,Q.over);                        //初始化，保险
         Q.arrived_xy.clear();                         //清空暂存所在格子坐标的数组
         Q.arrived_xy.append(QPoint(start_x,start_y));               //把起始点加入暂存所在格子坐标的数组
         int op=0;                                     //动作的代号

       //每次训练清空前一次的轨迹
         for(int i=0;i<=TABLE_ROW;i++)
         {
             for(int j=0;j<=TABLE_COLUMN;j++)
             {
                 MessageControl*  t_MessageControl =( MessageControl*) ui->tableWidget->cellWidget(i,j);
                 t_MessageControl->setCellType(MessageControl::Null);
                 if(i==0 && j==0)
                     t_MessageControl->setCellType(MessageControl::Start);
                 if(i==TABLE_ROW && j==TABLE_COLUMN)
                      t_MessageControl->setCellType(MessageControl::End);

             }
         }


         //障碍物标记保留
         for(auto t:Q.barrier_xy)
         {
             MessageControl* t_MessageControl =(MessageControl *)(ui->tableWidget->cellWidget(t.y(),t.x()));
             t_MessageControl->setCellType(MessageControl::Barrier);
         }


         //当训练次数为10的倍数时，打印在框里
         if(i % 10 == 0)
         {
             char buffer[256];
             sprintf(buffer,"训练次数为: %d\n",i);
             ui->lineEdit->setText(buffer);
         }

         //设置一个messagecontrol对象在目前所在格子
         MessageControl* t_MessageControl =(MessageControl*) ui->tableWidget->cellWidget(Q.arrived_xy.at(0).y(),Q.arrived_xy.at(0).x());

         for(int j = 0; j <= 200;j++)   //一次训练最大不超过100步
         {

            //走到10倍数次步数时打印步数
            if(j % 10 == 0)
             {
                char buffer1[256];
                sprintf(buffer1,"步数为: %d\n",j);
                ui->lineEdit_2->setText(buffer1);
             }

            int regx=Q.arrived_xy.at(0).x(); int regy=Q.arrived_xy.at(0).y();   //提取此时坐标，后面传参要用
            int regtwicex=regx;     int regtwicey=regy;                      //再次暂存此时坐标，后面更新Q表要用
            double nextmax = -1000000;

            if(Q.flag==1)
            {
                 MessageControl* t_MessageControl =(MessageControl*) ui->tableWidget->cellWidget(Q.arrived_xy.at(0).y(),Q.arrived_xy.at(0).x());
                //如果位置改变显示轨迹
                 t_MessageControl->setCellType(MessageControl::Path);

            }
              //有概率随机产生一个动作
            if(rand() % 101 > Greedy)
            {
                op = rand() % 8 + 1;
            }

            //如果不随机，寻找Q值最大的动作走
            else
            {
            for (int m = 1; m <= 8; m++)
            {
                nextmax = max(nextmax + 0.0,  Q.Qtable[regx][regy][m]);  //遍历各个动作的Qtable值，寻找最大的那个值
            }
                for (int m = 1; m <= 8; m++)
            {

                   if (nextmax ==  Q.Qtable[regx][regy][m])    //寻找Q值最大的那个动作
                        op = m;
            }
            }
             double reward = Q.action(op, regx ,regy, Q.over);  //做出动作，并存下获得的奖励值

               Q.Qtable[regtwicex][regtwicey][op] += reward;

             if (Q.over == 1)
                 {
                    wintimes++;    //如果成功，加1
                    walktimes=j;   //记录步数
                    MessageControl* t_MessageControl =(MessageControl*) ui->tableWidget->cellWidget(Q.arrived_xy.at(0).y(),Q.arrived_xy.at(0).x());
                    t_MessageControl->setCellType(MessageControl::Win);
                  //   QMessageBox MyBox(QMessageBox::NoIcon,"提示","正确,寻径成功");
                  //     MyBox.exec();
                      break;
                  }


           }
              char bufferwintimes[256];
              sprintf(bufferwintimes,"成功次数为: %d\n",wintimes);  //展示成功次数
               ui->lineEdit_wintimes->setText(bufferwintimes);

         if(Q.over != 1)
         {
              //     QMessageBox::critical(this,"错误","找不到路径");
                   //控件还原
                       ui->tableWidget->setEnabled(true);
                       ui->pushButton_barrier->setEnabled(true);
                       ui->pushButton_clear->setEnabled(true);
                       ui->lineEdit->setEnabled(true);
                       ui->pushButton_find->setText("开始寻路 (Enter)");
         }


         //打印Qtable
         //创建文件对象，指定文件位置

         QFile file("D:/Qt/project/robotpath/algorithmdata.txt");
          //对文件进行写操作
             if(!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
             {
                   QMessageBox::information(this,"警告","请选择正确的文件！");
             }
              else
             {
                    QTextStream stream( &file );
                    stream<<QString::number(i)<<endl;
                    if(Q.over==1)
                    {
                        stream<<"Win"<<endl;
                        stream<<QString::number(Q.length)<<endl<<endl;
                        QFile winfile("D:/Qt/project/robotpath/windata.txt");
                        winfile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
                        QTextStream stream1( &winfile );
                        stream1<<QString::number(i)<<"    "<<QString::number(walktimes)<<"     "<<Q.length<<endl<<endl;
                    }
                        else
                        stream<<"fail"<<endl<<endl;
                 for(int j=0;j<=TABLE_ROW;j++)
                 {
                     for(int i=0;i<=TABLE_COLUMN;i++)
                     {
                        stream<<QString::number(i)<<" "<<QString::number(j)<<":"<<"  ";    //打印格子坐标
                        for(int k=1;k<=8;k++)
                       {
                            stream<<QString::number(Q.Qtable[i][j][k])<<"  ";           //打印Qtable值
                       }
                        stream<<endl;

                     }
                 }
                 stream<<endl;
              }
     }



}




