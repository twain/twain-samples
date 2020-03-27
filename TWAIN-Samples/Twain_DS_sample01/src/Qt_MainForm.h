#ifndef QT_MAINFORM_H
#define QT_MAINFORM_H
#include "QT_UI.h"

#include <QtWidgets/QDialog>
#include <QDoubleValidator>

namespace Ui
{
  class MainForm;
}

class MainForm : public QDialog
{
  Q_OBJECT

public:
  MainForm(QT_UI *pUI, QWidget *parent = 0);
  ~MainForm();

protected slots:
  // void on_<object name>_<signal name>(<signal parameters>);

  // Profile buttons
  void on_pPushButton_Profile_Save_clicked(bool _bChecked = false);
  void on_pPushButton_Profile_Load_clicked(bool _bChecked = false);
  void on_pPushButton_Profile_Delete_clicked(bool _bChecked = false);

  // Scan Settings controls
  void on_pComboBox_Source_activated(int _nIndex);
  void on_pComboBox_ScanSide_activated(int _nIndex);
  void on_pComboBox_ColorMode_activated(int _nIndex);
  void on_pComboBox_Resolution_activated(int _nIndex);

  // Scan Area controls
  void on_pComboBox_Orientation_activated(int _nIndex);
  void on_pCheckBox_LongDoc_stateChanged(int _nState);
  void on_pComboBox_PaperSize_StandardSizes_activated(int _nIndex);
  void on_pLineEdit_PaperSize_Custom_Width_editingFinished();
  void on_pLineEdit_PaperSize_Custom_Height_editingFinished();
  void on_pComboBox_PaperSize_Custom_Units_activated(int _nIndex);

  // Sliders
  void on_pHorizontalSlider_Contrast_valueChanged(int value);
  void on_pHorizontalSlider_Brightness_valueChanged(int value);
  void on_pHorizontalSlider_Gamma_valueChanged(int value);
  void on_pHorizontalSlider_Threshold_valueChanged(int value);
  void on_pHorizontalSlider_Contrast_sliderReleased();
  void on_pHorizontalSlider_Brightness_sliderReleased();
  void on_pHorizontalSlider_Gamma_sliderReleased();
  void on_pHorizontalSlider_Threshold_sliderReleased();
  void on_pHorizontalSlider_Contrast_actionTriggered ( int action ) ;
  void on_pHorizontalSlider_Brightness_actionTriggered ( int action ) ;
  void on_pHorizontalSlider_Gamma_actionTriggered ( int action ) ;
  void on_pHorizontalSlider_Threshold_actionTriggered ( int action ) ;

  // Batch control spinboxes
  void on_pSpinBox_DocsInADF_valueChanged(int _nValue);

  // Bottom buttons
  void on_pPushButton_Scan_clicked(bool _bChecked = false);
  void on_pPushButton_Cancel_clicked(bool _bChecked = false);
  void on_pPushButton_Help_clicked(bool _bChecked = false);
  void on_pPushButton_About_clicked(bool _bChecked = false);

private:
  Ui::MainForm *ui;
  QT_UI *m_pUI;
  void UpdateControls(void);
  void closeEvent(QCloseEvent *);
  void hideEvent (QHideEvent *evHide);
  bool m_bSliderValueChanged;
  QDoubleValidator m_valH;
  QDoubleValidator m_valW;

};

#endif // QT_MAINFORM_H
