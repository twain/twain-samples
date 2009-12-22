#include "Qt_ProfileName.h"
#include "ui_ProfileName.h"

ProfileName::ProfileName(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ProfileName)
{
    m_ui->setupUi(this);
    m_ui->lineEdit->setFocus();
}

ProfileName::~ProfileName()
{
    delete m_ui;
}

void ProfileName::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString ProfileName::GetProfileName()
{
  return m_ui->lineEdit->text();
}
