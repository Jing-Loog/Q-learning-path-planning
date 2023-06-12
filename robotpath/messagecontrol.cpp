#include "messagecontrol.h"
#include "ui_messagecontrol.h"
#include "q_learning.h"
#include "mainwindow.h"
#include <QDebug>
#include<QPainter>
#include <QMouseEvent>

MessageControl::MessageControl(QWidget *parent, Q_learning *q_learning,int x,int y) :
    QDialog(parent),
    ui(new Ui::MessageControl)
{
    ui->setupUi(this);

        selectedColor.setRgb(195, 250, 232);    //选中颜色为青色
        defaultColor.setRgb(255,255,255);   //默认颜色为白色
        barrierColor.setRgb(173, 181, 189); //设置障碍颜色为灰色
        candidateColor.setRgb(235, 251, 238);    //设置候补颜色为浅蓝色
        pathColor.setRgb(32, 201, 151);  //设置路径颜色为浅红色
        startColor.setRgb(24, 100, 171); //设置起点颜色为蓝色
        endColor.setRgb(201, 42, 42); //设置终点颜色为红色
        winColor.setRgb(88, 251, 238);    //设置胜利颜色为浅蓝色

        this->setToolTip(QString("x : %1 , y : %2\n").arg(y).arg(x) );  //隐藏标签，鼠标放上可显示坐标

        this->pos_x = x;
        this->pos_y = y;

        this->setAttribute(Qt::WA_KeyboardFocusChange,true);
        this->setAutoFillBackground(true);
}

MessageControl::~MessageControl()
{
    delete ui;
}

void MessageControl::setCellType(CellType type)
{

     this->type = type;
    if(type == MessageControl::Null)
      {

           ui->widget->setStyleSheet(QString("background-color:%1;").arg(defaultColor.name()));    // 设置背景色

      }
    if(type == MessageControl::Barrier)
      {
           ui->widget->setStyleSheet(QString("background-color:%1;").arg(barrierColor.name()));    // 设置背景色


      }
    if(type == MessageControl::Candidate)
      {
           ui->widget->setStyleSheet(QString("background-color:%1;").arg(candidateColor.name()));    // 设置背景色

      }
   if(type == MessageControl::Path)
      {

           ui->widget->setStyleSheet(QString("background-color:%1;").arg(pathColor.name()));    // 设置背景色

      }
   if(type == MessageControl::Selected)
   {
           ui->widget->setStyleSheet(QString("background-color:%1;").arg(selectedColor.name()));    // 设置背景色

   }
    if(type ==  MessageControl::Start)
    {
           MessageControl::palette.setBrush(QPalette::Active, QPalette::Base, QBrush(Qt::green));     // 设置背景色
           this->setPalette(palette);
    }
    if(type == MessageControl::End)
    {
          MessageControl::palette.setBrush(QPalette::Active, QPalette::Base, QBrush(Qt::blue));       // 设置背景色
          this->setPalette(palette);
    }
    if(type == MessageControl::Win)
    {
          ui->widget->setStyleSheet(QString("background-color:%1;").arg(winColor.name()));    // 设置背景色
    }

}

//鼠标控制栅格选择
void MessageControl::mousePressEvent(QMouseEvent *event)        //摁住鼠标事件
{
    if(this->type == MessageControl::Selected)             //如果已经选中了。再次点击及取消选中状态
    {
        this->setCellType(MessageControl::Null);
        emit this->selectedCancelIndex(pos_y,pos_x);         //给mainwindows类发送一个信号
    }
    else
    {
        this->setCellType(MessageControl::Selected);
        emit this->selectedCellIndex(pos_y,pos_x);      //给mainwindows类发送一个信
    }
}

