#include "Qt_MainForm.h"
#include "ui_MainForm.h"
#include <QCloseEvent>
#include "Qt_About.h"
#include "Qt_ProfileName.h"
#include <QMessageBox>

MainForm::MainForm(QT_UI *pUI,QWidget *parent)
    : QDialog(parent), ui(new Ui::MainForm), m_valH(this),  m_valW(this)
{
  m_bSliderValueChanged = false;
  m_pUI = pUI;
  ui->setupUi(this);

  ui->pLineEdit_PaperSize_Custom_Width->setValidator(&m_valW);
  ui->pLineEdit_PaperSize_Custom_Height->setValidator(&m_valH);
  if(m_pUI->m_bSetup)
  {
    ui->pPushButton_Scan->setText(QApplication::translate("MainForm", "Save", 0, QApplication::UnicodeUTF8));
  }

  lstString strFileNames;
  pUI->TW_GetAllProfiles(strFileNames);
  lstString::iterator iter = strFileNames.begin();
  for(;iter!=strFileNames.end(); iter++)
  {
    QString strTemp = iter->c_str();
    ui->pComboBox_Profiles->addItem(strTemp);
  }
  UpdateControls();
  return;
}

MainForm::~MainForm()
{
  delete ui;
  return;
}

// Profile buttons
void MainForm::on_pPushButton_Profile_Save_clicked(bool _bChecked)
{
  ProfileName dlg;
  int nRes = dlg.exec();
  if(nRes)
  {
    QString strName = dlg.GetProfileName();
    if(ui->pComboBox_Profiles->findText(strName)>=0)
    {
      QMessageBox msgBox(QMessageBox::Critical, 
      QApplication::translate("MainForm", "Error", 0, QApplication::UnicodeUTF8),
      QApplication::translate("MainForm", "Profile already exist", 0, QApplication::UnicodeUTF8));
      msgBox.exec();
      return;
    }

    string strProfile = strName.toStdString();

    if(m_pUI->TW_SaveProfileToFile(strProfile))
    {
      ui->pComboBox_Profiles->addItem(strName);
      ui->pComboBox_Profiles->setCurrentIndex(ui->pComboBox_Profiles->count()-1);
    }
  }
  return;
}

void MainForm::on_pPushButton_Profile_Load_clicked(bool _bChecked)
{
  if(ui->pComboBox_Profiles->currentIndex()==0)
  {
    m_pUI->ResetAllCaps();
  }
  else
  {
    string strProfile = ui->pComboBox_Profiles->currentText().toStdString();
    m_pUI->TW_LoadProfileFromFile(strProfile);
  }
  UpdateControls();
  return;
}

void MainForm::on_pPushButton_Profile_Delete_clicked(bool _bChecked)
{
  string strProfile = ui->pComboBox_Profiles->currentText().toStdString();
  if(m_pUI->TW_DeleteProfile(strProfile))
  {
    ui->pComboBox_Profiles->removeItem(ui->pComboBox_Profiles->currentIndex());
  }
  return;
}

// Scan Settings controls
void MainForm::on_pComboBox_Source_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_Source->itemData(_nIndex);
  m_pUI->SetCapValueInt(CAP_FEEDERENABLED,val.toBool()?1:0);
  UpdateControls(); 
  return;
}

void MainForm::on_pComboBox_ScanSide_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_ScanSide->itemData(_nIndex);
  m_pUI->SetCapValueInt(CAP_DUPLEXENABLED,val.toBool()?1:0);
  UpdateControls(); 
  return;
}

void MainForm::on_pComboBox_ColorMode_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_ColorMode->itemData(_nIndex);
  m_pUI->SetCapValueInt(ICAP_PIXELTYPE,val.toInt());
  UpdateControls(); 
  return;
}

void MainForm::on_pComboBox_Resolution_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_Resolution->itemData(_nIndex);
  m_pUI->SetCapValueFloat(ICAP_XRESOLUTION,val.toInt());
  UpdateControls(); 
  return;
}

// Scan Area controls
void MainForm::on_pComboBox_Orientation_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_Orientation->itemData(_nIndex);
  m_pUI->SetCapValueFloat(ICAP_ORIENTATION,val.toInt());
  UpdateControls(); 
  return;
}

void MainForm::on_pCheckBox_LongDoc_stateChanged(int _nState)
{
  m_pUI->SetCapValueInt(CUSTCAP_LONGDOCUMENT,ui->pCheckBox_LongDoc->checkState()==Qt::Checked?1:0);
  UpdateControls(); 
  return;
}

void MainForm::on_pComboBox_PaperSize_StandardSizes_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_PaperSize_StandardSizes->itemData(_nIndex);
  m_pUI->SetCapValueInt(ICAP_SUPPORTEDSIZES,val.toInt());
  UpdateControls(); 
  return;
}

void MainForm::on_pLineEdit_PaperSize_Custom_Width_editingFinished()
{
  float fW = ui->pLineEdit_PaperSize_Custom_Width->text().toFloat();
  float fH = ui->pLineEdit_PaperSize_Custom_Height->text().toFloat();
  m_pUI->SetCurrentFrame(0.0f,0.0f,fH,fW);
  UpdateControls(); 
  return;
}

void MainForm::on_pLineEdit_PaperSize_Custom_Height_editingFinished()
{
  float fW = ui->pLineEdit_PaperSize_Custom_Width->text().toFloat();
  float fH = ui->pLineEdit_PaperSize_Custom_Height->text().toFloat();
  m_pUI->SetCurrentFrame(0.0f,0.0f,fH,fW);
  UpdateControls(); 
  return;
}

void MainForm::on_pComboBox_PaperSize_Custom_Units_activated(int _nIndex)
{
  QVariant val  = ui->pComboBox_PaperSize_Custom_Units->itemData(_nIndex);
  m_pUI->SetCapValueInt(ICAP_UNITS,val.toInt());
  UpdateControls(); 
  return;
}

// Sliders
void MainForm::on_pHorizontalSlider_Contrast_actionTriggered ( int action )
{
  if(action>0 && action<7)
  {
    m_bSliderValueChanged = true;
  }
}
void MainForm::on_pHorizontalSlider_Contrast_valueChanged(int value)
{
  if(m_bSliderValueChanged)
  {
    m_bSliderValueChanged = false;
    m_pUI->SetCapValueFloat(ICAP_CONTRAST,ui->pHorizontalSlider_Contrast->value());
    UpdateControls(); 
  }
}

void MainForm::on_pHorizontalSlider_Contrast_sliderReleased()
{
  m_pUI->SetCapValueFloat(ICAP_CONTRAST,ui->pHorizontalSlider_Contrast->value());
  UpdateControls(); 
  return;
}

void MainForm::on_pHorizontalSlider_Brightness_actionTriggered ( int action )
{
  if(action>0 && action<7)
  {
    m_bSliderValueChanged = true;
  }
}
void MainForm::on_pHorizontalSlider_Brightness_valueChanged(int value)
{
  if(m_bSliderValueChanged)
  {
    m_bSliderValueChanged = false;
    m_pUI->SetCapValueFloat(ICAP_BRIGHTNESS,ui->pHorizontalSlider_Brightness->value());
    UpdateControls(); 
  }
}
void MainForm::on_pHorizontalSlider_Brightness_sliderReleased()
{
  m_pUI->SetCapValueFloat(ICAP_BRIGHTNESS,ui->pHorizontalSlider_Brightness->value());
  UpdateControls(); 
  return;
}

void MainForm::on_pHorizontalSlider_Gamma_actionTriggered ( int action )
{
  if(action>0 && action<7)
  {
    m_bSliderValueChanged = true;
  }
}
void MainForm::on_pHorizontalSlider_Gamma_valueChanged(int value)
{
  if(m_bSliderValueChanged)
  {
    m_bSliderValueChanged = false;
    m_pUI->SetCapValueFloat(ICAP_GAMMA,ui->pHorizontalSlider_Gamma->value()/10);
    UpdateControls(); 
  }
}
void MainForm::on_pHorizontalSlider_Gamma_sliderReleased()
{
  m_pUI->SetCapValueFloat(ICAP_GAMMA,ui->pHorizontalSlider_Gamma->value()/10);
  UpdateControls(); 
  return;
}

void MainForm::on_pHorizontalSlider_Threshold_actionTriggered ( int action )
{
  if(action>0 && action<7)
  {
    m_bSliderValueChanged = true;
  }
}

void MainForm::on_pHorizontalSlider_Threshold_valueChanged(int value)
{
  if(m_bSliderValueChanged)
  {
    m_bSliderValueChanged = false;
    m_pUI->SetCapValueFloat(ICAP_THRESHOLD,ui->pHorizontalSlider_Threshold->value());
    UpdateControls(); 
  }
}

void MainForm::on_pHorizontalSlider_Threshold_sliderReleased()
{
  m_pUI->SetCapValueFloat(ICAP_THRESHOLD,ui->pHorizontalSlider_Threshold->value());
  UpdateControls(); 
  return;
}

// Batch control spinboxes
void MainForm::on_pSpinBox_DocsInADF_valueChanged(int _nValue)
{
  m_pUI->SetCapValueInt(CUSTCAP_DOCS_IN_ADF,_nValue);
  return;
}

// Bottom buttons
void MainForm::on_pPushButton_Scan_clicked(bool _bChecked)
{
  if(m_pUI->m_bSetup)
  {
    m_pUI->Save();
  }
  else
  {
    m_pUI->Scan();
  }
  return;
}

void MainForm::on_pPushButton_Cancel_clicked(bool _bChecked)
{
  QEvent *pQEvent = new QEvent(QEvent::Close);
  qApp->postEvent(this, pQEvent); 
  return;
}
void MainForm::closeEvent(QCloseEvent *evClose)
{
  m_pUI->Cancel();
  evClose->setAccepted (false);
  return;
}
void MainForm::hideEvent (QHideEvent *evHide) 
{
  QEvent *pQEvent = new QEvent(QEvent::Close);
  qApp->postEvent(this, pQEvent); 
  return;
}

void MainForm::on_pPushButton_Help_clicked(bool _bChecked)
{
  
  return;
}

void MainForm::on_pPushButton_About_clicked(bool _bChecked)
{
  About dlg;
  dlg.exec();
  return;
}

void MainForm::UpdateControls()
{
  ui->pComboBox_Source->clear();
  int nCapIndex = m_pUI->GetCurrentCapIndex(CAP_FEEDERENABLED);
  const IntVector* lstCapValues = m_pUI->GetValidCap(CAP_FEEDERENABLED);
  if(lstCapValues && lstCapValues->size()>0)
  {
    ui->pComboBox_Source->addItem(lstCapValues->at(0)!=0?QApplication::translate("MainForm", "ADF", 0, QApplication::UnicodeUTF8):
      QApplication::translate("MainForm", "Flatbed", 0, QApplication::UnicodeUTF8),lstCapValues->at(0)!=0);
    if(lstCapValues->size()>1)
    {
      ui->pComboBox_Source->addItem(lstCapValues->at(1)!=0?QApplication::translate("MainForm", "ADF", 0, QApplication::UnicodeUTF8):
        QApplication::translate("MainForm", "Flatbed", 0, QApplication::UnicodeUTF8),lstCapValues->at(1)!=0);
    }
    ui->pComboBox_Source->setCurrentIndex(nCapIndex);
  }

  ui->pComboBox_ScanSide->clear();
  nCapIndex = m_pUI->GetCurrentCapIndex(CAP_DUPLEXENABLED);
  lstCapValues = m_pUI->GetValidCap(CAP_DUPLEXENABLED);
  if(lstCapValues && lstCapValues->size()>0)
  {
    ui->pComboBox_ScanSide->addItem(lstCapValues->at(0)!=0?QApplication::translate("MainForm", "Duplex", 0, QApplication::UnicodeUTF8):
      QApplication::translate("MainForm", "Simplex", 0, QApplication::UnicodeUTF8),lstCapValues->at(0)!=0);
    if(lstCapValues->size()>1)
    {
      ui->pComboBox_ScanSide->addItem(lstCapValues->at(1)!=0?QApplication::translate("MainForm", "Duplex", 0, QApplication::UnicodeUTF8):
        QApplication::translate("MainForm", "Simplex", 0, QApplication::UnicodeUTF8),lstCapValues->at(1)!=0);
    }
    ui->pComboBox_ScanSide->setCurrentIndex(nCapIndex);
  }  

  ui->pComboBox_ColorMode->clear();
  nCapIndex = m_pUI->GetCurrentCapIndex(ICAP_PIXELTYPE);
  lstCapValues = m_pUI->GetValidCap(ICAP_PIXELTYPE);
  for(unsigned int i=0; i<lstCapValues->size();i++)
  {
    switch(lstCapValues->at(i))
    {
      case TWPT_BW:
        ui->pComboBox_ColorMode->addItem(QApplication::translate("MainForm", "Black and White", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWPT_GRAY:
        ui->pComboBox_ColorMode->addItem(QApplication::translate("MainForm", "Grayscale", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWPT_RGB:
        ui->pComboBox_ColorMode->addItem(QApplication::translate("MainForm", "Color", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      default:
        continue;
    }
    if(i==nCapIndex)
    {
      ui->pComboBox_ColorMode->setCurrentIndex(ui->pComboBox_ColorMode->count()-1);
    }
  }

  if(lstCapValues->at(nCapIndex)==TWPT_BW)
  {
    ui->pHorizontalSlider_Threshold->setEnabled(true);
    ui->pLabel_Threshold->setEnabled(true);
    ui->pHorizontalSlider_Contrast->setEnabled(false);
    ui->pLabel_Contrast->setEnabled(false);
    ui->pHorizontalSlider_Brightness->setEnabled(false);
    ui->pLabel_Brightness->setEnabled(false);
    ui->pHorizontalSlider_Gamma->setEnabled(false);
    ui->pLabel_Gamma->setEnabled(false);
  }
  else
  {
    ui->pHorizontalSlider_Threshold->setEnabled(false);
    ui->pLabel_Threshold->setEnabled(false);
    ui->pHorizontalSlider_Contrast->setEnabled(true);
    ui->pLabel_Contrast->setEnabled(true);
    ui->pHorizontalSlider_Brightness->setEnabled(true);
    ui->pLabel_Brightness->setEnabled(true);
    ui->pHorizontalSlider_Gamma->setEnabled(true);
    ui->pLabel_Gamma->setEnabled(true);
  }

  ui->pComboBox_Resolution->clear();
  nCapIndex = m_pUI->GetCurrentCapIndex(ICAP_XRESOLUTION);
  const FloatVector *lstCapValuesFlt = m_pUI->GetValidCapFloat(ICAP_XRESOLUTION);

  for(unsigned int i=0; i<lstCapValuesFlt->size();i++)
  {
    QString strTemp;
    strTemp.setNum((int)lstCapValuesFlt->at(i));
    ui->pComboBox_Resolution->addItem(strTemp,(int)lstCapValuesFlt->at(i));
    if(i==nCapIndex)
    {
      ui->pComboBox_Resolution->setCurrentIndex(ui->pComboBox_Resolution->count()-1);
    }
  }
  
  nCapIndex = m_pUI->GetCurrentCapIndex(CUSTCAP_LONGDOCUMENT);
  lstCapValues = m_pUI->GetValidCap(CUSTCAP_LONGDOCUMENT);

  ui->pCheckBox_LongDoc->setChecked(lstCapValues->at(nCapIndex)!=0);
  

  ui->pComboBox_Orientation->clear();
  nCapIndex = m_pUI->GetCurrentCapIndex(ICAP_ORIENTATION);
  lstCapValues = m_pUI->GetValidCap(ICAP_ORIENTATION);
  for(unsigned int i=0; i<lstCapValues->size();i++)
  {
    switch(lstCapValues->at(i))
    {
      case TWOR_PORTRAIT:
        ui->pComboBox_Orientation->addItem(QApplication::translate("MainForm", "Portrait", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWOR_LANDSCAPE:
        ui->pComboBox_Orientation->addItem(QApplication::translate("MainForm", "Landscape", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      default:
        continue;
    }
    if(i==nCapIndex)
    {
      ui->pComboBox_Orientation->setCurrentIndex(ui->pComboBox_Orientation->count()-1);
    }
  }

  ui->pComboBox_PaperSize_StandardSizes->clear();
  nCapIndex = m_pUI->GetCurrentCapIndex(ICAP_SUPPORTEDSIZES);
  lstCapValues = m_pUI->GetValidCap(ICAP_SUPPORTEDSIZES);
  for(unsigned int i=0; i<lstCapValues->size();i++)
  {
    switch(lstCapValues->at(i))
    {
      case TWSS_NONE:
        ui->pComboBox_PaperSize_StandardSizes->addItem(QApplication::translate("MainForm", "Custom", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWSS_USLETTER:
        ui->pComboBox_PaperSize_StandardSizes->addItem(QApplication::translate("MainForm", "US Letter (8.5\" x 11\")", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWSS_USLEGAL:
        ui->pComboBox_PaperSize_StandardSizes->addItem(QApplication::translate("MainForm", "US Legal (8.5\" x 14\")", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      default:
        continue;
    }
    if(i==nCapIndex)
    {
      ui->pComboBox_PaperSize_StandardSizes->setCurrentIndex(ui->pComboBox_PaperSize_StandardSizes->count()-1);
    }
  }
  if(lstCapValues->at(nCapIndex)==TWSS_NONE)
  {
    ui->pLineEdit_PaperSize_Custom_Width->setEnabled(true);
    ui->pLineEdit_PaperSize_Custom_Height->setEnabled(true);
    ui->pLabel_PaperSize_Custom_Width->setEnabled(true);
    ui->pLabel_PaperSize_Custom_Height->setEnabled(true);
    ui->pComboBox_Orientation->setEnabled(false);
    ui->pLabel_Orientation->setEnabled(false);
  }
  else
  {
    ui->pLineEdit_PaperSize_Custom_Width->setEnabled(false);
    ui->pLineEdit_PaperSize_Custom_Height->setEnabled(false);
    ui->pLabel_PaperSize_Custom_Width->setEnabled(false);
    ui->pLabel_PaperSize_Custom_Height->setEnabled(false);
    ui->pComboBox_Orientation->setEnabled(true);
    ui->pLabel_Orientation->setEnabled(true);
  }
  
  TW_FRAME frame = m_pUI->GetCurrentFrame();
  QString strTemp;
  strTemp.setNum(FIX32ToFloat(frame.Right));
  ui->pLineEdit_PaperSize_Custom_Width->setText(strTemp);

  strTemp.setNum(FIX32ToFloat(frame.Bottom));
  ui->pLineEdit_PaperSize_Custom_Height->setText(strTemp); 
  float fMin, fMax, fStep, fCur;

  fCur = m_pUI->GetCapValueFloat(ICAP_CONTRAST);
  if(m_pUI->GetCapRangeFloat(ICAP_CONTRAST, fMin, fMax, fStep))
  {
    ui->pHorizontalSlider_Contrast->setMinimum((int)fMin);
    ui->pHorizontalSlider_Contrast->setMaximum((int)fMax);
    ui->pHorizontalSlider_Contrast->setValue((int)fCur);
    ui->pHorizontalSlider_Contrast->setSingleStep((int)fStep);
    ui->pHorizontalSlider_Contrast->setTickInterval((fMax-fMin)/10);
  }

  fCur = m_pUI->GetCapValueFloat(ICAP_BRIGHTNESS);
  if(m_pUI->GetCapRangeFloat(ICAP_BRIGHTNESS, fMin, fMax, fStep))
  {
    ui->pHorizontalSlider_Brightness->setMinimum((int)fMin);
    ui->pHorizontalSlider_Brightness->setMaximum((int)fMax);
    ui->pHorizontalSlider_Brightness->setValue((int)fCur);
    ui->pHorizontalSlider_Brightness->setSingleStep((int)fStep);
    ui->pHorizontalSlider_Brightness->setTickInterval((fMax-fMin)/10);
  }

  fCur = m_pUI->GetCapValueFloat(ICAP_GAMMA);
  fCur *=10;
  if(fCur<1)
  {
    fCur = 1;
  }
  if(fCur>100)
  {
    fCur = 100;
  }
  ui->pHorizontalSlider_Gamma->setMinimum(1);
  ui->pHorizontalSlider_Gamma->setMaximum(100);
  ui->pHorizontalSlider_Gamma->setValue((int)fCur);
  ui->pHorizontalSlider_Gamma->setSingleStep(1);
  ui->pHorizontalSlider_Gamma->setTickInterval(10);

  fCur = m_pUI->GetCapValueFloat(ICAP_THRESHOLD);
  if(m_pUI->GetCapRangeFloat(ICAP_THRESHOLD, fMin, fMax, fStep))
  {
    ui->pHorizontalSlider_Threshold->setMinimum((int)fMin);
    ui->pHorizontalSlider_Threshold->setMaximum((int)fMax);
    ui->pHorizontalSlider_Threshold->setValue((int)fCur);
    ui->pHorizontalSlider_Threshold->setSingleStep((int)fStep);
    ui->pHorizontalSlider_Threshold->setTickInterval((fMax-fMin)/10);
  }

  lstCapValues = m_pUI->GetValidCap(CAP_XFERCOUNT);
  ui->pSpinBox_XferCount->setValue(lstCapValues->at(0));

  lstCapValues = m_pUI->GetValidCap(CUSTCAP_DOCS_IN_ADF);
  ui->pSpinBox_DocsInADF->setValue(lstCapValues->at(0));

  ui->pComboBox_PaperSize_Custom_Units->clear();
  nCapIndex = m_pUI->GetCurrentCapIndex(ICAP_UNITS);
  lstCapValues = m_pUI->GetValidCap(ICAP_UNITS);
  for(unsigned int i=0; i<lstCapValues->size();i++)
  {
    switch(lstCapValues->at(i))
    {
      case TWUN_INCHES:
        ui->pComboBox_PaperSize_Custom_Units->addItem(QApplication::translate("MainForm", "Inches", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWUN_PIXELS:
        ui->pComboBox_PaperSize_Custom_Units->addItem(QApplication::translate("MainForm", "Pixels", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWUN_CENTIMETERS:
        ui->pComboBox_PaperSize_Custom_Units->addItem(QApplication::translate("MainForm", "Centimeters", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWUN_PICAS:
        ui->pComboBox_PaperSize_Custom_Units->addItem(QApplication::translate("MainForm", "Picas", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWUN_POINTS:
        ui->pComboBox_PaperSize_Custom_Units->addItem(QApplication::translate("MainForm", "Points", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      case TWUN_TWIPS:
        ui->pComboBox_PaperSize_Custom_Units->addItem(QApplication::translate("MainForm", "Twips", 0, QApplication::UnicodeUTF8),lstCapValues->at(i));
        break;
      default:
        continue;
    }
    if(i==nCapIndex)
    {
      ui->pComboBox_PaperSize_Custom_Units->setCurrentIndex(ui->pComboBox_PaperSize_Custom_Units->count()-1);
    }
  }
}

