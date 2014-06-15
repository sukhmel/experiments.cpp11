#include "gamestate.h"

GameState::GameState(QObject *parent) :
    QObject(parent)
{
}

int GameState::xyToLinear(int x, int y)
{
    return x == y ?
               -1 :
                x + y * 3 - (x > y ?
                                 1 :
                                 0);
}

// --------- GETTER FUNCTIONS -----------

int GameState::getEast() const
{
    return state.eastPosition;
}

int GameState::getWinner() const
{
    return state.winnerPosition;
}

int GameState::getScore(int a) const
{
    return state.scores [a];
}

int GameState::getTotal(int a) const
{
    return state.totals [a];
}

int GameState::getOverall(int a) const
{
    return state.overall[a];
}

bool GameState::isPlaying(int a) const
{
    return state.playersSet[a];
}

int GameState::getResult(int a) const
{
    return state.results[a];
}

int GameState::getResult(int x, int y) const
{
    int index = xyToLinear(x, y);
    return index < 0 ? -1 : state.results[index];
}

QString GameState::getOverallHistory() const
{
    QString column [4];
    QTextStream* b [4];
    for (int i = 0; i < 4; ++i)
    {
        b[i] = new QTextStream(&column[i]);
    }

    int temp [4] = {0,0,0,0};
    int winner   = -1;

    for (auto rec : history)
    {
        if (0 != std::memcmp(temp, rec.overall, 4*sizeof(int)))
        {
            std::memcpy(temp, rec.overall, 4*sizeof(int));

            for (int i = 0; i < 4; ++i)
            {
                if (i == winner)
                {
                    *b[i] << "<u>";
                }

                *b[i] << temp[i];

                if (i == winner)
                {
                    *b[i] << "</u>";
                }

                *b[i] << "<br>";
            }
        }
        winner = rec.winnerPosition;
    }

    if (0 != std::memcmp(temp, state.overall, 4*sizeof(int)))
    {
        for (int i = 0; i < 4; ++i)
        {
            if (i == winner)
            {
                *b[i] << "<i>";
            }

            *b[i] << state.overall[i] << "\n";

            if (i == winner)
            {
                *b[i] << "</i>";
            }
        }
    }

    QString answer;
    QTextStream result(&answer);
    for (int i = 0; i < 4; ++i)
    {
        if (isPlaying(i))
        {
            result << column[i];
        }
        result << "##";
        delete b[i];
    }
    return answer;
}

// ------------ REACTING ON SIGNALS -------------

void GameState::undo()
{
    if (!history.isEmpty())
    {
        state = history.takeLast();
    }
    else
    {
        state = State{};
    }
    emit stateChanged();
}

void GameState::sumUp()
{
    record();
    State temp;
    for (int i = 0; i < 4; ++i)
    {
        temp.overall[i]    = state.overall[i]
                           + state.totals [i];
        temp.playersSet[i] = state.playersSet[i];
    }
    temp.eastPosition = (state.eastPosition +
                       ( state.eastPosition ==
                         state.winnerPosition
                         ? 0 : 1) ) % 4 ;
    state = temp;
    update();
}

void GameState::setEast(int p)
{
    record();
    state.eastPosition = p;
    update();
}

void GameState::setWinner(int p)
{
    record();
    state.winnerPosition =
            p == state.winnerPosition ?
                                   -1 :
                                    p ;
    update();
}

void GameState::setPlayer(int p, bool on)
{
    if (on != state.playersSet[p])
    {
        record();
        state.playersSet[p] = on;
        update();
    }
}

void GameState::setScore(int player, int score)
{
    if (state.scores[player] != score)
    {
        record();
        state.scores[player] = score;
        update();
    }
}

void GameState::record()
{
    history.append(state);
}

void GameState::update()
{
    state.update();
    emit stateChanged();
}

void GameState::State::update()
{
    std::memset(totals, 0, 4*sizeof(int));

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            if (  x  !=  y  )
            {
                // score difference -- main reason of calculus
                int diff = 0;

                if ( playersSet[x]
                  && playersSet[y] )
                {
                    diff = scores[x] - scores[y];
                    if ( x == winnerPosition )
                    {
                        diff =  scores[x];    // winner only receives
                    }
                    if ( y == winnerPosition )
                    {
                        diff = -scores[y];    // payments to winner
                    }
                    if ( x == eastPosition
                      || y == eastPosition )
                    {
                        diff *= 2;          // east pays and receives twice
                    }
                }

                results[xyToLinear(x, y)] = diff;
                totals[x] += diff;
            }
        }
    }
}

// -------------- SERIALIZATION ---------------

QDataStream &operator<<(QDataStream &out, const GameState::State &data)
{
   out << qint8(data.eastPosition)
       << qint8(data.winnerPosition);

   for (int i = 0; i < 4; ++i)
   {
       out <<        data.playersSet[i]
           << qint32(data.scores    [i])
           << qint32(data.overall   [i]);
   }
   return out;
}


QDataStream &operator>>(QDataStream &in, GameState::State &data)
{
    qint8 east, winner;
    qint32 score, sums;

    in >> east >> winner;
    data.eastPosition   = east;
    data.winnerPosition = winner;

    for (int i = 0; i < 4; ++i)
    {
        in >> data.playersSet[i]
           >> score   >>   sums;

        data.scores [i] = score;
        data.overall[i] = sums ;
    }

    data.update();

    return in;
}


QDataStream &operator<<(QDataStream &out, const GameState &data)
{
    out << data.state;
    out << data.history;

    return out;
}


QDataStream &operator>>(QDataStream &in, GameState &data)
{
    in >> data.state
       >> data.history;

    data.update();

    return in;
}
