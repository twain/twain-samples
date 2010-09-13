#include "stdafx.h"
#include "setstate.h"

SetState::SetState(QWidget *pParent, int nInitialState)
    : QDialog(pParent)
{
  ui.setupUi(this);
  //initlialize the state
  ui.spnState->setValue(nInitialState);
  //update the current state
  m_nState = nInitialState;
  return;
}

SetState::~SetState()
{
  //does nothing
  return;
}

void SetState::accept()
{
  //Update the state member variable
  m_nState = ui.spnState->value();
  //Call the base version of this signal
  QDialog::accept();
  return;
}
