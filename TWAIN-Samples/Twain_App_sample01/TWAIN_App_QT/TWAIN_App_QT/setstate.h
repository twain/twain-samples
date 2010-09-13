/**
* @file setstate.h
* @brief  Contains the Declaration of the SetState dialog
* @author JFL Peripheral Solutions Inc.
* @date September 2010
*/
#ifndef SETSTATE_H
#define SETSTATE_H

#include <QDialog>
#include "ui_setstate.h"

class SetState : public QDialog
{
  Q_OBJECT

  public:
  /**
  * Constructor initializes members and presets the state spin button
  * @param pParent pointer to the parent Widget
  * @param nInitialState the initial state for present
  */
  SetState(QWidget *pParent= 0, int nInitialState=5);
  /**
  * Destructor
  */
  ~SetState();
  int m_nState; /**< state resulting from user input into this dialog - invalid if result is reject */

  protected:
  /**
  * Called when the user selects "ok"
  */
  virtual void accept();

  private:
  Ui::SetStateClass ui; /**< QT Generated member containing the GUI members */
};

#endif // SETSTATE_H
