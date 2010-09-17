#include "stdafx.h"
#include "structureedit.h"
#include "twainstring.h"

class ComboDelegate : public QItemDelegate
{
  public:
  ComboDelegate(QObject *pParent=0)
    : QItemDelegate(pParent)
  {
  };
  
  ~ComboDelegate()
  {
  };

  virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
  virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

QWidget *ComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  const TW_UINT16 *pTable = static_cast<const TW_UINT16 *>(index.model()->data(index, Qt::UserRole + 3).value<void*>());
  if(pTable)
  {
    pfnStringCvrtFuncType pfnCvrt = static_cast<pfnStringCvrtFuncType>(index.model()->data(index, Qt::UserRole + 4).value<void*>());
    QComboBox *pCombo = new QComboBox(parent);
    int nIndex = 0;
    while(TWON_DONTCARE16!=pTable[nIndex])
    {
      pCombo->addItem(pfnCvrt(pTable[nIndex]), pTable[nIndex]);
      if(index.model()->data(index, Qt::DisplayRole).toString()==QString(pfnCvrt(pTable[nIndex])))
      {
        pCombo->setCurrentIndex(nIndex);
      }
      nIndex++;
    }
    return pCombo;
  }
  return QItemDelegate::createEditor(parent, option, index);
}
void ComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
//  const TW_UINT16 *pTable = static_cast<const TW_UINT16 *>(index.model()->data(index, Qt::UserRole + 3).value<void*>());
  QComboBox *pCombo = dynamic_cast<QComboBox*>(editor);
  if(pCombo)
  {
    //should have already been done during init
    return;
  }
  QItemDelegate::setEditorData(editor, index);
  return;
}

void ComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *pCombo = dynamic_cast<QComboBox*>(editor);
  if(pCombo)
  {
    pfnStringCvrtFuncType pfnCvrt = static_cast<pfnStringCvrtFuncType>(model->data(index, Qt::UserRole + 4).value<void*>());
//    model->setData(index, pfnCvrt(pCombo->itemData(pCombo->currentIndex()).toUInt()), Qt::EditRole);
    QStandardItemModel *pItemModel = dynamic_cast<QStandardItemModel *>(model);
    QStandardItem *pItem = pItemModel->itemFromIndex(index);
    pItem->setText(pfnCvrt(pCombo->itemData(pCombo->currentIndex()).toUInt()));
    void *pValue = model->data(index, Qt::UserRole + 1).value<void*>();
    TW_UINT16 twItemType = model->data(index, Qt::UserRole + 2).toUInt();
    switch(twItemType)
    {
      case TWTY_INT8:
        *static_cast<pTW_INT8>(pValue) = pCombo->itemData(pCombo->currentIndex()).toInt();
        break;
      case TWTY_UINT8:
        *static_cast<pTW_UINT8>(pValue) = pCombo->itemData(pCombo->currentIndex()).toUInt();
        break;
      case TWTY_UINT16:
      case TWTY_BOOL:
        *static_cast<pTW_UINT16>(pValue) = pCombo->itemData(pCombo->currentIndex()).toUInt();
        break;
      case TWTY_INT16:
        *static_cast<pTW_INT16>(pValue) = pCombo->itemData(pCombo->currentIndex()).toInt();
        break;
      case TWTY_INT32:
        *static_cast<pTW_INT32>(pValue) = pCombo->itemData(pCombo->currentIndex()).toInt();
        break;
      case TWTY_UINT32:
        *static_cast<pTW_UINT32>(pValue) = pCombo->itemData(pCombo->currentIndex()).toUInt();
        break;
      //the rest should never happen
      case TWTY_FIX32:
      case TWTY_FRAME:
      case TWTY_STR32:
      case TWTY_STR64:
      case TWTY_STR128:
      case TWTY_STR255:
      case TWTY_HANDLE:
        break;
    }
    return;
  }
  QItemDelegate::setModelData(editor, model, index);
  return;
}


StructureEdit::StructureEdit(QWidget *parent)
    : QDialog(parent)
{
  ui.setupUi(this);
  ui.treStructure->setModel(&m_StructureEditModel);
  ui.treStructure->setItemDelegate(new ComboDelegate);
  //
  connect( &m_StructureEditModel, SIGNAL( itemChanged(QStandardItem *) ), this, SLOT( OnItemChanged(QStandardItem *) ), Qt::AutoConnection);
  return;
}

StructureEdit::~StructureEdit()
{
  return;
}

const TW_UINT16 kCAMERATYPE_TABLE[] =
{
  TWFY_CAMERA,
  TWFY_CAMERATOP,
  TWFY_CAMERABOTTOM,
  TWFY_CAMERAPREVIEW,
  TWFY_DOMAIN,
  TWFY_HOST,
  TWFY_DIRECTORY,
  TWFY_IMAGE,
  TWFY_UNKNOWN,
  TWON_DONTCARE16,
};

const TW_UINT16 kFILEFORMAT_TABLE[] =
{
  TWFF_TIFF,
  TWFF_PICT,
  TWFF_BMP,
  TWFF_XBM,
  TWFF_JFIF,
  TWFF_FPX,
  TWFF_TIFFMULTI,
  TWFF_PNG,
  TWFF_SPIFF,
  TWFF_EXIF,
  TWFF_PDF,
  TWFF_JP2,
  TWFF_JPN,
  TWFF_JPX,
  TWFF_DEJAVU,
  TWFF_PDFA,
  TWFF_PDFA2,
  TWON_DONTCARE16,
};

void StructureEdit::SetTWAINStructure(void *pStructure, TW_UINT16 twDat)
{
  setWindowTitle(convertDataArgType_toString(twDat));
  //empty the view
  m_StructureEditModel.clear();
  m_StructureEditModel.setColumnCount(2);

  m_pStructure = pStructure;

  QStringList lstHeader;
  lstHeader << "Field" << "Value";
  m_StructureEditModel.setHorizontalHeaderLabels(lstHeader);
  ui.treStructure->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui.treStructure->header()->setResizeMode(1, QHeaderView::ResizeToContents);

  
  QStandardItem *pItem = new QStandardItem(convertDataArgType_toString(twDat));
  pItem->setFlags(Qt::ItemIsEnabled);
  QStandardItem *pItem2 = new QStandardItem("");
  pItem2->setFlags(Qt::ItemIsEnabled);

  switch(twDat)
  {
    case DAT_FILESYSTEM:
      {
        m_nStructureSize = sizeof(TW_FILESYSTEM);
        pTW_FILESYSTEM pStruct = static_cast<pTW_FILESYSTEM>(malloc(m_nStructureSize));
        if(pStruct)
        {
          memcpy(pStruct, pStructure, m_nStructureSize);
          pItem->setData(QVariant::fromValue(static_cast<void*>(pStruct)));
          pItem->setText("TW_FILESYSTEM");
          InsertField(pItem, "InputName", QString("%1").arg(pStruct->InputName), &pStruct->InputName, TWTY_STR255);
          InsertField(pItem, "OutputName", QString("%1").arg(pStruct->OutputName), &pStruct->OutputName, TWTY_STR255);
//          InsertField("Context", QString("%1").arg(pStruct->Context), &pStruct->Context, TWTY_MEMREF);
          /* DG_CONTROL / DAT_FILESYSTEM / MSG_DELETE field    */
          InsertField(pItem, "Recursive", QString("%1").arg(pStruct->Recursive), &pStruct->Recursive, TWTY_INT32);
          /* DG_CONTROL / DAT_FILESYSTEM / MSG_GETINFO fields  */
          InsertField(pItem, "FileType", QString("%1").arg(convertFileType_toString(pStruct->FileType)), &pStruct->FileType, TWTY_INT32, kCAMERATYPE_TABLE, convertFileType_toString);
          InsertField(pItem, "Size", QString("%1").arg(pStruct->Size), &pStruct->Size, TWTY_UINT32);
          InsertField(pItem, "CreateTimeDate", QString("%1").arg(pStruct->CreateTimeDate), &pStruct->CreateTimeDate, TWTY_STR32 );
          InsertField(pItem, "ModifiedTimeDate", QString("%1").arg(pStruct->ModifiedTimeDate), &pStruct->ModifiedTimeDate, TWTY_STR32 );
          InsertField(pItem, "FreeSpace", QString("%1").arg(pStruct->FreeSpace), &pStruct->FreeSpace, TWTY_UINT32);
          InsertField(pItem, "NewImageSize", QString("%1").arg(pStruct->NewImageSize), &pStruct->NewImageSize, TWTY_INT32 );
          InsertField(pItem, "NumberOfFiles", QString("%1").arg(pStruct->NumberOfFiles), &pStruct->NumberOfFiles, TWTY_UINT32);
          InsertField(pItem, "NumberOfSnippets", QString("%1").arg(pStruct->NumberOfSnippets), &pStruct->NumberOfSnippets, TWTY_UINT32);
          InsertField(pItem, "DeviceGroupMask", QString("%1").arg(pStruct->DeviceGroupMask), &pStruct->DeviceGroupMask, TWTY_UINT32);
//          InsertField("Reserved", QString("%1").arg(pStruct->Reserved), &pStruct->Reserved, chTYar     )[
        }
      }
      break;
    case DAT_SETUPFILEXFER:
      {
        m_nStructureSize = sizeof(TW_SETUPFILEXFER);
        pTW_SETUPFILEXFER pStruct = static_cast<pTW_SETUPFILEXFER>(malloc(m_nStructureSize));
        if(pStruct)
        {
          memcpy(pStruct, pStructure, m_nStructureSize);
          pItem->setData(QVariant::fromValue(static_cast<void*>(pStruct)));
          pItem->setText("TW_SETUPFILEXFER");
          InsertField(pItem, "FileName", QString("%1").arg(pStruct->FileName), &pStruct->FileName, TWTY_STR255);
          InsertField(pItem, "Format", QString("%1").arg(convertFileFormat_toString(pStruct->Format)), &pStruct->Format, TWTY_UINT16, kFILEFORMAT_TABLE, convertFileFormat_toString);
          InsertField(pItem, "VRefNum", QString("%1").arg(pStruct->VRefNum), &pStruct->VRefNum, TWTY_INT16);
        }
      }
      break;
    case DAT_IMAGELAYOUT:
      {
        m_nStructureSize = sizeof(TW_IMAGELAYOUT);
        pTW_IMAGELAYOUT pStruct = static_cast<pTW_IMAGELAYOUT>(malloc(m_nStructureSize));
        if(pStruct)
        {
          memcpy(pStruct, pStructure, m_nStructureSize);
          pItem->setData(QVariant::fromValue(static_cast<void*>(pStruct)));
          pItem->setText("TW_IMAGELAYOUT");
          InsertField(pItem, "Frame.Left", QString("%1").arg(FIX32ToFloat(pStruct->Frame.Left)), &pStruct->Frame.Left, TWTY_FIX32 );
          InsertField(pItem, "Frame.Top", QString("%1").arg(FIX32ToFloat(pStruct->Frame.Top)), &pStruct->Frame.Top, TWTY_FIX32 );
          InsertField(pItem, "Frame.Right", QString("%1").arg(FIX32ToFloat(pStruct->Frame.Right)), &pStruct->Frame.Right, TWTY_FIX32 );
          InsertField(pItem, "Frame.Bottom", QString("%1").arg(FIX32ToFloat(pStruct->Frame.Bottom)), &pStruct->Frame.Bottom, TWTY_FIX32 );
          InsertField(pItem, "DocumentNumber", QString("%1").arg(pStruct->DocumentNumber), &pStruct->DocumentNumber, TWTY_UINT32);
          InsertField(pItem, "PageNumber", QString("%1").arg(pStruct->PageNumber), &pStruct->PageNumber, TWTY_UINT32);
          InsertField(pItem, "FrameNumber", QString("%1").arg(pStruct->FrameNumber), &pStruct->FrameNumber, TWTY_UINT32);
        }
      }
      break;
  }

  QList<QStandardItem *> list;
  list << pItem << pItem2;
  m_StructureEditModel.appendRow(list);

  ui.treStructure->expandAll();
  return;
}

void StructureEdit::InsertField(QStandardItem *pItem, const QString &strField, const QString &strValue, void *pValue, TW_UINT16 twItemType, const TW_UINT16 *pEnums/*=NULL*/, pfnStringCvrtFuncType pfnStringCvrtFunc/*=NULL*/)
{
  QStandardItem *pFieldItem = new QStandardItem(strField);
  pFieldItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

  QStandardItem *pValueItem = new QStandardItem(strValue);
  pValueItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable);
  pValueItem->setData(QVariant::fromValue(pValue), Qt::UserRole + 1);
  pValueItem->setData(twItemType, Qt::UserRole + 2);
  
  if(pEnums && pfnStringCvrtFunc)
  {
    pValueItem->setData(QVariant::fromValue((void*)pEnums), Qt::UserRole + 3);
    pValueItem->setData(QVariant::fromValue((void*)pfnStringCvrtFunc), Qt::UserRole + 4);
  }


  QList<QStandardItem *> list;
  list << pFieldItem << pValueItem;
  pItem->appendRow(list);
  return;
}

void StructureEdit::accept()
{
  QStandardItem *pItem = m_StructureEditModel.item(0, 0);
  if(pItem)
  {
    void *pStruct = pItem->data().value<void*>();
    if(pStruct)
    {
      memcpy(m_pStructure, pStruct, m_nStructureSize);
      free(pStruct);
    }
  }
  m_pStructure = NULL;
  QDialog::accept();
  return;
}

void StructureEdit::reject()
{
  QStandardItem *pItem = m_StructureEditModel.item(0, 0);
  if(pItem)
  {
    void *pStruct = pItem->data().value<void*>();
    if(pStruct)
    {
      free(pStruct);
    }
  }
  m_pStructure = NULL;
  QDialog::reject();
  return;
}

void StructureEdit::OnItemChanged(QStandardItem *pItem)
{
  void *pValue = pItem->data().value<void*>();
  void *pfnCvrt = pItem->data(Qt::UserRole+4).value<void*>();
  TW_UINT16 twItemType = pItem->data(Qt::UserRole + 2).toUInt();
  if(NULL==pfnCvrt)
  {
    switch(twItemType)
    {
      case TWTY_INT8:
        *static_cast<pTW_INT8>(pValue) = pItem->text().toInt();
        pItem->setText(QString("%1").arg(*static_cast<pTW_INT8>(pValue)));
        break;
      case TWTY_UINT8:
        *static_cast<pTW_UINT8>(pValue) = pItem->text().toUInt();
        pItem->setText(QString("%1").arg(*static_cast<pTW_UINT8>(pValue)));
        break;
      case TWTY_UINT16:
      case TWTY_BOOL:
        *static_cast<pTW_UINT16>(pValue) = pItem->text().toUInt();
        pItem->setText(QString("%1").arg(*static_cast<pTW_UINT16>(pValue)));
        break;
      case TWTY_INT16:
        *static_cast<pTW_INT16>(pValue) = pItem->text().toInt();
        pItem->setText(QString("%1").arg(*static_cast<pTW_INT16>(pValue)));
        break;
      case TWTY_INT32:
        *static_cast<pTW_INT32>(pValue) = pItem->text().toInt();
        pItem->setText(QString("%1").arg(*static_cast<pTW_INT32>(pValue)));
        break;
      case TWTY_UINT32:
        *static_cast<pTW_UINT32>(pValue) = pItem->text().toUInt();
        pItem->setText(QString("%1").arg(*static_cast<pTW_UINT32>(pValue)));
        break;
      case TWTY_FIX32:
        *static_cast<pTW_FIX32>(pValue) = FloatToFIX32(pItem->text().toFloat());
        pItem->setText(QString("%1").arg(FIX32ToFloat(*static_cast<pTW_FIX32>(pValue))));
        break;
      case TWTY_FRAME:
        //should never happen - break this up into 4 rows of TW_FIX32 values
        break;
      case TWTY_STR32:
        memset(static_cast<char *>(pValue), 0, sizeof(TW_STR32));
        SSTRCPY(static_cast<char *>(pValue), 32, pItem->text().toAscii());
        pItem->setText(static_cast<char *>(pValue));
        break;
      case TWTY_STR64:
        memset(static_cast<char *>(pValue), 0, sizeof(TW_STR64));
        SSTRCPY(static_cast<char *>(pValue), 64, pItem->text().toAscii());
        pItem->setText(static_cast<char *>(pValue));
        break;
      case TWTY_STR128:
        memset(static_cast<char *>(pValue), 0, sizeof(TW_STR128));
        SSTRCPY(static_cast<char *>(pValue), 128, pItem->text().toAscii());
        pItem->setText(static_cast<char *>(pValue));
        break;
      case TWTY_STR255:
        memset(static_cast<char *>(pValue), 0, sizeof(TW_STR255));
        SSTRCPY(static_cast<char *>(pValue), 255, pItem->text().toAscii());
        pItem->setText(static_cast<char *>(pValue));
        break;
      case TWTY_HANDLE:
        break;
    }
  }
  return;
}