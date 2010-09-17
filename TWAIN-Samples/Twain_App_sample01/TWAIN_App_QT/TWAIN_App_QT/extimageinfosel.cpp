#include "stdafx.h"
#include "extimageinfosel.h"
#include "twainstring.h"

bool Find(vector<TW_UINT16>lst, const TW_UINT16 twItem)
{
  for(vector<TW_UINT16>::iterator iter = lst.begin(); iter != lst.end(); iter++)
  {
    if(twItem == *iter)
    {
      return true;
    }
  }
  return false;
}

ExtImageInfoSel::ExtImageInfoSel(vector<TW_UINT16> &lstExtImageInfo, QWidget *parent/*=0*/)
    : QDialog(parent)
    , m_lstExtImageInfo(lstExtImageInfo)
{
  ui.setupUi(this);
  ui.treExtImgInfo->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui.treExtImgInfo->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  
  int nIndex = 0;
  while(TWON_DONTCARE16 != kTWEI_TABLE[nIndex])
  {
    QTreeWidgetItem *pItem = new QTreeWidgetItem();
    pItem->setText(0, convertExtImageInfoName_toString(kTWEI_TABLE[nIndex]));
    pItem->setCheckState(1, Find(m_lstExtImageInfo, kTWEI_TABLE[nIndex])?Qt::Checked:Qt::Unchecked );
    pItem->setData(0, Qt::UserRole, kTWEI_TABLE[nIndex]);
    ui.treExtImgInfo->addTopLevelItem(pItem);
    nIndex++;
  }

  return;
}

ExtImageInfoSel::~ExtImageInfoSel()
{
  return;
}

void ExtImageInfoSel::accept()
{
  m_lstExtImageInfo.clear();
  for(int nIndex = 0; nIndex < ui.treExtImgInfo->topLevelItemCount(); nIndex++)
  {
    QTreeWidgetItem *pItem = ui.treExtImgInfo->topLevelItem(nIndex);
    if(Qt::Checked==pItem->checkState(1))
    {
      m_lstExtImageInfo.push_back(pItem->data(0, Qt::UserRole).toUInt());
    }
  }
  QDialog::accept();
  return;
}

void ExtImageInfoSel::on_btnAll_clicked(bool bChecked)
{
  for(int nIndex = 0; nIndex < ui.treExtImgInfo->topLevelItemCount(); nIndex++)
  {
    QTreeWidgetItem *pItem = ui.treExtImgInfo->topLevelItem(nIndex);
    pItem->setCheckState(1, Qt::Checked);
  }
  return;
}

void ExtImageInfoSel::on_btnNone_clicked(bool bChecked)
{
  for(int nIndex = 0; nIndex < ui.treExtImgInfo->topLevelItemCount(); nIndex++)
  {
    QTreeWidgetItem *pItem = ui.treExtImgInfo->topLevelItem(nIndex);
    pItem->setCheckState(1, Qt::Unchecked);
  }
  return;
}

