#ifndef IOHELPER_H
#define IOHELPER_H

#include <QFile>
#include <QFileDialog>
#include <QDataStream>

class IoHelper
{
public:
    IoHelper() {}

    template <class T>
    static bool save (T* obj, QWidget *parent = 0)
    {
        QString filename =
            QFileDialog::getSaveFileName
                ( parent
                , "Save game"
                , ""
                , "Mahjong saves (*.mjg)"
                , 0
                , QFileDialog::HideNameFilterDetails);

        if (!filename.isEmpty())
        {
            if (!filename.endsWith(".mjg"))
            {
                filename += ".mjg";
            }
            QFile file(filename);
            file.open(QFile::WriteOnly);
            QDataStream out(&file);
            out << *obj;
            file.close();
        }

        return !filename.isEmpty();
    }

    template <class T>
    static bool load (T* obj, QWidget *parent = 0)
    {
        QString filename =
            QFileDialog::getOpenFileName
                ( parent
                , "Save game"
                , ""
                , "Mahjong saves (*.mjg)"
                , 0
                , QFileDialog::HideNameFilterDetails);

        if (!filename.isEmpty())
        {
            QFile file(filename);
            file.open(QFile::ReadOnly);

            QDataStream in(&file);
            in >> *obj;

            file.close();
        }

        return !filename.isEmpty();
    }
};

#endif // IOHELPER_H
