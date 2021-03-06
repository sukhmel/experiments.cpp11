#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QLabel>
#include <QEvent>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QBoxLayout>
#include <QScrollBar>
#include <QIntValidator>

#include <math.h>
#include <functional>

#include "button.h"
#include "iohelper.h"
#include "gamestate.h"

class MahjCalc : public QWidget
{
    Q_OBJECT

protected:
    Button *east       [ 4];
    Button *winner     [ 4];
    Button *control    [ 4];

    QLineEdit *totals  [ 4];
    QLineEdit *results [12];

    QLineEdit *scores  [ 4];
    QLineEdit *players [ 4];

    QTextEdit *overall;

    GameState *game = 0;

    int currentWinner = -1;

    bool eventFilter(QObject *object, QEvent *event);

    void setLayout();
    void setControls();
    void setConnections();

    template <class T, class Q, class A, typename F>
    void fillNumbered ( T** list
                      , void (Q::*sent)(A)
                      , F f
                      , int increase
                      , Qt::FocusPolicy fp = Qt::StrongFocus)
    {
        for (int i = 0; i < 4; ++i)
        {
            list[i] = new T("", this);
            list[i]->setFocusPolicy(fp);
            connect(list[i], sent, [=](A a){f(i, a);});

            growTextSize(list[i], increase);
        }
    }

    template <class T>
    void fillTextual ( T** list
                     , int amount
                     , int increase
                     , Qt::Alignment a = Qt::AlignCenter)
    {
        for (int i = 0; i < amount; ++i)
        {
            list[i] = new T("0", this);
            list[i]->setReadOnly(true);
            list[i]->setAlignment(a);
            list[i]->setFocusPolicy(Qt::NoFocus);

            growTextSize(list[i], increase);
        }
    }

    template <class T>
    void autoSetText ( T* element
                     , int index
                     , int (GameState::*f)(int) const)
    {
        element[index]->setText(QString("%1").arg((game->*f)(index)));
    }

    template <class W>
    void growTextSize ( W* widget
                      , int increase)
    {
            QFont font = widget->font();
            font.setPointSize(font.pointSize() + increase);
            widget->setFont(font);
    }

public:
    MahjCalc(QWidget *parent = 0);

    friend QDataStream &operator<<(QDataStream &out, const MahjCalc &data);
    friend QDataStream &operator>>(QDataStream &in,        MahjCalc &data);

public slots:
    void onStateChanged();
    void onGameLoad();
    void onGameSave();
};
QDataStream &operator<<(QDataStream &out, const MahjCalc &data);
QDataStream &operator>>(QDataStream &in,        MahjCalc &data);

#endif
