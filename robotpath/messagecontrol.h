#ifndef MESSAGECONTROL_H
#define MESSAGECONTROL_H
#include "q_learning.h"
#include <QDialog>

namespace Ui {
class MessageControl;
}

class MessageControl : public QDialog
{
    Q_OBJECT

public:
    explicit MessageControl(QWidget *parent = nullptr,Q_learning* q_learning = 0,int x = 0,int y = 0);
    ~MessageControl();

    enum CellType
        {
            Null = 0 , Barrier = 1 , Candidate = 2, Path = 3, Selected=4, Start=5 ,End=6, Win=7
        } type;

    QColor selectedColor;
    QColor defaultColor;
    QColor barrierColor;
    QColor candidateColor;
    QColor pathColor;
    QColor startColor;
    QColor endColor;
    QColor winColor;

    void setCellType(CellType type);
    int getX();
    int getY();
    QPalette palette;

      Q_learning* q_learning;

signals:
    void selectedCellIndex(int x,int y);    //选中栅格
    void selectedCancelIndex(int x,int y);    //取消已选中栅格



protected:
   void mousePressEvent(QMouseEvent * event);        //鼠标事件


private:
    Ui::MessageControl *ui;
    int pos_x ;
    int pos_y ;

};

#endif // MESSAGECONTROL_H
