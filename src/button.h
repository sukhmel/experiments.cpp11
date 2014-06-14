#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QToolButton>

class Button : public QToolButton
{
    Q_OBJECT

public:
    explicit Button( const QString &text
                   , QWidget *parent = 0);

    QSize sizeHint() const;
};

#endif
