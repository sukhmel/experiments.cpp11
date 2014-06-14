#include "calculator.h"

MahjCalc::MahjCalc(QWidget *parent)
    : QWidget(parent)
{
    int f = 4;
    game = new GameState(this);
    connect(game, &GameState::stateChanged, this, &MahjCalc::onStateChanged);

    fillNumbered( east
                , &Button::clicked
                , [&](int i, bool)
                     {
                         game->setEast(i);
                     }
                , 3*f );

    fillNumbered( players
                , &QLineEdit::textEdited
                , [&](int i, const QString &t)
                     {
                         game->setPlayer(i, !t.isEmpty());
                     }
                , f );

    for (int i = 0; i < 4; ++i) { players[i]->setAlignment(Qt::AlignCenter); }

    fillNumbered( scores
                , &QLineEdit::textEdited
                , [&](int i, const QString &t)
                     {
                         game->setScore(i, t.toInt());
                     }
                , f);

    for (int i = 0; i < 4; ++i)
    {
        scores[i]->installEventFilter(this);
        scores[i]->setAlignment(Qt::AlignRight);
        scores[i]->setValidator(new QIntValidator(0, 3000, this));
    }

    fillNumbered( winner
                , &Button::clicked
                , [&](int i, bool)
                     {
                         game->setWinner(i);
                     }
                , f );

    sum  = new Button("Sum up", this);
    undo = new Button("Undo", this);
    save = new Button("Save", this);

    growTextSize( sum, f);
    growTextSize(undo, f);
    growTextSize(save, f);
    save->setEnabled(false);

    connect(sum,  &Button::clicked, game, &GameState::sumUp);
    connect(undo, &Button::clicked, game, &GameState::undo );

    fillTextual( totals,   4, f);
    fillTextual( overall,  4, f);
    fillTextual( results, 12, f
               , Qt::AlignRight);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if ( i != j )
            {
                connect( overall[i]->verticalScrollBar(), &QScrollBar::valueChanged
                       , overall[j]->verticalScrollBar(), &QScrollBar::setValue  );
            }
        }
    }

    QBoxLayout *mainColumn = new QBoxLayout(QBoxLayout::TopToBottom);
    mainColumn->setSpacing(15);

    QBoxLayout *upperRow = new QBoxLayout(QBoxLayout::LeftToRight);
    upperRow->setSpacing(0);

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
        tempColumn->addWidget(overall[i], 1);

        upperRow->addLayout(tempColumn);
    }

    QBoxLayout *lowerRow = new QBoxLayout(QBoxLayout::LeftToRight);
    lowerRow->setSpacing(15);
    lowerRow->addWidget(sum );
    lowerRow->addWidget(undo);
    lowerRow->addWidget(save);

    mainColumn->addLayout(  upperRow);
    mainColumn->addLayout(  lowerRow);

    setLayout(mainColumn);

    setWindowTitle(tr("Mahjong Calculator"));

    onStateChanged();
}

void MahjCalc::onStateChanged()
{
    QStringList history = game->getOverallHistory().split("##");

    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    centerFormat.setTopMargin(0);
    centerFormat.setLeftMargin(0);
    centerFormat.setRightMargin(0);
    centerFormat.setBottomMargin(0);

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

        { // fancy overall with colors
            QString format = game->getOverall(i) < 0 ?
                                "rgb(250, 200, 200)" :
                             game->getOverall(i) > 0 ?
                                "rgb(200, 250, 200)" :
                                "rgb(200, 200, 200)" ;

            overall[i]->clear();
            QTextCursor cursor(overall[i]->document());
            cursor.movePosition(QTextCursor::End);
            cursor.insertBlock();
            cursor.mergeBlockFormat(centerFormat);
            cursor.insertText(history.at(i));

            overall[i]->setAlignment(Qt::AlignCenter);
            overall[i]->setEnabled(active);
            overall[i]->setStyleSheet("color: black; background-color: " + format);
        }

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
