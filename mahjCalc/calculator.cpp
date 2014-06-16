#include "calculator.h"

MahjCalc::MahjCalc(QWidget *parent)
    : QWidget(parent)
    , game(new GameState(this))
{
    setControls();

    setConnections();

    setLayout();

    setWindowTitle(tr("Mahjong Calculator"));

    onStateChanged();
}

void MahjCalc::setLayout()
{
    QBoxLayout *mainCol = new QBoxLayout(QBoxLayout::TopToBottom);
    mainCol->setSpacing(15);

    QBoxLayout *topRow = new QBoxLayout(QBoxLayout::LeftToRight);
    topRow->setSpacing(0);

    QBoxLayout *bottomRow = new QBoxLayout(QBoxLayout::LeftToRight);
    bottomRow->setSpacing(0);

    for (int i = 0; i < 4; ++i)
    {
        QBoxLayout *tempColumn = new QBoxLayout(QBoxLayout::TopToBottom);
        tempColumn->setSpacing(10);

        QBoxLayout *scorColumn = new QBoxLayout(QBoxLayout::TopToBottom);
        scorColumn->setSpacing(0);

        for (int j = 0; j < 4; ++j)
        {
            int a = game->xyToLinear(i, j);
            scorColumn->addWidget( a  <  0  ?
                                 scores [i] :
                                 results[a]);
        }

        tempColumn->addWidget(east   [i]);
        tempColumn->addWidget(players[i]);
        tempColumn->addLayout(scorColumn);
        tempColumn->addWidget(winner [i]);
        tempColumn->addWidget(totals [i]);

        bottomRow->addWidget(control[i]);

        topRow->addLayout(tempColumn);
    }

    mainCol->addLayout(topRow);
    mainCol->addWidget(overall, 1);
    mainCol->addLayout( bottomRow);

    QWidget::setLayout(mainCol);
}

void MahjCalc::setControls()
{
#ifdef Q_OS_ANDROID
    #define FONTGROW      2
    #define BIG_FONTGROW 16
#else
    #define FONTGROW      4
    #define BIG_FONTGROW 12
#endif

    fillNumbered( east
                , &Button::clicked
                , [&](int i, bool)
                     {
                         game->setEast(i);
                     }
                , BIG_FONTGROW  );

    fillNumbered( players
                , &QLineEdit::textEdited
                , [&](int i, const QString &t)
                     {
                         game->setPlayer(i, !t.isEmpty());
                     }
                , FONTGROW  );

    fillNumbered( scores
                , &QLineEdit::textEdited
                , [&](int i, const QString &t)
                     {
                         game->setScore(i, t.toInt());
                     }
                , FONTGROW );

    fillNumbered( winner
                , &Button::clicked
                , [&](int i, bool)
                     {
                         game->setWinner(i);
                     }
                , FONTGROW );

    Button  *sum = new Button("Sum up", this);
    Button *undo = new Button("Undo", this);
    Button *load = new Button("Load", this);
    Button *save = new Button("Save", this);

    fillTextual( totals,   4, FONTGROW );
    fillTextual( results, 12, FONTGROW
               , Qt::AlignRight);

    overall = new QTextEdit(this);
    overall->setReadOnly(true);
    overall->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    overall->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    growTextSize(overall, FONTGROW);

    control[0] =  sum;
    control[1] = undo;
    control[2] = load;
    control[3] = save;

    for (int i = 0; i < 4; ++i)
    {
        players[i]->setAlignment(Qt::AlignCenter);
        players[i]->setTextMargins(0,0,0,0);

        scores[i]->installEventFilter(this);
        scores[i]->setAlignment(Qt::AlignRight);
        scores[i]->setValidator(new QIntValidator(0, 3000, this));

        growTextSize(control[i], FONTGROW );
    }
#undef FONTGROW
#undef BIG_FONTGROW
}

void MahjCalc::setConnections()
{
    connect(game, &GameState::stateChanged, this, &MahjCalc::onStateChanged);

    connect(control[0], &Button::clicked, game, &GameState::sumUp);
    connect(control[1], &Button::clicked, game, &GameState::undo );
    connect(control[2], &Button::clicked, this, &MahjCalc::onGameLoad);
    connect(control[3], &Button::clicked, this, &MahjCalc::onGameSave);
}

void MahjCalc::onStateChanged()
{
    overall->setHtml(game->getOverallHistory());

    QTextCursor cur(overall->document());
    cur.movePosition(QTextCursor::End);

    overall->setTextCursor(cur);

    int i = 0;
    for (; i < 4; ++i)
    {
        bool active = game->isPlaying(i);
        {   // fancy wind directions setup
            QString eastText = "";
            switch ((4 + i - game->getEast()) % 4)
            {
            case 0: eastText = "東"; break;
            case 1: eastText = "南"; break;
            case 2: eastText = "西"; break;
            case 3: eastText = "北"; break;
            }

            east[i]->setText(eastText);
            east[i]->setStyleSheet((i == game->getEast()) ?
                                           "color:   red" :
                                           "color: black");
        }

        {   // fancy win/lose/draw setup
            winner[i]->setText(" ");
            winner[i]->setEnabled(active);
            winner[i]->setStyleSheet("color: black; "
                                     "background-color: "
                                     "rgb(200, 200, 200)");
            if (active)
            {
                if (i == game->getWinner())
                {
                    winner[i]->setText( "Won!" );
                    winner[i]->setStyleSheet("color: black; "
                                             "background-color: "
                                             "rgb(200, 250, 200)");
                }
                else
                {
                    if (game->getWinner() < 0)
                    {
                        winner[i]->setText( "draw" );
                    }
                    else
                    {
                        winner[i]->setText( "lost" );
                        winner[i]->setStyleSheet("color: black; "
                                                 "background-color: "
                                                 "rgb(250, 200, 200)");
                    }
                }
            }
        }

        autoSetText(totals,  i, &GameState::getTotal  );
        totals[i]->setEnabled(active);

        autoSetText(results, i, &GameState::getResult );
        results[i]->setEnabled(active);

        autoSetText(scores,  i, &GameState::getScore  );
        scores[i]->setEnabled(active);
        scores[i]->setStyleSheet(active ?
           "background-color: rgb(230, 250, 240)" : "");

        players[i]->setStyleSheet(active ?
           "background-color: rgb(230, 250, 240)" : "");
        if (!active) { players[i]->setText(""); }
    }

    for (; i < 12; ++i)
    {
        autoSetText(results, i, &GameState::getResult );
    }

    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            i = game->xyToLinear(x, y);
            results[i]->setEnabled( game->isPlaying(x)
                                 && game->isPlaying(y));
        }
    }
}

void MahjCalc::onGameLoad()
{
    IoHelper::load(this, this);
}

void MahjCalc::onGameSave()
{
    IoHelper::save(this, this);
}

bool MahjCalc::eventFilter(QObject *object, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn )
    {
        for (int i = 0; i < 4; ++i)
        {
            if ( object == scores[i] )
            {
                scores[i]->clear();
            }
        }
    }
    return false;
}


QDataStream &operator<<(QDataStream &out, const MahjCalc &data)
{
    for (int i = 0; i < 4; ++i)
    {
        out << data.players[i]->text();
    }
    out << *data.game;

    return out;
}


QDataStream &operator>>(QDataStream &in, MahjCalc &data)
{
    QString temp;
    for (int i = 0; i < 4; ++i)
    {
        in >> temp;
        data.players[i]->setText(temp);
    }
    in >> *data.game;

    return in;
}
