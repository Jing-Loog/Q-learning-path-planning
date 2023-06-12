#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "messagecontrol.h"
#include <QMainWindow>
#include <QTimer>
#include <QVector>

//栅格尺寸布局定义
#define COST_DIET 10    //定义一格花费10
#define COST_LINE 14    //对角线花费14

#define TABLE_COLUMN 22
#define TABLE_ROW 14

#define WIDTH 50
#define HEIGHT 50

#define default_ms 50





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
     QVector<QPoint> selected_xy;   //坐标的记录

     int start_x = 0;   //起点坐标
     int start_y = 0;

     int walktimes=0;
     int Greedy=20;

     int end_x = TABLE_COLUMN;     //终点坐标
     int end_y = TABLE_ROW;

     void Reinforce();

public slots:
       void selectedCellIndex(int x,int y);
       void selectedCancelIndex(int x,int y);

private slots:
       void on_pushButton_barrier_clicked();
       void on_pushButton_clear_clicked();
       void on_pushButton_find_clicked();
       void on_pushButton_clicked();
       void on_pushButton_cliQ_clicked();
};
#endif // MAINWINDOW_H



