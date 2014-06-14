#include "gamestate.h"

GameState::GameState(QObject *parent) :
    QObject(parent)
{
}

int GameState::xyToLinear(int x, int y) const
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
    QString result = "";
    QTextStream builder(&result);

    int temp [4] = {0,0,0,0};

    for (auto rec : history)
    {
        if (0 != std::memcmp(temp, rec.overall, 4*sizeof(int)))
        {
            std::memcpy(temp, rec.overall, 4*sizeof(int));

            for (int i = 0; i < 4; ++i)
            {
                if (isPlaying(i))
                {
                    builder << "\t"
                            << qSetFieldWidth(7)
                            << right << temp[i];
                }
            }
            builder << "\n";
        }
    }

    if (0 != std::memcmp(temp, state.overall, 4*sizeof(int)))
    {
        for (int i = 0; i < 4; ++i)
        {
            if (isPlaying(i))
            {
                builder << qSetFieldWidth(6)
                        << center
                        << state.overall[i];
            }
        }
    }
    return result;
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

void GameState::update()
{
    std::memset(state.totals, 0, 4*sizeof(int));

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            if (  x  !=  y  )
            {
                // score difference -- main reason of calculus
                int diff = 0;

                if ( isPlaying(x)
                  && isPlaying(y) )
                {
                    diff = state.scores[x] - state.scores[y];
                    if ( x == state.winnerPosition )
                    {
                        diff =  state.scores[x];    // winner only receives
                    }
                    if ( y == state.winnerPosition )
                    {
                        diff = -state.scores[y];    // payments to winner
                    }
                    if ( x == state.eastPosition
                      || y == state.eastPosition )
                    {
                        diff *= 2;          // east pays and receives twice
                    }
                }

                state.results[ xyToLinear(x, y)] = diff;
                state.totals[x] += diff;
            }
        }
    }
    emit stateChanged();
}

void GameState::record()
{
    history.append(state);
}
