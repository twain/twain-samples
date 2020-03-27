#ifndef QT_ABOUT_H
#define QT_ABOUT_H

#include <QtWidgets/QDialog>

namespace Ui {
    class About;
}

class About : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(About)
public:
    explicit About(QWidget *parent = 0);
    virtual ~About();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::About *m_ui;
};

#endif // QT_ABOUT_H
