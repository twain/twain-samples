#ifndef QT_PROFILENAME_H
#define QT_PROFILENAME_H

#include <QtWidgets/QDialog>

namespace Ui {
    class ProfileName;
}

class ProfileName : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ProfileName)
public:
    explicit ProfileName(QWidget *parent = 0);
    virtual ~ProfileName();
    QString GetProfileName();
protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ProfileName *m_ui;
};

#endif // QT_PROFILENAME_H
