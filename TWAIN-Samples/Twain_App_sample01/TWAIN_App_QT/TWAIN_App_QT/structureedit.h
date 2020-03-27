/**
* @file structureedit.h
* @brief  Contains the Declaration of StructureEdit dialog
* @author JFL Peripheral Solutions Inc.
* @date September 2010
*/
#ifndef _inc_structureedit_h
#define _inc_structureedit_h

#include <QtWidgets/QDialog>
#include "ui_structureedit.h"
#include "twain.h"
#include "twainstring.h"

/**
* Dialog designed to allow users to modify structure contents
*/
class StructureEdit : public QDialog
{
  Q_OBJECT

  public:
  /**
  * Basic contructor initializes the GUI
  */
  StructureEdit(QWidget *parent = 0);
  /**
  * Basic destructor destroys the object
  */
  ~StructureEdit();
  /**
  * Method for loading the dialog with a TWAIN defined structure
  * @param[in] pStructure a pointer to the stucture
  * @param[in] twData the correspondiung DAT_ that describes the structure
  */
  void SetTWAINStructure(void *pStructure, TW_UINT16 twDat);
  /**
  * Applies the users changes to the original structure
  */
  virtual void accept();
  /**
  * Discards the users changes and preserves the original structure
  */
  virtual void reject();

  public slots:
  /**
  * Handles updating structure members when an item changes
  * @param[in] pItem the item that changed
  */
  void OnItemChanged(QStandardItem *pItem);

  protected:
  void *m_pStructure; /**< pointer to the original structure */
  size_t m_nStructureSize; /**< size of the stucture */

  /**
  * Method for inserting a field into the 
  */
  void InsertField(QStandardItem *pItem, const QString &strField, const QString &strValue, void *pValue, TW_UINT16 twItemType, const TW_UINT16 *pEnums=NULL, pfnStringCvrtFuncType pfnStringCvrtFunc=NULL);

  private:
  Ui::StructureEditClass ui;
  QStandardItemModel m_StructureEditModel;
};

#endif //#ifndef _inc_structureedit_h
