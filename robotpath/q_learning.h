#ifndef Q_LEARNING_H
#define Q_LEARNING_H

#include <QMainWindow>
#include<QVector>

#define TABLE_COLUMN 22
#define TABLE_ROW 14
class Q_learning
{
public:
    Q_learning();
    friend class MainWindow;
    double R=0.8;   //折扣因子
    int flag=1;              //用来记录机器人是否位置改变，1为改变
    int over;               //判定是否到达终点
    double length=0.0;  //打印路径长度
    double Qtable[TABLE_COLUMN+2][TABLE_ROW+2][8]={0.0};     //存放奖励函数Q的表，每次训练更新一遍，8(方向)+1(障碍)+4(边沿)=13
    double r[TABLE_COLUMN+2][TABLE_ROW+2]={0.0};
    double action(int act , int& x,int& y,int& den);
    void initplace(int& x,int& y,int& den);
    double get_expected_max_reward(int x,int y);
    void actpunish();


    QVector<QPoint> barrier_xy; //障碍物的坐标
    QVector<QPoint> arrived_xy; //已走过的坐标
};

#endif // Q_LEARNING_H
