/***************************************************************************
* Copyright © 2007 TWAIN Working Group:  
*   Adobe Systems Incorporated, AnyDoc Software Inc., Eastman Kodak Company, 
*   Fujitsu Computer Products of America, JFL Peripheral Solutions Inc., 
*   Ricoh Corporation, and Xerox Corporation.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the TWAIN Working Group nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY TWAIN Working Group ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL TWAIN Working Group BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************/

/**
* @file CTWAINDS_Base.cpp
* Base class describing a TWAIN Data Source.
* @author TWAIN Working Group
* @date April 2007
*/

#include "CTWAINDS_Base.h"

#if (TWNDS_CMP == TWNDS_CMP_VISUALCPP)
  #include <io.h>
#endif

#ifdef TWNDS_OS_APPLE
  #import <QuickTime/QuickTime.h>		// for GraphicsImporter
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <sstream>

//////////////////////////////////////////////////////////////////////////////
CTWAINDS_Base::CTWAINDS_Base() :
  m_DSConditionCode(TWCC_SUCCESS)
{
  memset(&m_MyIdentity, 0, sizeof(m_MyIdentity));
  memset(&m_App, 0, sizeof(m_App));
  m_CurrentState = dsState_Loaded;
  memset(&m_CurFileExferName, 0, sizeof(m_CurFileExferName));
  memset(&m_DefFileExferName, 0, sizeof(m_DefFileExferName));
  memset(&m_Xfers, 0, sizeof(m_Xfers));
  m_nDestScanLine = 0;
  m_hImageData = 0;
  m_DocumentNumber = 0;
  m_PageNumber = 0;
  return;
}

//////////////////////////////////////////////////////////////////////////////
CTWAINDS_Base::~CTWAINDS_Base()
{
  memset(&m_MyIdentity, 0, sizeof(TW_IDENTITY));
  memset(&m_App, 0, sizeof(TW_IDENTITY));

  // free all resources belonging to m_IndependantCapMap
  TWAINCapabilitiesMap::iterator cur = m_IndependantCapMap.begin();

  while(cur != m_IndependantCapMap.end())
  {
    delete cur->second;
    cur++;
  }

  if(m_hImageData)
  {
    _DSM_Free(m_hImageData);
    m_hImageData = 0;
  }
}

//////////////////////////////////////////////////////////////////////////////
void CTWAINDS_Base::fillIdentityStructure(TW_IDENTITY& _idStruct)
{
  // fill in the Identity structure
  SSTRNCPY(_idStruct.Manufacturer, sizeof(_idStruct.Manufacturer), "Unknown", 32);
  SSTRNCPY(_idStruct.ProductFamily, sizeof(_idStruct.ProductFamily), "Unknown", 32);
  SSTRNCPY(_idStruct.ProductName, sizeof(_idStruct.ProductName), "Unknown", 32);
}


//////////////////////////////////////////////////////////////////////////////
TW_UINT16 CTWAINDS_Base::DS_Entry( pTW_IDENTITY _pOrigin,
                                   TW_UINT32    _DG,
                                   TW_UINT16    _DAT,
                                   TW_UINT16    _MSG,
                                   TW_MEMREF    _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch (_DG)
  {
    case DG_CONTROL:
      {
        switch (_DAT)
        {
          case DAT_EVENT:
            twrc = dat_event(_MSG, (pTW_EVENT) _pData);
            break;

          case DAT_IDENTITY:
            twrc = dat_identity(_pOrigin, _MSG, (pTW_IDENTITY) _pData);
            break;

          case DAT_CAPABILITY:
            twrc = dat_capability(_MSG, (pTW_CAPABILITY) _pData);
            break;

          case DAT_USERINTERFACE:
            twrc = dat_userinterface(_MSG, (pTW_USERINTERFACE) _pData);
            break;

          case DAT_SETUPFILEXFER:
            twrc = dat_setupfilexfer(_MSG, (pTW_SETUPFILEXFER) _pData);
            break;

          case DAT_SETUPMEMXFER:
            twrc = dat_setupmemxfer(_MSG, (pTW_SETUPMEMXFER) _pData);
            break;

          case DAT_STATUS:
            twrc = dat_status(_MSG, (pTW_STATUS) _pData);
            break;

          case DAT_PENDINGXFERS:
            twrc = dat_pendingxfers(_MSG, (pTW_PENDINGXFERS)_pData);
            break;

          case DAT_ENTRYPOINT:
            twrc = dat_entrypoint(_MSG, (pTW_ENTRYPOINT)_pData);
            break;

          case DAT_XFERGROUP:
            twrc = dat_xfergroup(_MSG, (pTW_UINT32)_pData);
            break;
            
          case DAT_CUSTOMDSDATA:
            twrc = dat_customdsdata(_MSG, (pTW_CUSTOMDSDATA)_pData);
            break;
            
          default:
            setConditionCode(TWCC_BADPROTOCOL);
            //assert(0);
            twrc = TWRC_FAILURE;
            break;
        }
      }
      break;

    case DG_IMAGE:
      {
        switch (_DAT)
        {
          case DAT_IMAGEINFO:
            twrc = dat_imageinfo(_MSG, (pTW_IMAGEINFO)_pData);
          break;

          case DAT_IMAGENATIVEXFER:
			if(0 == _pData)
			{
			  setConditionCode(TWCC_BADVALUE);
			  return TWRC_FAILURE;
			}
			twrc = dat_imagenativexfer(_MSG, *((TW_HANDLE*)_pData));
          break;

          case DAT_IMAGEFILEXFER:
            twrc = dat_imagefilexfer(_MSG);
          break;

          case DAT_IMAGEMEMXFER:
            twrc = dat_imagememxfer(_MSG, (pTW_IMAGEMEMXFER)_pData);
          break;

          case DAT_IMAGELAYOUT:
            twrc = dat_imagelayout(_MSG, (pTW_IMAGELAYOUT)_pData);
          break;

          case DAT_EXTIMAGEINFO:
            twrc = dat_extimageinfo(_MSG, (pTW_EXTIMAGEINFO)_pData);
          break;

          default:
            setConditionCode(TWCC_CAPUNSUPPORTED);
            //assert(0);
            twrc = TWRC_FAILURE;
          break;
        }
      }
      break;

    default:
      setConditionCode(TWCC_CAPUNSUPPORTED);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_event(TW_UINT16    _MSG,
                                  pTW_EVENT    _pEvent)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_PROCESSEVENT:
      twrc = processEvent(_pEvent);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_identity(pTW_IDENTITY _pOrigin,
                                     TW_UINT16    _MSG,
                                     pTW_IDENTITY _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      memcpy(_pData, &m_MyIdentity, sizeof(TW_IDENTITY));
      break;

    case MSG_OPENDS:
      // store the id assigned to us by the DSM.
      m_MyIdentity.Id = _pData->Id;
      twrc = openDS(_pOrigin);
      break;

    case MSG_CLOSEDS:
      twrc = closeDS();
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_capability(TW_UINT16      _MSG,
                                       pTW_CAPABILITY _pCap)
{
  TW_INT16 twrc = TWRC_FAILURE;

  if(MSG_RESETALL == _MSG) // special case
  {
    if(m_CurrentState >= dsState_XferReady)
    {
      setConditionCode(TWCC_SEQERROR);
      return TWRC_FAILURE;
    }
    // Loop through all supported caps and reset them.
    CTWAINContainer      *pCap     = NULL;
    CTWAINContainerInt   *pCapSC   = dynamic_cast<CTWAINContainerInt*>(findCapability(CAP_SUPPORTEDCAPS));
    if(!pCapSC)
    {
      setConditionCode(TWCC_BADCAP);
      return TWRC_FAILURE;
    }
    const IntVector listInts = pCapSC->GetSupported();

    twrc = TWRC_SUCCESS;
    const int nSize = (int)(listInts.size());
    int x;
    for(x = 0; x < nSize; ++x)
    {
      int Cap = listInts[x];
      if(NULL != (pCap = findCapability(Cap)))
      {
        if(pCap->isOperationAllowed(MSG_RESET))
        {
          if(!pCap->Reset())
          {
            setConditionCode(TWCC_CAPUNSUPPORTED);
            twrc = TWRC_FAILURE;
          }
        }
      }
    }
  }
  else
  {
    // check for invalid args
    if(0 == _pCap)
    {
      setConditionCode(TWCC_BADVALUE);
      return TWRC_FAILURE;
    }

    // first find the capability
    CTWAINContainer* pCap = findCapability(_pCap->Cap);

    // Now handle the cap if one was found
    if(0 != pCap)
    {
      twrc = handleCap(_MSG, pCap, _pCap);

      // when some capabilities are successfully changed with Set or Reset 
      // it requires changing others
      if( (twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS) && 
          (MSG_SET == _MSG || MSG_RESET == _MSG) )
      {
        TW_INT16 twrc2 = updatePostDependencies(_MSG, _pCap->Cap);
        if(twrc == TWRC_SUCCESS && twrc2 != TWRC_SUCCESS)
        {
          twrc = twrc2;
        }
      }
    }
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
CTWAINContainer* CTWAINDS_Base::findCapability(const TW_UINT16 _unCap)
{
  CTWAINContainer* pCap = 0;

  TWAINCapabilitiesMap::iterator itCap = m_IndependantCapMap.find(_unCap);

  if(itCap == m_IndependantCapMap.end())
  {
    setConditionCode(TWCC_CAPUNSUPPORTED);
  }
  else
  {
    pCap = itCap->second;
  }

  return pCap;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_userinterface(TW_UINT16         _MSG,
                                          pTW_USERINTERFACE _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;


  switch(_MSG)
  {
    case MSG_ENABLEDS:
      twrc = enableDS(_pData);
      break;

    case MSG_ENABLEDSUIONLY:
      twrc = enableDSOnly(_pData);
      break;

    case MSG_DISABLEDS:
      twrc = disableDS(_pData);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_setupfilexfer( TW_UINT16         _MSG,
                                           pTW_SETUPFILEXFER _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      twrc = getFileXfer(_pData);
      break;

    case MSG_GETDEFAULT:
      twrc = getDefaultFileXfer(_pData);
      break;

    case MSG_RESET:
      twrc = resetFileXfer(_pData);
      break;

    case MSG_SET:
      twrc = setFileXfer(_pData);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_setupmemxfer(TW_UINT16        _MSG,
                                         pTW_SETUPMEMXFER _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      twrc = getMemoryXfer(_pData);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_imagenativexfer(TW_UINT16    _MSG,
                                            TW_HANDLE&    _hData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      twrc = transferNativeImage(_hData);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_imagefilexfer(TW_UINT16    _MSG)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      twrc = saveImageFile();
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_customdsdata(TW_UINT16 _MSG, pTW_CUSTOMDSDATA  _pDSData)
{
  TW_INT16 twrc = TWRC_SUCCESS;
  if(m_CurrentState != dsState_Open)
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  switch(_MSG)
  {
    case MSG_GET:
      twrc = GetGustomDSData(_pDSData);
      break;

    case MSG_SET:
      twrc = SetGustomDSData(_pDSData);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_imagememxfer(TW_UINT16         _MSG,
                                         pTW_IMAGEMEMXFER  _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      twrc = transferMemoryBuffers(_pData);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_imagelayout(TW_UINT16, 
                                        pTW_IMAGELAYOUT)
{
  // This should be implemented in the derived class.
  setConditionCode(TWCC_CAPUNSUPPORTED);
  return TWRC_FAILURE;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_extimageinfo(TW_UINT16,
                                         pTW_EXTIMAGEINFO)
{
  // This should be implemented in the derived class.
  setConditionCode(TWCC_CAPUNSUPPORTED);
  return TWRC_FAILURE;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getConditionCode() const
{
  return m_DSConditionCode;
}

//////////////////////////////////////////////////////////////////////////////
void CTWAINDS_Base::setConditionCode(TW_INT16 _cc)
{
  m_DSConditionCode = _cc;
}

//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY CTWAINDS_Base::getIdentity()
{
  return &m_MyIdentity;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_imageinfo(TW_UINT16     _MSG,
                                      pTW_IMAGEINFO _pImageInfo)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      twrc = getImageInfo(_pImageInfo);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_status(TW_UINT16     _MSG,
                                   pTW_STATUS    _pStatus)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
      _pStatus->ConditionCode = getConditionCode();
      setConditionCode(TWCC_SUCCESS);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_pendingxfers(TW_UINT16        _MSG,
                                         pTW_PENDINGXFERS _pXfers)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_ENDXFER:
      twrc = endXfer(_pXfers);
      break;

    case MSG_GET:
      twrc = getXfer(_pXfers);
      break;

    case MSG_RESET:
      twrc = resetXfer(_pXfers);
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_entrypoint(TW_UINT16        _MSG,
                                         pTW_ENTRYPOINT _pEntryPoints)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_SET:
      setEntryPoints(_pEntryPoints);
      twrc = TWRC_SUCCESS;
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::dat_xfergroup(TW_UINT16   _MSG,
                                      pTW_UINT32  _pXferGroup)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  if(!_pXferGroup)
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  switch(_MSG)
  {
    case MSG_GET:
      *_pXferGroup = DG_IMAGE;
      twrc = TWRC_SUCCESS;
      break;

    case MSG_SET:
      if(*_pXferGroup != DG_IMAGE)
      {
        setConditionCode(TWCC_BADVALUE);
        return TWRC_FAILURE;
      }
      twrc = TWRC_SUCCESS;
      break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      //assert(0);
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_Base::DoXferReadyEvent()
{
  bool bRC = false;

  if( dsState_Enabled != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return bRC;
  }

  /// @TODO check to see if we are simplex or duplex.  
  /// Document only updates with new sheet of paper.
  m_DocumentNumber++;
  m_PageNumber++;
  m_CurrentState = dsState_XferReady;
  getImageInfo(&m_ImageInfo);

#ifdef TWNDS_OS_APPLE
  TW_CALLBACK callback = {};
  callback.Message = MSG_XFERREADY;
		
  bRC = TWRC_SUCCESS==_DSM_Entry(getIdentity(),
						   (pTW_IDENTITY)NULL,
						   (TW_UINT32)DG_CONTROL,
						   (TW_UINT16)DAT_CALLBACK,
						   (TW_UINT16)MSG_INVOKE_CALLBACK,
						   (TW_MEMREF) &callback);
#else
  // Tell the DSM that we are ready to send images
  if(TWRC_SUCCESS==_DSM_Entry(getIdentity(),
                              getApp(),
                              DG_CONTROL,
                              DAT_NULL,
                              MSG_XFERREADY,
                              0))
  {
    bRC = true;
  }
#endif	
  return bRC;
}

//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_Base::DoCloseDSRequestEvent()
{
  bool bRC = false;

  if( dsState_Enabled != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return bRC;
  }
#ifdef TWNDS_OS_APPLE
	TW_CALLBACK callback = {};
	callback.Message = MSG_CLOSEDSREQ;
	
	bRC = TWRC_SUCCESS==_DSM_Entry(getIdentity(),
					   (pTW_IDENTITY)NULL,
					   (TW_UINT32)DG_CONTROL,
					   (TW_UINT16)DAT_CALLBACK,
					   (TW_UINT16)MSG_INVOKE_CALLBACK,
					   (TW_MEMREF) &callback);
#else
  // Tell the DSM that DS request to close or disable our user interface
  // most likly user clicks on the “CLOSE” button on GUI
  if(TWRC_SUCCESS==_DSM_Entry(getIdentity(),
                              getApp(),
                              DG_CONTROL,
                              DAT_NULL,
                              MSG_CLOSEDSREQ,
                              0))
  {
    bRC = true;
  }
#endif
  return bRC;
}

//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_Base::DoCloseDSOkEvent()
{
  bool bRC = false;

  if( dsState_Enabled != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return bRC;
  }

#ifdef TWNDS_OS_APPLE
	TW_CALLBACK callback = {};
	callback.Message = MSG_CLOSEDSOK;
	
	bRC = TWRC_SUCCESS==_DSM_Entry(getIdentity(),
								   (pTW_IDENTITY)NULL,
								   (TW_UINT32)DG_CONTROL,
								   (TW_UINT16)DAT_CALLBACK,
								   (TW_UINT16)MSG_INVOKE_CALLBACK,
								   (TW_MEMREF) &callback);
#else
	// Tell the DSM that we are ready to send images
  // most likly user clicks on the “OK” button on GUI
  if(TWRC_SUCCESS==_DSM_Entry(getIdentity(),
                              getApp(),
                              DG_CONTROL,
                              DAT_NULL,
                              MSG_CLOSEDSOK,
                              0))
  {
    bRC = true;
  }
#endif
  return bRC;
}

//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_Base::DoDeviceEvent()
{
  //Report to the DSM that a device event has occurred
  bool bRC = false;
  return bRC;
}

//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY CTWAINDS_Base::getApp()
{
  return &m_App;
}

//////////////////////////////////////////////////////////////////////////////
template<class TWAINContainerType> 
TW_INT16 CTWAINDS_Base::handleCap(TW_UINT16 _MSG, TWAINContainerType* _pContainer, pTW_CAPABILITY _pCap)
{
  TW_INT16 twrc = TWRC_FAILURE;

  if(MSG_QUERYSUPPORT == _MSG) // special case
  {
    _pCap->ConType = TWON_ONEVALUE;
    _pCap->hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE));

    if(0 != _pCap->hContainer)
    {
      pTW_ONEVALUE pCap = (pTW_ONEVALUE)_DSM_LockMemory(_pCap->hContainer);

      pCap->ItemType = TWTY_INT32;
      pCap->Item = _pContainer->getMSG_QUERYSUPPORT();

      _DSM_UnlockMemory(_pCap->hContainer);
      twrc = TWRC_SUCCESS;
    }
  }
  else
  {
    // first check if the operation is allowed on this container
    if(!_pContainer->isOperationAllowed(_MSG))
    {
      setConditionCode(TWCC_CAPBADOPERATION);
      return TWRC_FAILURE;
    }

    switch(_pContainer->GetItemType())
    {
      case TWTY_INT8:
      case TWTY_INT16:
      case TWTY_INT32:
      case TWTY_UINT8:
      case TWTY_UINT16:
      case TWTY_BOOL:
      case TWTY_FIX32:
      case TWTY_FRAME:
      case TWTY_STR32:
      case TWTY_STR64:
      case TWTY_STR128:
      case TWTY_STR255:
        twrc = TWRC_SUCCESS; // so far so good
      break;
      
      default:
        setConditionCode(TWCC_CAPBADOPERATION);
        return TWRC_FAILURE;
      break;

    } // switch(_pContainer->GetGetType())

    if(TWRC_SUCCESS == twrc)
    {
      // Now that the type is valid, do the actual operation
      switch(_MSG)
      {
        case MSG_RESET:
          //MSG_RESET is supposed to reset and then return the current value
          if(m_CurrentState >= dsState_XferReady)
          {
            setConditionCode(TWCC_SEQERROR);
            twrc = TWRC_FAILURE;
            break;
          }
          _pContainer->Reset();
          //change the behavior to a get current
          _MSG = MSG_GETCURRENT;
        case MSG_GETCURRENT:
        case MSG_GETDEFAULT:
        case MSG_GET:
          _pCap->ConType = _pContainer->GetGetType(_MSG);
          twrc = updatePreDependencies(_pContainer);
          if(twrc != TWRC_SUCCESS)
          {
            break;
          }

          _pCap->hContainer = _pContainer->GetContainer(_MSG);
          twrc = updatePostContainer(_pCap);
          break;

        case MSG_SET:
          {
            if(m_CurrentState >= dsState_XferReady)
            {
              setConditionCode(TWCC_SEQERROR);
              twrc = TWRC_FAILURE;
              break;
            }
            twrc = updatePreDependencies(_pContainer);
            if(twrc != TWRC_SUCCESS)
            {
              break;
            }
            twrc = updatePreContainer(_pCap);
            if(twrc!=TWRC_SUCCESS)
            {
              break;
            }
            BYTE * pContainer = (BYTE*)_DSM_LockMemory(_pCap->hContainer);
            if(pContainer==0)
            {
              setConditionCode(TWCC_LOWMEMORY);
              twrc = TWRC_FAILURE;
              break;
            }
            twrc = validateCapabilitySet(_pCap->Cap,_pCap->ConType, pContainer);
            _DSM_UnlockMemory(_pCap->hContainer);
            if(twrc != TWRC_SUCCESS)
            {
              setConditionCode(TWCC_BADVALUE);
              if(twrc == TWRC_FAILURE)
              {
                break;
              }
            }

            TW_INT16 Condition;
            TW_INT16 twrc2 = _pContainer->Set(_pCap, Condition);
            if(twrc2 != TWRC_SUCCESS)
            {
              setConditionCode(Condition);
              if(twrc == TWRC_SUCCESS && twrc2 != TWRC_SUCCESS)
              {
                twrc = twrc2;
              }
            }
          }
        break;

        default:
          setConditionCode(TWCC_CAPBADOPERATION);
        break;
      }
    } // if(TWRC_SUCCESS == twrc)
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::validateCapabilitySet(TW_UINT16 _Cap, TW_UINT16  _ConType, BYTE* _pContainer)
{
  TW_INT16 twrc  = TWRC_SUCCESS;

  switch(_Cap)
  {
    case CAP_XFERCOUNT:
    {
      twrc = TWRC_FAILURE;
      if(TWON_ONEVALUE == _ConType)
      {
        pTW_ONEVALUE pCap = (pTW_ONEVALUE)_pContainer;

        if(pCap)
        {
          if( (TW_INT16)pCap->Item == -1 || (TW_INT16)pCap->Item > 0 )
          {
            twrc = TWRC_SUCCESS;
          }
        }
      }
      break;
    }
    case ICAP_GAMMA:
    {
      twrc = TWRC_FAILURE;
      if(TWON_ONEVALUE == _ConType)
      {
        pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)_pContainer;
        if(pCap)
        {
          float flVal = FIX32ToFloat(pCap->Item);
          if( flVal>0 )
          {
            twrc = TWRC_SUCCESS;
          }
        }
      }
      break;
    }
    case ICAP_FRAMES:
    {
      int   unit = TWUN_PIXELS;
      float Xres = 100;
      float Yres = 100;
      twrc = getCurrentUnits(unit, Xres, Yres);

      if(TWON_ONEVALUE == _ConType)
      {
        pTW_ONEVALUE_FRAME pCap = (pTW_ONEVALUE_FRAME)_pContainer;

        if(pCap && pCap->ItemType == TWTY_FRAME)
        {
          InternalFrame frame(pCap->Item, unit, Xres, Yres);
          bool bConstrained;
          if(!ConstrainFrameToScanner(frame,bConstrained))
          {
            setConditionCode(TWCC_BADVALUE);
            twrc = TWRC_FAILURE;
          }
          else if(bConstrained)
          {
            pCap->Item = frame.AsTW_FRAME(unit, Xres, Yres);
            twrc = TWRC_CHECKSTATUS;
          }
        }
      }
      else if(TWON_ENUMERATION == _ConType)
      {
        CTWAINContainerInt *pDepCapMax = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_MAXFRAMES));
        if(pDepCapMax==0)
        {
          setConditionCode(TWCC_BUMMER);
          twrc = TWRC_FAILURE;
        }
        else
        {
          pTW_ENUMERATION_FRAME pCap = (pTW_ENUMERATION_FRAME)_pContainer;
          int nMax;
          if(!pDepCapMax->GetCurrent(nMax) || pCap->NumItems>(TW_UINT32)nMax)
          {
            setConditionCode(TWCC_BADVALUE);
            twrc = TWRC_FAILURE;
          }
          else
          {
            if(pCap && pCap->ItemType == TWTY_FRAME)
            {
              for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
              {
                InternalFrame frame(pCap->ItemList[x], unit, Xres, Yres);
                bool bConstrained;
                if(!ConstrainFrameToScanner(frame,bConstrained))
                {
                  setConditionCode(TWCC_BADVALUE);
                  twrc = TWRC_FAILURE;
                }
                else if(bConstrained)
                {
                  pCap->ItemList[x] = frame.AsTW_FRAME(unit, Xres, Yres);
                  twrc = TWRC_CHECKSTATUS;
                }
              }
            }
          }
        }
      }
      break;
    }
    case ICAP_MAXFRAMES:
      if(TWON_ONEVALUE == _ConType)
      {
        TW_ONEVALUE *pCap = (TW_ONEVALUE*)_pContainer;

        if(!pCap || pCap->ItemType != TWTY_UINT16 || pCap->Item!=1)
        {
          twrc = TWRC_FAILURE;
        }
      }
      else
      {
        setConditionCode(TWCC_CAPBADOPERATION);
        twrc = TWRC_FAILURE;
      }
      break;    
    default:
    break;
  }

  return twrc;
}
//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::updatePreContainer(pTW_CAPABILITY _pCap)
{
  TW_INT16 twrc  = TWRC_SUCCESS;
  BYTE * pContainer = (BYTE*)_DSM_LockMemory(_pCap->hContainer);
  if(pContainer==0)
  {
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  switch(_pCap->Cap)
  {
    case ICAP_XRESOLUTION:
    case ICAP_YRESOLUTION:
    {
      int nUnit = TWUN_INCHES;
      CTWAINContainerInt    *pnCap = 0;
      if( 0 == (pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_UNITS)))
       || false == pnCap->GetCurrent(nUnit))
      {
        setConditionCode(TWCC_OPERATIONERROR);
        twrc = TWRC_FAILURE;
      }
      else
      {
        if(nUnit==TWUN_PIXELS)
        {
          setConditionCode(TWCC_CAPSEQERROR);
          twrc = TWRC_FAILURE;
        }
        else
        {
          if(TWON_ONEVALUE == _pCap->ConType)
          {
            pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)pContainer;

            if(pCap==0 || pCap->ItemType != TWTY_FIX32)
            {
              setConditionCode(TWCC_BADVALUE);
              twrc = TWRC_FAILURE;
            }
            else
            {
              pCap->Item = ConvertUnits(pCap->Item,TWUN_INCHES,nUnit,1.0);
            }
          }
          else if(TWON_ENUMERATION == _pCap->ConType)
          {
            pTW_ENUMERATION_FIX32 pCap = (pTW_ENUMERATION_FIX32)pContainer;
            if(pCap==0 || pCap->ItemType != TWTY_FIX32)
            {
              setConditionCode(TWCC_BADVALUE);
              twrc = TWRC_FAILURE;
            }
            else
            {
              for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
              {
                pCap->ItemList[x] = ConvertUnits(pCap->ItemList[x],TWUN_INCHES,nUnit,1.0);
              }
            }
          }
        }
      }
    }
    break;
  }
  _DSM_UnlockMemory(_pCap->hContainer);

  return twrc;
}
//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::updatePostContainer(pTW_CAPABILITY _pCap)
{
  TW_INT16 twrc  = TWRC_SUCCESS;
  BYTE * pContainer = (BYTE*)_DSM_LockMemory(_pCap->hContainer);
  if(pContainer==0)
  {
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  switch(_pCap->Cap)
  {
    case ICAP_XRESOLUTION:
    case ICAP_YRESOLUTION:
    {
      int   nUnit = TWUN_PIXELS;
      float Xres = 100;
      float Yres = 100;
      twrc = getCurrentUnits(nUnit, Xres, Yres);
      if(_pCap->Cap==ICAP_YRESOLUTION)
      {
        Xres = Yres;
      }
      if(twrc==TWRC_SUCCESS)
      {
        if(TWON_ONEVALUE == _pCap->ConType)
        {
          pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)pContainer;

          if(pCap==0 || pCap->ItemType != TWTY_FIX32)
          {
            setConditionCode(TWCC_BADVALUE);
            twrc = TWRC_FAILURE;
          }
          else
          {
            pCap->Item = ConvertUnits(pCap->Item,nUnit,TWUN_INCHES,Xres);
          }
        }
        else if(TWON_ENUMERATION == _pCap->ConType)
        {
          pTW_ENUMERATION_FIX32 pCap = (pTW_ENUMERATION_FIX32)pContainer;
          if(pCap==0 || pCap->ItemType != TWTY_FIX32)
          {
            setConditionCode(TWCC_BADVALUE);
            twrc = TWRC_FAILURE;
          }
          else
          {
            for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
            {
              pCap->ItemList[x] = ConvertUnits(pCap->ItemList[x],nUnit,TWUN_INCHES,Xres);
            }
          }
        }
      }
    }
    break;
    case ICAP_PHYSICALHEIGHT:
    case ICAP_PHYSICALWIDTH:
    {
      int   nUnit = TWUN_PIXELS;
      float Xres = 100;
      float Yres = 100;
      twrc = getCurrentUnits(nUnit, Xres, Yres);
      if(_pCap->Cap==ICAP_PHYSICALHEIGHT)
      {
        Xres = Yres;
      }
      if(twrc==TWRC_SUCCESS)
      {
        if(TWON_ONEVALUE == _pCap->ConType)
        {
          pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)pContainer;

          if(pCap==0 || pCap->ItemType != TWTY_FIX32)
          {
            setConditionCode(TWCC_BADVALUE);
            twrc = TWRC_FAILURE;
          }
          else
          {
            pCap->Item = ConvertUnits(pCap->Item,TWUN_INCHES,nUnit,Xres);
          }
        }
        else
        {
          setConditionCode(TWCC_OPERATIONERROR);
          twrc = TWRC_FAILURE;
        }
      }
    }
    break;
  }
  _DSM_UnlockMemory(_pCap->hContainer);

  return twrc;
}
//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::updatePreDependencies(CTWAINContainer* _pContainer)
{
  TW_INT16 twrc  = TWRC_SUCCESS;

  switch(_pContainer->GetCapID())
  {
    case ICAP_FRAMES:
    {
      int   unit = TWUN_PIXELS;
      float Xres = 100;
      float Yres = 100;
      twrc = getCurrentUnits(unit, Xres, Yres);
      CTWAINContainerFrame *pnCap = dynamic_cast<CTWAINContainerFrame*>(_pContainer);
      if(pnCap)
      {
        pnCap->setCurrentUnits(unit, Xres, Yres);
      }
      else
      {
        twrc = TWRC_FAILURE;
      }
    }
    break;

    default:
    break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::updatePostDependencies(TW_UINT16 MSG, TW_UINT16 Cap)
{
  TW_INT16 twrc  = TWRC_SUCCESS;

  // Handle some special post dependancy cases
  // This is where capabilities that depend on the one that just got changed
  // must also get changed so they stay in sync.

  // Check to see what got changed
  switch(Cap)
  {      
    case ICAP_SUPPORTEDSIZES:
    {
      // if ICAP_SUPPORTEDSIZES was modified, ICAP_FRAME needs to be synced.
      CTWAINContainerInt   *pDepCapSS     = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_SUPPORTEDSIZES));
      CTWAINContainerFrame *pDepCapFrames = dynamic_cast<CTWAINContainerFrame*>(findCapability(ICAP_FRAMES));
      
      if(0 == pDepCapSS || 0 == pDepCapFrames)
      {
        setConditionCode(TWCC_CAPBADOPERATION);
        twrc = TWRC_FAILURE;
        break;
      }

      if( MSG == MSG_SET
       || MSG == MSG_RESET )
      {
        int ss;
        pDepCapSS->GetCurrent(ss);
        InternalFrame frame(ss);
        bool bConstrained;
        ConstrainFrameToScanner(frame,bConstrained);
        pDepCapFrames->Set(frame);
      }        
    }
    break;
    
    case ICAP_FRAMES:
    {
      // if ICAP_FRAMES was modified, ICAP_SUPPORTEDSIZES needs to be synced.
      CTWAINContainerInt   *pDepCapSS     = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_SUPPORTEDSIZES));
      CTWAINContainerFrame *pDepCapFrames = dynamic_cast<CTWAINContainerFrame*>(findCapability(ICAP_FRAMES));

      if(0 == pDepCapSS || 0 == pDepCapFrames)
      {
        setConditionCode(TWCC_CAPBADOPERATION);
        twrc = TWRC_FAILURE;
        break;
      }

      if( MSG == MSG_SET
       || MSG == MSG_RESET )
      {
        // Figure out if the current frame is in our list of ICAP_SUPPORTEDSIZES
        // if it is in the list, then we need to set the current SS to match
        // loop through all of our Supported Sizes and make a frame from them
        // then see if the frame matches the one we have.

        InternalFrame curFrame; 
        pDepCapFrames->GetCurrent(curFrame);
        const IntVector listInts = pDepCapSS->GetSupported();

        const int nSize = (int)(listInts.size());
        int x;
        for(x = 0; x < nSize; ++x)
        {
          int ss = listInts[x];
          InternalFrame frame(ss);
          if(curFrame == frame)
          {
            pDepCapSS->SetCurrent(ss);
            break;
          }
        }

        // No match found
        if(x >= nSize)
        {
          if(!pDepCapSS->SetCurrent(TWSS_NONE))
          {
            pDepCapSS->Add(TWSS_NONE);
            pDepCapSS->SetCurrent(TWSS_NONE);
          }
        }
      }        
    }
    break;
    case ICAP_XRESOLUTION:
    case ICAP_YRESOLUTION:
    case ICAP_UNITS:
    {
      int   unit;
      float Xres, Yres;
      CTWAINContainerFrame *pDepCapFrames = dynamic_cast<CTWAINContainerFrame*>(findCapability(ICAP_FRAMES));

      if(pDepCapFrames ==0)
      {
        setConditionCode(TWCC_BUMMER);
        twrc = TWRC_FAILURE;
        break;
      }

      if(getCurrentUnits(unit, Xres, Yres)==TWRC_SUCCESS)
      {
        pDepCapFrames->setCurrentUnits(unit, Xres, Yres);
      }
      else
      {
        setConditionCode(TWCC_BUMMER);
        return TWRC_FAILURE;
      }
    }
    break;
    case CAP_FEEDERENABLED:
      //TODO add routine here if CAP_DUPLEXENABLED supports TRUE 
    break;
    default:
    break;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_Base::ConstrainFrameToScanner(InternalFrame& _frame, bool &bConstrained)
{
  bConstrained = false;
  int nMaxHValue = 0x7FFFFFFF; //max frame size fit in InternalFrame
  int nMinHValue = 0;
  int nMaxWValue = 0x7FFFFFFF; //max frame size fit in InternalFrame
  int nMinWValue = 0;
  float fTemp;
  // ICAP_PHYSICALWIDTH and ICAP_PHYSICALHEIGHT are required Caps while
  // ICAP_MINIMUMWIDTH and ICAP_MINIMUMHEIGHT are not required caps.
  CTWAINContainerFix32* pPhysicalWidth  = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_PHYSICALWIDTH));
  CTWAINContainerFix32* pPhysicalHeight = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_PHYSICALHEIGHT));
  CTWAINContainerFix32* pMinWidth       = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_MINIMUMWIDTH));
  CTWAINContainerFix32* pMinHeight      = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_MINIMUMHEIGHT));
  
  if(pPhysicalWidth)
  {
    if(pPhysicalWidth->GetCurrent(fTemp))
    {
      nMaxWValue = (int)(fTemp*1000.0);
    }
  }
  if(pPhysicalHeight)
  {
    if(pPhysicalHeight->GetCurrent(fTemp))
    {
      nMaxHValue = (int)(fTemp*1000.0);
    }
  }
  if(pMinWidth)
  {
    if(pMinWidth->GetCurrent(fTemp))
    {
      nMinWValue = (int)(fTemp*1000.0);
    }
  }
  if(pMinHeight)
  {
    if(pMinHeight->GetCurrent(fTemp))
    {
      nMinHValue = (int)(fTemp*1000.0);
    }
  }

  // Constrain the width
  if(_frame.nLeft < 0)
  {
    /*
    _frame.nLeft = 0;
    bConstrained = true;
    */
    return false;
  }
  if(_frame.nRight <= 0 || _frame.nRight > nMaxWValue)
  {
    /*
    _frame.nRight = nMaxWValue;
    bConstrained = true;
    */
    return false;
  }
  if(_frame.nLeft >= _frame.nRight)
  {
    _frame.nLeft = 0;
    _frame.nRight = nMaxWValue;
    bConstrained = true;
  }

  if((_frame.nRight-_frame.nLeft) < nMinWValue)
  {
    _frame.nLeft  = max( 0, _frame.nLeft - (nMinWValue - (_frame.nRight-_frame.nLeft)) );
    _frame.nRight = _frame.nLeft + max(nMinWValue, _frame.nRight-_frame.nLeft);
    bConstrained = true;
  }

  // Constrain the height
  if(_frame.nTop < 0)
  {
    /*_frame.nTop = 0;
    bConstrained = true;*/
    return false;
  }
  if(_frame.nBottom <= 0 || _frame.nBottom>nMaxHValue)
  {
    /*
    _frame.nBottom = nMaxHValue;
    bConstrained = true;
    */
    return false;
  }
  if(_frame.nTop >= _frame.nBottom)
  {
    _frame.nTop = 0;
    _frame.nBottom = nMaxHValue;
    bConstrained = true;
  }  
  if(( _frame.nBottom-_frame.nTop)< nMinHValue)
  {
    _frame.nTop  = max( 0, _frame.nTop - (nMinHValue - (_frame.nBottom-_frame.nTop)) );
    _frame.nBottom = _frame.nTop + max(nMinHValue, _frame.nBottom-_frame.nTop);
    bConstrained = true;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getCurrentUnits(int &Unit, float &Xres, float &Yres)
{
  TW_INT16               twrc  = TWRC_SUCCESS;
  CTWAINContainerFix32  *pfCap = 0;
  CTWAINContainerInt    *pnCap = 0;

  // X resolution
  if( 0 == (pfCap = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_XRESOLUTION)))
   || false == pfCap->GetCurrent(Xres) )
  {
    setConditionCode(TWCC_OPERATIONERROR);
    twrc = TWRC_FAILURE;
  }

  // Y resolution
  if( 0 == (pfCap = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_YRESOLUTION)))
   || false == pfCap->GetCurrent(Yres) )
  {
    setConditionCode(TWCC_OPERATIONERROR);
    twrc = TWRC_FAILURE;
  }

  // Unit
  if( 0 == (pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_UNITS)))
   || false == pnCap->GetCurrent(Unit) )
  {
    setConditionCode(TWCC_OPERATIONERROR);
    twrc = TWRC_FAILURE;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getFileXfer(pTW_SETUPFILEXFER _pData)
{
  // valid to call for 4, 5, & 6
  if( !(dsState_Open      == m_CurrentState ||
        dsState_Enabled   == m_CurrentState ||
        dsState_XferReady == m_CurrentState   ))
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  CTWAINContainerInt* pCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_IMAGEFILEFORMAT));
  if(0 == pCap)
  {
    setConditionCode(TWCC_BADPROTOCOL);
    return TWRC_FAILURE;
  }
  
  SSTRCPY(_pData->FileName, sizeof(m_CurFileExferName), m_CurFileExferName);
  int nCurFF;
  pCap->GetCurrent(nCurFF);
  _pData->Format = nCurFF;

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getDefaultFileXfer(pTW_SETUPFILEXFER _pData)
{
  // valid to call for 4, 5, & 6
  if( !(dsState_Open      == m_CurrentState ||
        dsState_Enabled   == m_CurrentState ||
        dsState_XferReady == m_CurrentState   ))
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  CTWAINContainerInt* pCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_IMAGEFILEFORMAT));
  if(0 == pCap)
  {
    setConditionCode(TWCC_BADPROTOCOL);
    return TWRC_FAILURE;
  }

  SSTRCPY(_pData->FileName, sizeof(m_DefFileExferName), m_DefFileExferName);
  int nDefFF;
  pCap->GetDefault(nDefFF);
  _pData->Format = nDefFF;

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::resetFileXfer(pTW_SETUPFILEXFER _pData)
{
  // valid to call for 4 only
  if( !(dsState_Open == m_CurrentState))
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  CTWAINContainerInt* pCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_IMAGEFILEFORMAT));
  if(0 == pCap)
  {
    setConditionCode(TWCC_BADPROTOCOL);
    return TWRC_FAILURE;
  }
  SSTRCPY(m_CurFileExferName, sizeof(m_CurFileExferName), m_DefFileExferName);
  SSTRCPY(_pData->FileName, sizeof(m_CurFileExferName), m_CurFileExferName);
  pCap->Reset();
  int nCurFF = 0;
  pCap->GetCurrent(nCurFF);
  _pData->Format = nCurFF;

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::setFileXfer(pTW_SETUPFILEXFER _pData)
{
  // valid to call for 4, 5, & 6
  if( !(dsState_Open      == m_CurrentState ||
        dsState_Enabled   == m_CurrentState ||
        dsState_XferReady == m_CurrentState   ))
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  CTWAINContainerInt* pCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_IMAGEFILEFORMAT));
  if(0 == pCap)
  {
    setConditionCode(TWCC_BADPROTOCOL);
    return TWRC_FAILURE;
  }
  
  if(!pCap->SetCurrent(_pData->Format))
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_CHECKSTATUS;
  }
  SSTRCPY(m_CurFileExferName, sizeof(m_CurFileExferName), _pData->FileName);

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::saveImageFile()
{
  TW_INT16 twrc = TWRC_FAILURE;

  if( dsState_XferReady != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // Get the image that should be waiting for us.
  if(TWRC_SUCCESS != transfer())
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  int nCurFF = -1;
  CTWAINContainerInt* pCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_IMAGEFILEFORMAT));
  if(0 == pCap || !pCap->GetCurrent(nCurFF))
  {
    setConditionCode(TWCC_BADPROTOCOL);
    return TWRC_FAILURE;
  }

  // Do a quick test to see if we got a supported file type.
  switch (nCurFF)
  {
    case TWFF_BMP:
      twrc = saveImageFileAsBMP();
      break;

    case TWFF_TIFF:
      twrc = saveImageFileAsTIFF();
      break;

    /* 
    Untill compression is implimented we  support Jpeg
    case TWFF_JFIF:
      break;
    */

    default:
      setConditionCode(TWCC_BADVALUE);
      return TWRC_FAILURE;
      break;
  }

  if(twrc == TWRC_XFERDONE)
  {
    m_CurrentState = dsState_Xferring;
  }

  return twrc;
}
//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::saveImageFileAsTIFF()
{
  TW_INT32      nWidth   = m_ImageInfo.ImageWidth;
  TW_INT32      nHeight  = m_ImageInfo.ImageLength;
  TW_INT16      nBPP     = m_ImageInfo.BitsPerPixel;
  TW_INT32      nBPL     = BYTES_PERLINE(nWidth, nBPP);
  CTiffWriter  *pTifImg  = new CTiffWriter(m_CurFileExferName, nWidth, nHeight, nBPP, nBPL);
  if(!pTifImg)
  {
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  pTifImg->setXResolution(m_ImageInfo.XResolution.Whole, 1);
  pTifImg->setYResolution(m_ImageInfo.YResolution.Whole, 1);
  pTifImg->writeImageHeader();

  BYTE *pImage = (BYTE *)_DSM_LockMemory(m_hImageData);
  if(pImage == NULL)
  {
    setConditionCode(TWCC_BADVALUE);
    delete pTifImg;
    return TWRC_FAILURE;
  }

  bool bSwitch = true;
  CTWAINContainerInt *pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_BITORDER));
  if(pnCap)
  {
    int nVal;
    if(pnCap->GetCurrent(nVal))
    {
      bSwitch = nVal==TWBO_LSBFIRST? true:false;
    }
  }

  // write the received image data to the image file
  if( m_ImageInfo.BitsPerPixel < 24 // BW or Gray
   || m_ImageInfo.BitsPerPixel > 24 && !bSwitch) //Color but  have to switch between RGB and BGR
  {
    if(!pTifImg->WriteTIFFData(reinterpret_cast<char*>(pImage), nBPL*nHeight))
    {
      setConditionCode(TWCC_FILEWRITEERROR);
      _DSM_UnlockMemory(m_hImageData);
      delete pTifImg;
      return TWRC_FAILURE;
    }
  }
  else // color
  {
    // we need to reverse the color from BRG to RGB
    BYTE      *pLineBuff = NULL;
    TW_HANDLE  hLineBuff = _DSM_Alloc(nBPL);

    if( NULL == hLineBuff )
    {
      setConditionCode(TWCC_LOWMEMORY);
      return TWRC_FAILURE;
    }

    pLineBuff = (BYTE*)_DSM_LockMemory(hLineBuff);
    if(NULL == pLineBuff)
    {
      _DSM_Free(hLineBuff);
      setConditionCode(TWCC_LOWMEMORY);
      return TWRC_FAILURE;
    }

    BYTE *pSource = pImage;
    BYTE *pDest   = NULL;


    for(TW_INT32 row=0; row<nHeight; row++)
    {
      pSource = pImage + row*nBPL;
      pDest   = pLineBuff;

      // need to switch from BGR to RGB
      for(TW_INT32 nCol=0; nCol<nWidth; nCol++)
      {
        *pDest++ = pSource[2];
        *pDest++ = pSource[1];
        *pDest++ = pSource[0];
        pSource += 3;
      }

      // Save each line after it is converted.
      if(!pTifImg->WriteTIFFData(reinterpret_cast<char*>(pLineBuff), nBPL))
      {
        setConditionCode(TWCC_FILEWRITEERROR);
        _DSM_UnlockMemory(m_hImageData);
        delete pTifImg;
        return TWRC_FAILURE;
      }
    }

    _DSM_UnlockMemory(hLineBuff);
    _DSM_Free(hLineBuff);

  }
  delete pTifImg;
  _DSM_UnlockMemory(m_hImageData);

  return TWRC_XFERDONE;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::saveImageFileAsBMP()
{
  // Save the image to disk
  FILE *pFile;
  FOPEN(pFile, m_CurFileExferName, "wb");
  if(pFile == 0)
  {
    setConditionCode(TWCC_FILEWRITEERROR);
    return TWRC_FAILURE;
  }

  TW_HANDLE hDIB = 0;
  if( TWRC_SUCCESS != getDIBImage(hDIB) )
  {
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  // -Here we get a handle to a DIB. Save it to disk as a bmp. 
  PBITMAPINFOHEADER pDIB = (PBITMAPINFOHEADER)_DSM_LockMemory(hDIB);
  if( pDIB == 0 )
  {
    setConditionCode(TWCC_LOWMEMORY);
    _DSM_Free(hDIB);
    return TWRC_FAILURE;
  }

  int nOffSet  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed);
  int nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + pDIB->biSizeImage;
                
  BITMAPFILEHEADER bmpFIH;
  memset(&bmpFIH, 0, sizeof(BITMAPFILEHEADER));
  bmpFIH.bfType    = ( (WORD) ('M' << 8) | 'B');
  bmpFIH.bfOffBits = nOffSet;
  bmpFIH.bfSize    = sizeof(BITMAPFILEHEADER) + nDIBSize;

  fwrite(&bmpFIH, 1, sizeof(BITMAPFILEHEADER), pFile);
  fwrite(pDIB, 1, nDIBSize, pFile);
  fclose(pFile);
  pFile = 0;
  _DSM_UnlockMemory(hDIB);
  _DSM_Free(hDIB);

  return TWRC_XFERDONE;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getDIBImage(TW_HANDLE& _hImage)
{
  if( 0 == m_hImageData )
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  TW_INT16 twrc = TWRC_FAILURE;
  _hImage = 0;

  TW_HANDLE       hDIB                = NULL; //create a handle to a bitmap, which we'll allocate
  BYTE           *pSourceBuff         = NULL;
  BYTE           *pDestBuff           = NULL;
  BYTE           *pSrc                = NULL;
  BYTE           *pDst                = NULL;

  PBITMAPINFOHEADER pDIBInfoHeader    = NULL;
  const WORD      bpp                 = m_ImageInfo.BitsPerPixel;
  const DWORD     SrcWidth            = m_ImageInfo.ImageWidth;
  const DWORD     SrcLength           = m_ImageInfo.ImageLength;
  const DWORD     SrcBytesPerRow      = BYTES_PERLINE(SrcWidth, bpp);
  const DWORD     DstBytesPerRowAlgn  = BYTES_PERLINE_ALIGN4(SrcWidth, bpp);	//get the number of bytes per line we'll need for this image, and make sure it's DWORD-aligned
  const WORD      numcolors           = bpp==1?2:bpp==8?256:0;// B&W = 2,  Grey = 256,  Color = 0
  const WORD      palettesize         = sizeof(RGBQUAD)* numcolors;
  const DWORD     bitmapSize          = sizeof(BITMAPINFOHEADER) + palettesize + DstBytesPerRowAlgn*SrcLength;	//multiply the bytes-per-row by the number of scanlines, and add that to the size of the bitmap header to find the total size of the bitmap


  try
  {
    pSourceBuff = (BYTE *)_DSM_LockMemory(m_hImageData);
    if(pSourceBuff == NULL)
    {
      setConditionCode(TWCC_BADVALUE);
      throw TWCC_BADVALUE;
    }

    hDIB  = _DSM_Alloc(bitmapSize);	//create a buffer as large as the bitmap
    if(hDIB==NULL)	//if we couldn't allocate the memory
    {
      //show that we ran out of memory
      setConditionCode(TWCC_LOWMEMORY);
      throw TWCC_LOWMEMORY;
    }

    pDIBInfoHeader=(PBITMAPINFOHEADER)_DSM_LockMemory(hDIB);	//get a pointer to the bitmap info header
    if(pDIBInfoHeader==NULL)
    {
      //show that we ran out of memory
      setConditionCode(TWCC_LOWMEMORY);
      throw TWCC_LOWMEMORY;
    }

    pDIBInfoHeader->biSize          = sizeof(BITMAPINFOHEADER);	//show how big the header info is
    pDIBInfoHeader->biWidth         = SrcWidth;	//show how wide the image is
    pDIBInfoHeader->biHeight        = SrcLength;	//show how tall the image is	//G***shouldn't this be a negative number?
    pDIBInfoHeader->biPlanes        = 1;	//bitmaps only support one color plane
    pDIBInfoHeader->biBitCount      = bpp;	//set the bit depth
    pDIBInfoHeader->biCompression   = 0;	//we only support uncompressed bitmaps
    pDIBInfoHeader->biSizeImage     = DstBytesPerRowAlgn*SrcLength;	//uncompressed bitmaps can just set the size to zero
    pDIBInfoHeader->biXPelsPerMeter = static_cast<LONG>(FIX32ToFloat(m_ImageInfo.XResolution) * 39.37F + 0.5);
    pDIBInfoHeader->biYPelsPerMeter = static_cast<LONG>(FIX32ToFloat(m_ImageInfo.YResolution) * 39.37F + 0.5);
    pDIBInfoHeader->biClrUsed       = numcolors;
    pDIBInfoHeader->biClrImportant  = numcolors;	//all the colors are important

    _DSM_UnlockMemory(hDIB);	//unlock our DIB memory

    // Add Pallette
    pDestBuff =(BYTE*)_DSM_LockMemory(hDIB);	//get a pointer to the destination data
    if(pDestBuff==NULL)
    {
      //show that we ran out of memory
      setConditionCode(TWCC_LOWMEMORY);
      throw TWCC_LOWMEMORY;
    }

    pDst = pDestBuff + sizeof(BITMAPINFOHEADER);

    //fill the bitmap palette
    if(numcolors==2)
    {
      RGBQUAD         *pPal = (RGBQUAD*)pDst;
      pPal->rgbBlue = pPal->rgbGreen = pPal->rgbRed = 0x00;
      pPal->rgbReserved = 0x00;

      pPal++;
      pPal->rgbBlue = pPal->rgbGreen = pPal->rgbRed = 0xff;
      pPal->rgbReserved = 0x00;
    }
    else if(numcolors==256)
    {
      RGBQUAD         *pPal = (RGBQUAD*)pDst;
      for(int iPal = 0; iPal <= 255; iPal++, pPal++)
      {
        pPal->rgbBlue = pPal->rgbGreen = pPal->rgbRed = iPal;
        pPal->rgbReserved = 0x00;
      }
    }
    pDst += palettesize;

    bool bSwitch = false;
    CTWAINContainerInt *pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_BITORDER));
    if(pnCap)
    {
      int nVal;
      if(pnCap->GetCurrent(nVal))
      {
        bSwitch = nVal==TWBO_LSBFIRST? false:true;
      }
    }

    // flip the image top to bottom
    // for color change from BGR to RGB
    if(TWPT_RGB==m_ImageInfo.PixelType && bSwitch)
    {
      for(DWORD length=0; length<SrcLength; length++)
      {
        pSrc = pSourceBuff + ( SrcBytesPerRow*(SrcLength-length-1) );
        // need to switch from BGR to RGB
        for(WORD nCol = 0; nCol<m_ImageInfo.ImageWidth; nCol++)
        {
          pDst[(nCol*3)]   = pSrc[FI_RGBA_RED];
          pDst[(nCol*3)+1] = pSrc[FI_RGBA_GREEN];
          pDst[(nCol*3)+2] = pSrc[FI_RGBA_BLUE];
          pSrc += 3;
        }
        pDst += DstBytesPerRowAlgn;
      }
    }
    else // BW or Gray or color that does not need flipped
    {
      pSrc    = pSourceBuff + ( SrcBytesPerRow*(SrcLength-1) );
      for(DWORD length=0; length<SrcLength; length++)
      {
        memcpy(pDst, pSrc, SrcBytesPerRow);
        pDst += DstBytesPerRowAlgn;
        pSrc -= SrcBytesPerRow;
      }
    }

    twrc = TWRC_SUCCESS;
    _hImage = hDIB;
  
  } // try
  catch (WORD wConditionCodeError)
  {
    setConditionCode(wConditionCodeError);
    // in the future we will have logging implemented and the wConditionCodeError will be recorded 
    _hImage = 0;
  }

  // cleanup
  if(pSourceBuff)
  {
    _DSM_UnlockMemory(m_hImageData);
  }

  if(pDestBuff)
  {
    _DSM_UnlockMemory(hDIB);
  }

  if(twrc != TWRC_SUCCESS)
  {
    if(hDIB)
      _DSM_Free(hDIB);
  }

  return twrc;
}
#ifdef TWNDS_OS_APPLE
//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getPICTImage(TW_HANDLE &_hPICTImage)
{
	TW_INT16 twrc = TWRC_FAILURE;	
	if( 0 == m_hImageData )
	{
	  setConditionCode(TWCC_BADVALUE);
	  return TWRC_FAILURE;
	}  
	
	TW_HANDLE _hTIFFdata;
	twrc = getTIFFImage(_hTIFFdata);
	if(twrc!=TWRC_SUCCESS)
	{
		return twrc;
	}
	
	twrc = TWRC_FAILURE;
	setConditionCode(TWCC_BUMMER);
	
	Handle    dataRef = nil;
    OSErr err = PtrToHand( &_hTIFFdata, &dataRef, sizeof(Handle));
	if(err)
	{
	  return twrc;
	}
		
	GraphicsImportComponent gi; 
	GetGraphicsImporterForDataRef(dataRef, HandleDataHandlerSubType, &gi); 

	if(gi)
	{
	  PicHandle ph; 

		if (GraphicsImportGetAsPicture(gi, &ph) == noErr)
		{
			// Whew! We now have a PicHandle. Unfortunately, the caller
			// expects a normal Handle. Copy the data and kill the picture.
			UInt32	pictSize = GetHandleSize((Handle) ph);
			Handle	dataHandle = NewHandle(pictSize);
			
			if (dataHandle && MemError() == noErr)
			{
				BlockMoveData(*ph, *dataHandle, pictSize);
				_hPICTImage = dataHandle;
				twrc = TWRC_SUCCESS;
				setConditionCode(TWCC_SUCCESS);
			}
			KillPicture(ph);
		}
		
		CloseComponent(gi);		
	}
	_DSM_Free(_hTIFFdata);
	return twrc;
}
#endif

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::getTIFFImage(TW_HANDLE &_hTIFFImage)
{
  if( 0 == m_hImageData )
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }  

  _hTIFFImage = 0;
  BYTE *pImage = (BYTE *)_DSM_LockMemory(m_hImageData);
  if(pImage == NULL)
  {
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }
  
  TW_INT32      nWidth   = m_ImageInfo.ImageWidth;
  TW_INT32      nHeight  = m_ImageInfo.ImageLength;
  TW_INT16      nBPP     = m_ImageInfo.BitsPerPixel;
  TW_INT32      nBPL     = BYTES_PERLINE(nWidth, nBPP);
  CTiffWriter  *pTifImg  = new CTiffWriter("", nWidth, nHeight, nBPP, nBPL);
  if(!pTifImg)
  {
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  pTifImg->setXResolution(m_ImageInfo.XResolution.Whole, 1);
  pTifImg->setYResolution(m_ImageInfo.YResolution.Whole, 1);

  stringstream Header;
  pTifImg->GetImageHeader(Header);
  Header.seekp(0, ios_base::end);
  DWORD dwSize =(DWORD) Header.tellp();
  Header.seekg(0, ios_base::beg);
  TW_HANDLE hTiff  = _DSM_Alloc(dwSize+nBPL*nHeight);	//create a buffer as large as the bitmap
  if(hTiff==0)
  {
    setConditionCode(TWCC_LOWMEMORY);
    delete pTifImg;
    return TWRC_FAILURE;
  }
  char *pData = (char*)_DSM_LockMemory(hTiff);
  if(pData==0)
  {
    _DSM_Free(hTiff);
    setConditionCode(TWCC_LOWMEMORY);
    delete pTifImg;
    return TWRC_FAILURE;
  }

  Header.read(pData,dwSize);
  pData +=dwSize;

  bool bSwitch = true;
  CTWAINContainerInt *pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_BITORDER));
  if(pnCap)
  {
    int nVal;
    if(pnCap->GetCurrent(nVal))
    {
      bSwitch = nVal==TWBO_LSBFIRST? true:false;
    }
  }

  // write the received image data to the image file
  if( m_ImageInfo.BitsPerPixel < 24 // BW or Gray
   || m_ImageInfo.BitsPerPixel > 24 && !bSwitch) //Color but  have to switch between RGB and BGR
  {
    memcpy(pData,pImage,nBPL*nHeight);
  }
  else // color
  {
    BYTE *pSource = pImage;
    BYTE *pDest   = NULL;


    for(TW_INT32 row=0; row<nHeight; row++)
    {
      pSource = pImage;
      pDest   = (BYTE*)pData;

      // need to switch from BGR to RGB
      for(TW_INT32 nCol=0; nCol<nWidth; nCol++)
      {
        *pDest++ = pSource[2];
        *pDest++ = pSource[1];
        *pDest++ = pSource[0];
        pSource += 3;
      }
      pData += nBPL;
      pImage += nBPL;
    }
  }

  delete pTifImg;
  _DSM_UnlockMemory(m_hImageData);
  _DSM_UnlockMemory(hTiff);
  _hTIFFImage = hTiff;

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::transferMemoryBuffers(pTW_IMAGEMEMXFER _ImageXfer)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  if( !( dsState_XferReady == m_CurrentState ||
         dsState_Xferring  == m_CurrentState    ) )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }
  
  DWORD nDestBytesPerRow   = BYTES_PERLINE_ALIGN4(m_ImageInfo.ImageWidth, m_ImageInfo.BitsPerPixel);
  DWORD nSrcBytesPerRow    = BYTES_PERLINE(m_ImageInfo.ImageWidth, m_ImageInfo.BitsPerPixel);

  if(_ImageXfer->Memory.Length < nDestBytesPerRow)
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }
  
  if(0 == m_nDestScanLine)
  {
    // Get the image that should be waiting for us.
    transfer();
  }

  if(0 == m_hImageData)
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  BYTE *pSourceBuff = (BYTE *)_DSM_LockMemory(m_hImageData);
  if(pSourceBuff == NULL)
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  BYTE *pDestBuff   = 0;
  if(TWMF_POINTER & _ImageXfer->Memory.Flags)
  {
    pDestBuff   = (BYTE*)_ImageXfer->Memory.TheMem;
  }
  else if(TWMF_HANDLE & _ImageXfer->Memory.Flags)
  {
    pDestBuff   = (BYTE*)_DSM_LockMemory((TW_HANDLE)(_ImageXfer->Memory.TheMem));
  }
  if(pDestBuff == NULL)
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  m_CurrentState = dsState_Xferring;

  DWORD nRows = MIN(_ImageXfer->Memory.Length / nDestBytesPerRow, m_ImageInfo.ImageLength-m_nDestScanLine);

  bool bSwitch = true;
  CTWAINContainerInt *pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_BITORDER));
  if(pnCap)
  {
    int nVal;
    if(pnCap->GetCurrent(nVal))
    {
      bSwitch = nVal==TWBO_LSBFIRST? true:false;
    }
  }

  BYTE  *pSrc    = 0;
  BYTE  *pDst    = pDestBuff;
  // for color change from BGR to RGB
  if(TWPT_RGB==m_ImageInfo.PixelType && bSwitch)
  {
    for(DWORD row=0; row<nRows; row++)
    {
      pSrc = pSourceBuff + nSrcBytesPerRow * (m_nDestScanLine+row);
      pDst = pDestBuff   + nDestBytesPerRow * row;
      // need to switch from BGR to RGB
      for(WORD nCol = 0; nCol<m_ImageInfo.ImageWidth; nCol++)
      {
        *pDst++ = pSrc[2];
        *pDst++ = pSrc[1];
        *pDst++ = pSrc[0];
        pSrc += 3;
      }
    }
  }
  else // BW or Gray or Color that does not need to be flipped
  {
    pSrc    = pSourceBuff + nSrcBytesPerRow*(m_nDestScanLine);
    for(DWORD row=0; row<nRows; row++)
    {
      memcpy(pDst, pSrc, nSrcBytesPerRow);
      pDst += nDestBytesPerRow;
      pSrc += nSrcBytesPerRow;
    }
  }

  //uncompressed image data
  _ImageXfer->Compression = m_ImageInfo.Compression;
  //bytes per row (DWORD aligned as per spec)
  _ImageXfer->BytesPerRow = nDestBytesPerRow;
  //width in pixels
  _ImageXfer->Columns = m_ImageInfo.ImageWidth;
  //X offset always 0, only used in tiled images - obsolete
  _ImageXfer->XOffset = 0;
  //position of this strip from the beginning of the image
  _ImageXfer->YOffset = m_nDestScanLine;
  //update the number of rows and bytes written
  _ImageXfer->Rows= nRows;
  _ImageXfer->BytesWritten = nDestBytesPerRow*nRows;

  // increment the current scanline for next pass
  m_nDestScanLine+=nRows;

  // check for finished scan
  if(m_nDestScanLine >= (DWORD)m_ImageInfo.ImageLength)
  {
    //we are done early
    m_nDestScanLine = 0;
    twrc = TWRC_XFERDONE;
  }

  _DSM_UnlockMemory(m_hImageData);

  if(TWMF_HANDLE & _ImageXfer->Memory.Flags)
  {
     _DSM_UnlockMemory((TW_HANDLE)(_ImageXfer->Memory.TheMem));
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::transferNativeImage(TW_HANDLE &_hData)
{
  TW_INT16 twrc = TWRC_FAILURE;

  if( dsState_XferReady != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // Get the image that should be waiting for us.
  twrc = transfer();
  if(TWRC_SUCCESS == twrc)
  {
    // Native is basicaly an image file transfered by memory
    // Windows is a Device independent BMP
    // Mac has changed from a PICT to a TIFF
    // Linux is a TIFF
#if defined(TWNDS_OS_LINUX) 
	twrc = getTIFFImage(_hData);
#elif defined(TWNDS_OS_APPLE)
	twrc = getPICTImage(_hData);	  
#else
    twrc = getDIBImage(_hData);
#endif
    if( TWRC_SUCCESS == twrc )
    {
#if !defined(TWNDS_OS_APPLE)
		twrc = TWRC_XFERDONE;
#endif
      m_CurrentState = dsState_Xferring;
    }
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::GetGustomDSData(pTW_CUSTOMDSDATA _pDSData)
{
  setConditionCode(TWCC_BADPROTOCOL);
  return TWRC_FAILURE;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_Base::SetGustomDSData(pTW_CUSTOMDSDATA _pDSData)
{
  setConditionCode(TWCC_BADPROTOCOL);
  return TWRC_FAILURE;
}