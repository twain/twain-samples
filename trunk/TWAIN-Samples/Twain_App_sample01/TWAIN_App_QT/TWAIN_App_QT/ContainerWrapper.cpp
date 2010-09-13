#include "stdafx.h"
#include "ContainerWrapper.h"
#pragma message("TODO: Fix")
#include "CommonTWAIN.h"
#include "..\..\Src\DSMInterface.h"

CContainerWrapper::CContainerWrapper(TW_HANDLE hContainer, TW_UINT16 twConType, bool bOwn/*=true*/)
  : m_bValid(false)
  , m_bOwn(bOwn)
  , m_hContainer(hContainer)
  , m_twConType(twConType)
  , m_pCurrent(NULL)
  , m_pDefault(NULL)
  , m_pItems(NULL)
  , m_pContainer(NULL)
  , m_pMinimum(NULL)
  , m_pMaximum(NULL)
  , m_pStep(NULL)
{
  m_pContainer = _DSM_LockMemory(m_hContainer);
  if(m_pContainer)
  {
    switch(m_twConType)
    {
      case TWON_ONEVALUE:
        //values, current and default are all the same value
        m_pItems = ConvertContainerAddressByteType(&static_cast<pTW_ONEVALUE>(m_pContainer)->Item, 
          static_cast<pTW_ONEVALUE>(m_pContainer)->ItemType);
        m_pCurrent = ConvertContainerAddressByteType(&static_cast<pTW_ONEVALUE>(m_pContainer)->Item, 
          static_cast<pTW_ONEVALUE>(m_pContainer)->ItemType);
        m_pDefault = ConvertContainerAddressByteType(&static_cast<pTW_ONEVALUE>(m_pContainer)->Item, 
          static_cast<pTW_ONEVALUE>(m_pContainer)->ItemType);
        m_bValid = true;
        break;
      case TWON_RANGE:
        //only current, default, minimum, maximum and step are valid
        m_pCurrent = ConvertContainerAddressByteType(&static_cast<pTW_RANGE>(m_pContainer)->CurrentValue, 
          static_cast<pTW_RANGE>(m_pContainer)->ItemType);
        m_pDefault = ConvertContainerAddressByteType(&static_cast<pTW_RANGE>(m_pContainer)->DefaultValue, 
          static_cast<pTW_RANGE>(m_pContainer)->ItemType);
        m_pMinimum = ConvertContainerAddressByteType(&static_cast<pTW_RANGE>(m_pContainer)->MinValue, 
          static_cast<pTW_RANGE>(m_pContainer)->ItemType);
        m_pMaximum = ConvertContainerAddressByteType(&static_cast<pTW_RANGE>(m_pContainer)->MaxValue, 
          static_cast<pTW_RANGE>(m_pContainer)->ItemType);
        m_pStep = ConvertContainerAddressByteType(&static_cast<pTW_RANGE>(m_pContainer)->StepSize, 
          static_cast<pTW_RANGE>(m_pContainer)->ItemType);
        m_bValid = true;
        break;
      case TWON_ARRAY:
        //only values is valid
        m_pItems = &static_cast<pTW_ARRAY>(m_pContainer)->ItemList;
        m_bValid = true;
        break;
      case TWON_ENUMERATION:
        //only values, current and default are valid
        m_pItems = &static_cast<pTW_ENUMERATION>(m_pContainer)->ItemList;
        m_pCurrent = &static_cast<pTW_ENUMERATION>(m_pContainer)->ItemList[
          static_cast<pTW_ENUMERATION>(m_pContainer)->CurrentIndex * 
            getTWTYsize(static_cast<pTW_ENUMERATION>(m_pContainer)->ItemType)];
        m_pDefault = &static_cast<pTW_ENUMERATION>(m_pContainer)->ItemList[
          static_cast<pTW_ENUMERATION>(m_pContainer)->DefaultIndex * 
            getTWTYsize(static_cast<pTW_ENUMERATION>(m_pContainer)->ItemType)];
        m_bValid = true;
        break;
    }
  }
  return;
}

CContainerWrapper::~CContainerWrapper(void)
{
  //Invalidate the members
  m_bValid = false;
  m_pCurrent = NULL;
  m_pDefault = NULL;
  m_pItems = NULL;
  m_pContainer = NULL;
  m_pMinimum = NULL;
  m_pMaximum = NULL;
  m_pStep = NULL;

  if(m_hContainer)
  {
    //Unlock the handle
    _DSM_UnlockMemory(m_hContainer);
    if(m_bOwn)
    {
      //Free the memory
      _DSM_Free(m_hContainer);
    }
    //Invalidate the container
    m_hContainer = NULL;
  }
  return;
}

TW_UINT16 CContainerWrapper::GetItemType()
{
  TW_UINT16 twItemType = TWON_DONTCARE16;
  if(IsValid())
  {
    twItemType = static_cast<pTW_ONEVALUE>(m_pContainer)->ItemType;
  }
  return twItemType;    
}

TW_UINT32 CContainerWrapper::GetCount()
{
  TW_UINT32 twCount = TWON_DONTCARE32;
  if(IsValid())
  {
    switch(m_twConType)
    {
      case TWON_ONEVALUE:
        twCount = 1;
        break;
      case TWON_RANGE:
        twCount = 0;
        break;
      case TWON_ARRAY:
      case TWON_ENUMERATION:
        twCount = static_cast<pTW_ARRAY>(m_pContainer)->NumItems;
        break;
    }
  }
  return twCount;    
}

void *CContainerWrapper::ConvertContainerAddressByteType(void *pItem, TW_UINT16 twType)
{
  void *pResult;
  switch(twType)
  {
    case TWTY_INT8:
    case TWTY_UINT8:
    case TWTY_INT16:
    case TWTY_UINT16:
    case TWTY_BOOL:
    case TWTY_HANDLE:
#ifdef _SYSTEM_BIGENDIAN
      pResult = &static_cast<BYTE *>(pItem)[sizeof(TW_UINT32)-getTWTYsize(twType)];
#else //#ifdef _SYSTEM_BIGENDIAN
      pResult = pItem;
#endif //#ifdef _SYSTEM_BIGENDIAN
      break;
    default:
      pResult = pItem;
      break;
  }
  return pResult;
}

void *CContainerWrapper::GetItem(TW_UINT32 uiItemIndex)
{
  void *pResult = NULL;
  if(uiItemIndex <= GetCount())
  {
    switch(m_twConType)
    {
      case TWON_ENUMERATION:
      case TWON_ARRAY:
        return &static_cast<unsigned char *>(m_pItems)[getTWTYsize(GetItemType())*uiItemIndex];
        break;
    }
  }
  return pResult;
}

bool CContainerWrapper::ContainsItem(TW_UINT16 twValue)
{
  bool bRC = false;
  if(TWTY_UINT16==GetItemType())
  {
    if(TWON_RANGE!=GetConType())
    {
      for(int nIndex = 0; nIndex < GetCount(); nIndex++)
      {
        if(twValue==static_cast<pTW_UINT16>(m_pItems)[nIndex])
        {
          bRC = true;
          break;
        }
      }
    }
    else
    {
      #pragma message("TODO: test in a range")
    }
  }
  return bRC;
}
