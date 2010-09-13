#ifndef METERDLG_H
#define METERDLG_H

#include <QDialog>
#include "ui_meterdlg.h"

class MeterDlg : public QDialog
{
    Q_OBJECT

public:
    MeterDlg(QWidget *parent = 0);
    ~MeterDlg();

//private:
    Ui::MeterDlgClass ui;
};

#endif // METERDLG_H
