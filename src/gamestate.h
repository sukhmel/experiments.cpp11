#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <cstring>
#include <QTextStream>

class GameState : public QObject
{
    Q_OBJECT
public:
    explicit GameState(QObject *parent = 0);

    int xyToLinear(int x, int y) const;

    int getEast()   const;
    int getWinner() const;

    int getScore  (int a) const;
    int getTotal  (int a) const;
    int getOverall(int a) const;

    bool isPlaying(int a) const;

    int getResult (int a) const;
    int getResult (int x, int y) const;

    QString getOverallHistory () const;

signals:
    void stateChanged();

public slots:
    void undo();                // go back one step in history
    void sumUp();               // add current totals and overall
                                //     and clean all scores/winner
    void setEast     (int p);   // set new East Player position
    void setWinner   (int p);   //
    void setPlayer(int player
                  , bool on);   // toggle Nth player on/off
    void setScore( int player
                 , int score);  // set score for Nth player

protected:
    struct State
    {
        int eastPosition    = 0;
        int winnerPosition  = -1;
        bool playersSet [4] = { false
                              , false
                              , false
                              , false
                              };
        int results [12]    = {};
        int scores  [ 4]    = {};
        int totals  [ 4]    = {};
        int overall [ 4]    = {};
    } state;

    QVector<State> history;

    void update();

    void record();  // update history with current state,
                    //       and emit stateChange signal.
};

#endif // GAMESTATE_H
