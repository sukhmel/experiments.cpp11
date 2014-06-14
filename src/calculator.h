#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QBoxLayout>
#include <QIntValidator>

#include <math.h>
#include <functional>

#include "button.h"
#include "gamestate.h"

class MahjCalc : public QWidget
{
    Q_OBJECT

public:
    MahjCalc(QWidget *parent = 0);

public slots:
    void onStateChanged();

protected:
    Button *sum;
    Button *undo;
    Button *save;

    Button *east       [ 4];
    Button *winner     [ 4];

    QLineEdit *totals  [ 4];
    QLineEdit *overall [ 4];
    QLineEdit *results [12];

    QLineEdit *scores  [ 4];
    QLineEdit *players [ 4];

    QTextEdit *history;

    GameState *game;

    int currentWinner = -1;

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
};

#endif
