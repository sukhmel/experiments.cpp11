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

        void update();
    } state;

    QVector<State> history;

    void update();  // update state and emit stateChange signal.
    void record();  // update history with current state,

public:
    explicit GameState(QObject *parent = 0);

    static int xyToLinear(int x, int y);

    int getEast()   const;
    int getWinner() const;

    int getScore  (int a) const;
    int getTotal  (int a) const;
    int getOverall(int a) const;

    bool isPlaying(int a) const;

    int getResult (int a) const;
    int getResult (int x, int y) const;

    QString getOverallHistory () const;

    friend QDataStream &operator<<(QDataStream &out, const GameState &data);
    friend QDataStream &operator>>(QDataStream &in,        GameState &data);
    friend QDataStream &operator<<(QDataStream &out, const GameState::State &data);
    friend QDataStream &operator>>(QDataStream &in,        GameState::State &data);

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

};
QDataStream &operator<<(QDataStream &out, const GameState        &data);
QDataStream &operator>>(QDataStream &in,        GameState        &data);
QDataStream &operator<<(QDataStream &out, const GameState::State &data);
QDataStream &operator>>(QDataStream &in,        GameState::State &data);

#endif // GAMESTATE_H
