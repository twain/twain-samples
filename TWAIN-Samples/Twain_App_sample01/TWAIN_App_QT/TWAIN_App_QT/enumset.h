#ifndef ENUMSET_H
#define ENUMSET_H

#include <QtWidgets/QDialog>
#include "ui_enumset.h"

class EnumSet : public QDialog
{
    Q_OBJECT

public:
    EnumSet(QWidget *parent = 0);
    ~EnumSet();

//private:
    Ui::EnumSetClass ui;
};

#endif // ENUMSET_H
