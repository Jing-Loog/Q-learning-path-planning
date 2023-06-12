#include "q_learning.h"
#include<iostream>
#include<cstdlib>
#include<algorithm>
#include<vector>
#include <QMessageBox>
#include<math.h>
using namespace std;
Q_learning::Q_learning()
{
    this->Qtable[TABLE_COLUMN+2][TABLE_ROW+2][9];
    this->R=0.8;
    this->r[TABLE_COLUMN+2][TABLE_ROW+2];
}

const int up=1;
const int down =2;
const int Left =3;
const int Right=4;
const int leftup=5;
const int leftdown=6;
const int rightup=7;
const int rightdown=8;
const double S=-5;


void Q_learning::actpunish()
{


    for(int i=0;i<=TABLE_ROW;i++)
      {
              Qtable[0][i][leftup] = S;
              Qtable[0][i][leftdown] = S;
              Qtable[0][i][Left] = S;
              Qtable[TABLE_COLUMN][i][Right] = S;
              Qtable[TABLE_COLUMN][i][rightup] = S;
              Qtable[TABLE_COLUMN][i][rightdown] = S;
      }
      for(int i=0;i<=TABLE_COLUMN;i++)
      {

              Qtable[i][0][up] = S;
              Qtable[i][0][leftup] = S;
              Qtable[i][0][rightup] = S;
              Qtable[i][TABLE_ROW][down] = S;
              Qtable[i][TABLE_ROW][leftdown] = S;
              Qtable[i][TABLE_ROW][rightdown] = S;
      }
        Qtable[TABLE_COLUMN][TABLE_ROW][Right] = 0;
        Qtable[TABLE_COLUMN][TABLE_ROW][down] = 0;
        Qtable[TABLE_COLUMN][TABLE_ROW][rightdown] = 0;


      srand(time(0));  //种一个随机种子，后面选择动作要用

}



 //判断下一步选择中最大的奖励值
void Q_learning::initplace(int& x,int& y,int& den)   //初始化机器人位置
{
    x=0;
    y=0;
    den=0;     //是否到终点，不到都为0;
}
double Q_learning::get_expected_max_reward(int x,int y)      //得到最大期望Q值，用来后面传播到旧状态改变其Q值
{
   double Nexstpmaxrd = -10000;
   for(int i=1;i<=8;i++)
   {
       Nexstpmaxrd = max(Nexstpmaxrd,Qtable[x][y][i]);
   }
    return Nexstpmaxrd;
}

//行为函数
double Q_learning::action(int act , int& x,int& y,int& den)
{
   //边界的惩罚
   if((x==0 && act == Left) || (x==0 && act == leftup) || (x==0 &&  act == leftdown))
   {
       flag=0;
      return S;                                 //S为遇到边界和障碍的惩罚值，会在main函数或别处全局定义，统一值为-5;
   }
   if((x==TABLE_COLUMN  && act == Right ) || (x==TABLE_COLUMN && act == rightup) || (x==TABLE_COLUMN &&  act == rightdown))
   {
       flag=0;
       return S;
   }
   if((y ==0 && act == up) || (y==0 && act == leftup) || (y==0 &&  act == rightup))
   {
       flag=0;
       return S;
   }
   if((y==TABLE_ROW && act == down) || (y==TABLE_ROW && act == leftdown) || (y==TABLE_ROW &&  act == rightdown))
   {
       flag=0;
       return S;
    }
   //障碍物惩罚
   if((barrier_xy.contains(QPoint(x+1,y)) && (act==Right || act == rightup || act==rightdown)) || (barrier_xy.contains(QPoint(x,y+1)) && (act==down || act == rightdown || act==leftdown)) || (barrier_xy.contains(QPoint(x,y-1)) && (act== ( up || rightup || leftup) )) || (barrier_xy.contains(QPoint(x-1,y)) && (act==Left || act == leftup || act==leftdown))
     || (barrier_xy.contains(QPoint(x+1,y+1)) && (act==rightdown)) || (barrier_xy.contains(QPoint(x-1,y+1)) && (act==leftdown)) || (barrier_xy.contains(QPoint(x+1,y-1)) && (act== rightup ))  || (barrier_xy.contains(QPoint(x-1,y-1)) && (act==leftup )))  //格子为障碍物，barrier为vector类型，在其他类中定义，里面存储了障碍物的横纵坐标（points)
   {
       flag=0;
       return S;
   }

   //其他行为，（正常行走）,八叉树
   if(act == up)
   {
      y--;
     length++;
   }
   if(act == down)
   {
       y++;
      length++;
   }
   if(act == Left)
   {
       x--;
       length++;
   }
   if(act == Right)
   {
       x++;
       length++;
   }
   if(act == leftup)
   {
       x--;
       y--;
       length+=sqrt(2);
   }
   if(act == leftdown)
   {
       x--;
       y++;
       length+=sqrt(2);
   }
   if(act == rightup)
   {
       x++;
       y--;
       length+=sqrt(2);
   }
    if(act == rightdown)
   {
       x++;
       y++;
       length+=sqrt(2);
   }

    //走到障碍周围一圈的格子，立即奖励-1，因为贴近障碍，该行为不好
    if((barrier_xy.contains(QPoint(x+1,y)))|| (barrier_xy.contains(QPoint(x,y+1)) ) || (barrier_xy.contains(QPoint(x,y-1))) || (barrier_xy.contains(QPoint(x-1,y)))
      || (barrier_xy.contains(QPoint(x+1,y+1)) ) || (barrier_xy.contains(QPoint(x-1,y+1)) ) || (barrier_xy.contains(QPoint(x+1,y-1)) && (act== rightup ))  || (barrier_xy.contains(QPoint(x-1,y-1)) ))  //格子为障碍物，barrier为vector类型，在其他类中定义，里面存储了障碍物的横纵坐标（points)
    {
        r[x][y]-=1;
    }

    flag=1;

    arrived_xy.clear();    //清空旧位置
    arrived_xy.append(QPoint(x,y));     //追加新位置
   if(x == TABLE_COLUMN && y == TABLE_ROW )   //到达终点
   {
       den = 1;
       return 100;           //终点的奖励;
   }
   else if(x >= 0 && x < TABLE_COLUMN && y >= 0 && y < TABLE_ROW )   //正常走，非终点
   {
     double noractrew = get_expected_max_reward(x,y);
     return r[x][y] + R * noractrew ;                //计算最大奖励值
   }
   else
   return 0;
}






























