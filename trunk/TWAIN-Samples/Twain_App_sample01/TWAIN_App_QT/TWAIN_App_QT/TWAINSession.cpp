#include "stdafx.h"
#include "TWAINSession.h"
#include "TwainString.h"
#pragma message("TODO: Fix")
#include "..\..\Src\DSMInterface.h"

class CDSMPrePostCaller
{
  public:
  inline CDSMPrePostCaller(CTWAINSession *pApp);
  inline ~CDSMPrePostCaller();
  protected:
  CTWAINSession *m_pTWAINApp;
};

inline CDSMPrePostCaller::CDSMPrePostCaller(CTWAINSession *pApp)
  : m_pTWAINApp(pApp)
{
  if(m_pTWAINApp)
  {
    m_pTWAINApp->PreDSMCall();
  }
  return;
}

inline CDSMPrePostCaller::~CDSMPrePostCaller()
{
  if(m_pTWAINApp)
  {
    m_pTWAINApp->PostDSMCall();
    m_pTWAINApp = NULL;
  }
  return;
}

map<TW_UINT32, CTWAINSession *> CTWAINSession::m_mapInstance;
TW_UINT32 CTWAINSession::m_uiInstCount = 0;
CRITICAL_SECTION CTWAINSession::m_csProtectInstanceMap;
int CTWAINSession::m_nRefCount = 0;

CTWAINSession::CTWAINSession()
  : m_uiState(1)
  , m_twInstID(0)
  , m_bInTWAIN(false)
{
  //Find out if this is the first time through
  if(0==m_nRefCount)
  {
    //Initialize the critical section that protects the instance map
    InitializeCriticalSection(&m_csProtectInstanceMap);
  }

  //Define the scope of the critical section
  {
    CAutoCriticalSection acsObject(&m_csProtectInstanceMap);
    //Update the instance information and reference counts
    m_twInstID = ++m_uiInstCount;
    m_nRefCount++;
  }

  //Initialize the DataSource request queue protection
  InitializeCriticalSection(&m_csProtectRequestQueue);

  //Initialize member variables
  memset(&m_twAppIdentity, 0, sizeof(m_twAppIdentity));
  memset(&m_twLastStatus, 0, sizeof(m_twLastStatus));
  memset(&m_twSourceIdentity, 0, sizeof(m_twSourceIdentity));
  TW_UINT16 m_uiState = 1;

  //Start with some default Application information
  m_twAppIdentity.Version.MajorNum = 2;
  m_twAppIdentity.Version.MinorNum = 0;
  m_twAppIdentity.Version.Language = TWLG_ENGLISH_CANADIAN;
  m_twAppIdentity.Version.Country = TWCY_CANADA;
  SSTRCPY(m_twAppIdentity.Version.Info, sizeof(m_twAppIdentity.Version.Info), "2.0.9");
  m_twAppIdentity.ProtocolMajor = TWON_PROTOCOLMAJOR;
  m_twAppIdentity.ProtocolMinor = TWON_PROTOCOLMINOR;
  m_twAppIdentity.SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
  SSTRCPY(m_twAppIdentity.Manufacturer, sizeof(m_twAppIdentity.Manufacturer), "App's Manufacturer");
  SSTRCPY(m_twAppIdentity.ProductFamily, sizeof(m_twAppIdentity.ProductFamily), "App's Product Family");
  SSTRCPY(m_twAppIdentity.ProductName, sizeof(m_twAppIdentity.ProductName), "Specific App Product Name");

  return;
}

CTWAINSession::~CTWAINSession()
{
  //Define the scope of the critical section
  {
    CAutoCriticalSection acsObject(&m_csProtectInstanceMap);
    //Cleanup the instance information and decrement the reference count
    if(m_mapInstance.end()!=m_mapInstance.find(m_twInstID))
    {
      //Erase our instance from the instance map
      m_mapInstance.erase(m_twInstID);
    }
    m_nRefCount--;
  }

  if(0==m_nRefCount)
  {
    //This is the last one out so destroy the critical section
    DeleteCriticalSection(&m_csProtectInstanceMap);
  }
  //Delete the DataSource request queue protection
  DeleteCriticalSection(&m_csProtectRequestQueue);
  return;
}

void CTWAINSession::SetApplicationIdentity(const TW_IDENTITY &twIdentity)
{
  //copy the identity information
  m_twAppIdentity = twIdentity;
  return;
}

TW_UINT16 CTWAINSession::LoadDSM()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TraceMessage("Loading DataSource Manager: %s", kTWAIN_DSM_DIR kTWAIN_DSM_DLL_NAME);
  //Attempt to load the DataSource Manager
  if(LoadDSMLib(kTWAIN_DSM_DIR kTWAIN_DSM_DLL_NAME))
  {
    twRC = TWRC_SUCCESS;
    m_uiState = 2;
    TraceMessage("* State transition 1 -> 2 *");
  }
  //Trace the result
  TraceMessage("Load DataSource Manager %s", (TWRC_SUCCESS==twRC)?"Success":"Failure");
  return twRC;
}

TW_UINT16 CTWAINSession::FreeDSM()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TraceMessage("Unloading DataSource Manager");
  if(2==m_uiState)
  {
    unLoadDSMLib();
    twRC = TWRC_SUCCESS;
    m_uiState = 1;
    TraceMessage("* State transition 2 -> 1 *");
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;
  }
  TraceMessage("Unload DataSource Manager %s", (TWRC_SUCCESS==twRC)?"Success":"Failure");
  return twRC;
}

TW_UINT16 CTWAINSession::DSM_Entry(TW_UINT32 uiDG, TW_UINT16 uiDAT, TW_UINT16 uiMSG, TW_MEMREF pData, pTW_IDENTITY pDataSource/*=NULL*/)
{
  //Assume failure
  TW_UINT16 twRC = TWRC_FAILURE;
  //Reset the condition code
  m_twLastStatus.ConditionCode = TWCC_SUCCESS;

  //Handle Pre/Post notifications
  CDSMPrePostCaller pcObject(this);

  if(MSG_PROCESSEVENT!=uiMSG)
  {
    //Trace out the beginning
    TraceMessage("%s, %s, %s, pData=0x%08X, pSrc=0x%08X", convertDataGroup_toString(uiDG), 
      convertDataArgType_toString(uiDAT), convertMessage_toString(uiMSG), pData, pDataSource);
  }

  //do not make re-entrant calls
  if(false == m_bInTWAIN)
  {
    CAutoFlag afObject(m_bInTWAIN);
    if(2 <= m_uiState)
    {
      TW_UINT16 twPrevState = m_uiState;
      //Another special case
      if(MSG_ENABLEDS==uiMSG)
      {
        //When ShowUI == false, there may be a bit of a race condition here
        m_uiState = 5;
      }
      //Call the actual DSM
      twRC = _DSM_Entry(&m_twAppIdentity, pDataSource, uiDG, uiDAT, uiMSG, pData);
      if((TWRC_FAILURE==twRC)||(TWRC_CHECKSTATUS==twRC))
      {
        //In this case automatically pull status
        _DSM_Entry(&m_twAppIdentity, pDataSource, DG_CONTROL, DAT_STATUS, MSG_GET, &m_twLastStatus);
      }
      else
      {
        //look for basic state transitions
        if((DAT_PENDINGXFERS==uiDAT)&&(MSG_RESET==uiMSG))
        {
          //only valid in state 6 - success means transition to state 5
          m_uiState = 5;
        }
        else
        {
          switch(uiMSG)
          {
            case MSG_OPENDSM:
              //only valid in state 2, success means transition to state 3
              m_uiState = 3;
              break;
            case MSG_CLOSEDSM:
              //only valid in state 3, success means transition to state 2
              m_uiState = 2;
              break;
            case MSG_OPENDS:
              //only valid in state 3, success means transition to state 4
              m_uiState = 4;
              break;
            case MSG_CLOSEDS:
              //only valid in state 4, success means transition to state 3
              m_uiState = 3;
              break;
            case MSG_ENABLEDSUIONLY:
            case MSG_ENABLEDS:
              //only valid in state 4, success means transition to state 5 (using max in case of ShowUI==false race condition)
              m_uiState = max(m_uiState, 5);
              break;
            case MSG_DISABLEDS:
              //only valid in state 5, success means transition to state 4
              m_uiState = 4;
              break;
            case MSG_ENDXFER:
              {
                //valid in state 6 or 7, success means transition to state 6 or 5
                pTW_PENDINGXFERS pXfer = static_cast<pTW_PENDINGXFERS>(pData);
                if(pXfer && (0==pXfer->Count))
                {
                  m_uiState = 5;
                }
                else
                {
                  m_uiState = 6;
                }
              }
              break;
            case MSG_GET:
              {
                switch(uiDAT)
                {
                  case DAT_IMAGENATIVEXFER:
                  case DAT_IMAGEFILEXFER:
                    //only valid in state 6, success means transition to state 7
                  case DAT_IMAGEMEMXFER:
                  case DAT_IMAGEMEMFILEXFER:
                    //only valid in state 6 or 7, success means transition to state 7
                    m_uiState = 7;
                    break;
                }
              }
              break;
          }
        }

        if(twPrevState!=m_uiState)
        {
          //state transition was detected
          TraceMessage("* State transition %d -> %d *", twPrevState, m_uiState);
        }
      }
    }
    else
    {
      //This is a sequence error
      m_twLastStatus.ConditionCode = TWCC_SEQERROR;
      twRC = TWRC_FAILURE;
    }
  }
  else
  {
    //call was re-entrant, most suitable condition code
    m_twLastStatus.ConditionCode = TWCC_DENIED;
    if(MSG_PROCESSEVENT!=uiMSG)
    {
      TraceMessage("*** Attempt to make re-entrant TWAIN call rejected");
    }
  }
  if(MSG_PROCESSEVENT!=uiMSG)
  {
    //Trace out the result
    TraceMessage("Result = %s, Condition Code = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::GetCurrentState()
{
  return m_uiState;
}

void CTWAINSession::GetLastStatus(TW_STATUS &twStatus)
{
  twStatus = m_twLastStatus;
  return;
}

TW_UINT16 CTWAINSession::OpenDSM(TW_MEMREF mrParent)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(2==m_uiState)
  {
    m_mrParent = mrParent;
    twRC = DSM_Entry(DG_CONTROL, DAT_PARENT, MSG_OPENDSM, &m_mrParent);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("OpenDSM Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::CloseDSM()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(3==m_uiState)
  {
    twRC = DSM_Entry(DG_CONTROL, DAT_PARENT, MSG_CLOSEDSM, &m_mrParent);
    if(TWRC_SUCCESS==twRC)
    {
      m_mrParent = NULL;
    }
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("CloseDSM Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::GetDataSourceID(const char *pszProductName, TW_IDENTITY &twIdentity)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(3<=m_uiState)
  {
    bool bFound = false;
    TW_IDENTITY twDataSource = {0};
    //Iterate through all the DataSources
    if(TWRC_SUCCESS==(twRC=DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETFIRST, &twDataSource)))
    {
      do
      {
        if(0==strncmp(twDataSource.ProductName, pszProductName, sizeof(twDataSource.ProductName)))
        {
          //DataSource was found, copy the information and stop looking
          twIdentity = twDataSource;
          twRC = TWRC_SUCCESS;
          bFound = true;
          break;
        }
      }while(TWRC_SUCCESS==(twRC=DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETNEXT, &twDataSource)));
    }
    //last check
    if(false==bFound)
    {
      //the DS was not found
      twRC = TWRC_FAILURE;
      m_twLastStatus.ConditionCode = TWCC_BADDEST;
    }
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("CloseDSM Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::OpenDS()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(3==m_uiState)
  {
    memset(&m_twSourceIdentity, 0, sizeof(m_twSourceIdentity));
    //Open the default data source
    if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT, &m_twSourceIdentity))
    {
      //Select the default data source
      twRC = OpenDS(m_twSourceIdentity);
    }
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("OpenDS Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::OpenDS(const char *pszProductName)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(3==m_uiState)
  {
    memset(&m_twSourceIdentity, 0, sizeof(m_twSourceIdentity));
    //Set just the ProductName of the Source Identity structure
    SSTRCPY(m_twSourceIdentity.ProductName, sizeof(m_twSourceIdentity.ProductName), pszProductName);
    //Attempt to select the source
    twRC = OpenDS(m_twSourceIdentity);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("OpenDS Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::OpenDS(TW_IDENTITY &twSource)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(3==m_uiState)
  {
    //Ask the DSM to open the requested Data Source
    twRC = DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_OPENDS, &twSource);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("OpenDS Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::CloseDS()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(4==m_uiState)
  {
    //Ask the DSM to close the requested Data Source
    twRC = DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_CLOSEDS, &m_twSourceIdentity);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("CloseDS Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCallback()
{
  //Must be called in state 4
  if(4 != m_uiState)
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    return TWRC_FAILURE;
  }

  //Initialize the callback structure
  TW_CALLBACK twCallBack = {0};
  //Provide our instance ID
  twCallBack.RefCon = m_twInstID;
  twCallBack.CallBackProc = static_cast<TW_MEMREF>(CTWAINSession::DSMCallback);

  //define the scope of the critical section
  {
    CAutoCriticalSection acsObject(&m_csProtectInstanceMap);
    //Store class pointer information in the instance map
    m_mapInstance[m_twInstID] = this;
  }
  
  //call the DSM
  return DSM_Entry(DG_CONTROL, DAT_CALLBACK, MSG_REGISTER_CALLBACK, &twCallBack, &m_twSourceIdentity);
}

TW_UINT16 CTWAINSession::EnableDS(TW_HANDLE hParent, bool ShowUI)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(4==m_uiState)
  {
    TW_USERINTERFACE twUI = {0};
    twUI.ShowUI = ShowUI;
    twUI.hParent = hParent;
    twRC = DSM_Entry(DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, &twUI, &m_twSourceIdentity);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("EnableDS Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::EnableDSUIOnly(TW_HANDLE hParent)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(4==m_uiState)
  {
    TW_USERINTERFACE twUI = {0};
    twUI.ShowUI = true;
    twUI.hParent = hParent;
    twRC = DSM_Entry(DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDSUIONLY, &twUI, &m_twSourceIdentity);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("EnableDSUIOnly Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::DisableDS()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(5==m_uiState)
  {
    TW_USERINTERFACE twUI = {0};
    twRC = DSM_Entry(DG_CONTROL, DAT_USERINTERFACE, MSG_DISABLEDS, &twUI, &m_twSourceIdentity);
  }
  else
  {
    //This is a sequence error
    m_twLastStatus.ConditionCode = TWCC_SEQERROR;
    twRC = TWRC_FAILURE;

    //Trace out the result
    TraceMessage("DisableDS Failed / Result = %s, Status = %s", convertReturnCode_toString(twRC), convertConditionCode_toString(m_twLastStatus.ConditionCode));
  }
  return twRC;
}

TW_UINT16 CTWAINSession::ProcessEvent(const TW_EVENT &twEvent)
{
  TW_UINT16 twRC = TWRC_NOTDSEVENT;
  TW_EVENT twCopy = twEvent;
  if(5<=m_uiState)
  {
    twRC = DSM_Entry(DG_CONTROL, DAT_EVENT, MSG_PROCESSEVENT, &twCopy, &m_twSourceIdentity);
    if(TWRC_DSEVENT==twRC)
    {
      switch(twCopy.TWMessage)
      {
        case MSG_XFERREADY:
        case MSG_CLOSEDSREQ:
        case MSG_CLOSEDSOK:
        case MSG_DEVICEEVENT:
          //this was one of our events!
          SignalDSRequest(twCopy.TWMessage);
          break;
      }
    }
  }
  return twRC;
}

TW_UINT16 CTWAINSession::GetDSRequest()
{
  TW_UINT16 twRC = MSG_NULL;
  //Define the scope of the critical section
  {
    CAutoCriticalSection acsObject(&m_csProtectRequestQueue);
    if(m_twDSRequestQueue.size())
    {
      twRC = m_twDSRequestQueue.front();
    }
  }
  return twRC;
}

void CTWAINSession::ClearDSRequest()
{
  m_twDSRequestQueue.pop();
  return;
}

TW_UINT16 CTWAINSession::DoTransfer()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_SETUPMEMXFER twSetupMemXfer = {0};
  TW_IMAGEMEMXFER twImageMemXfer = {0};

  //Time to inspect and figure out a few things
  TW_UINT16 twXferMech = TWSX_NATIVE;
  if(TWRC_SUCCESS==GetCapabilityOneValue(ICAP_XFERMECH, twXferMech))
  {
    TW_IMAGEINFO twInfo = {0};
    //This is to be called before any transfer
    if(TWRC_SUCCESS==DSM_Entry(DG_IMAGE, DAT_IMAGEINFO, MSG_GET, &twInfo, &m_twSourceIdentity))
    {
      //signal the derived class
      OnImageBegin(twInfo, twXferMech);

      bool bAbort = false;
      do
      {
        switch(twXferMech)
        {
          case TWSX_NATIVE:
            {
              TW_HANDLE hImage = NULL;
              //Perform a Native transfer
              twRC = DSM_Entry(DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, &hImage, &m_twSourceIdentity);
              if((TWRC_XFERDONE==twRC)||(TWRC_SUCCESS==twRC))
              {
                //Assume we are to free the image
                bool bFree = true;
                //Notify the derived class
                OnNativeTransfer(&hImage, bAbort, bFree);
                if(bFree)
                {
                  //Discard the result
                  _DSM_Free(hImage);
                  hImage = NULL;
                }
                
                if(TWRC_SUCCESS==twRC)
                {
                  TraceMessage("*** Note: DS responded with invalid TWRC_SUCCESS for DAT_IMAGENATIVEXFER/MSG_GET Expected: TWRC_XFERDONE ***");
                  twRC = TWRC_XFERDONE;
                }
              }
            }
            break;
          case TWSX_FILE:
              {
                TW_SETUPFILEXFER twSetupFileXfer = {0};
                twRC = DSM_Entry(DG_CONTROL, DAT_SETUPFILEXFER, MSG_GET, &twSetupFileXfer, &m_twSourceIdentity);
                if(TWRC_SUCCESS==twRC)
                {
                  //Perform a File transfer
                  twRC = DSM_Entry(DG_IMAGE, DAT_IMAGEFILEXFER, MSG_GET, NULL, &m_twSourceIdentity);
                  if((TWRC_XFERDONE==twRC)||(TWRC_SUCCESS==twRC))
                  {
                    //Notify the derived class
                    OnFileTransfer(twSetupFileXfer, bAbort);
                    if(TWRC_SUCCESS==twRC)
                    {
                      TraceMessage("*** Note: DS responded with invalid TWRC_SUCCESS for DAT_IMAGEFILEXFER/MSG_GET Expected: TWRC_XFERDONE ***");
                      twRC = TWRC_XFERDONE;
                    }
                  }
                }
              }
            break;
          case TWSX_MEMORY:
          case TWSX_MEMFILE:
            {
              if(6==m_uiState)
              {
                //Find out how we are supposed to setup the memory
                twRC = DSM_Entry(DG_CONTROL, DAT_SETUPMEMXFER, MSG_GET, &twSetupMemXfer, &m_twSourceIdentity);
                if(TWRC_SUCCESS==twRC)
                {
                  //Allocate the memory
                  twImageMemXfer.Memory.Flags = TWMF_APPOWNS|TWMF_POINTER;
                  twImageMemXfer.Memory.Length = twSetupMemXfer.Preferred;
                  twImageMemXfer.Memory.TheMem = new BYTE[twSetupMemXfer.Preferred];
                }
              }
              if(6<=m_uiState)
              {
                //perform a Memory transfer
                twRC = DSM_Entry(DG_IMAGE, DAT_IMAGEMEMXFER, MSG_GET, &twImageMemXfer, &m_twSourceIdentity);
              }
              if((TWRC_XFERDONE==twRC)||(TWRC_SUCCESS==twRC))
              {
                if(TWSX_MEMORY==twXferMech)
                {
                  //Notify the derived class that a memory transfer occured
                  OnMemoryTransfer(twInfo, twImageMemXfer, twRC, bAbort);
                }
                else if(TWSX_MEMFILE==twXferMech)
                {
                  //Notify the derived class that a file in memory transfer occured
                  OnFileMemTransfer(twImageMemXfer, twRC, bAbort);
                }

                if(TWRC_XFERDONE==twRC)
                {
                  //finished, discard the memory used
                  delete [] reinterpret_cast<BYTE*>(twImageMemXfer.Memory.TheMem);
                  twImageMemXfer.Memory.TheMem = NULL;
                }
              }
            }
            break;
        }
      }while(TWRC_SUCCESS==twRC);
    }
    
    if(7==m_uiState)
    {
      //signal the derived class
      OnImageEnd(twInfo, twXferMech);
      //transition a state
      TW_PENDINGXFERS twPendingXfers = {0};
      twRC = DSM_Entry(DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, &twPendingXfers, &m_twSourceIdentity);
      if(TWRC_SUCCESS==twRC)
      {
        //signal the derived class
        OnEndXfer(twPendingXfers);
      }
    }
  }
  return twRC;
}

bool CTWAINSession::ReturnToState(TW_UINT16 twDesiredState)
{
  bool bAssumeHigher = true;
  //Reduce state until we reach 5
  while(twDesiredState<m_uiState)
  {
    if(TWRC_SUCCESS!=DownOneState())
    {
      if(TWCC_SEQERROR == m_twLastStatus.ConditionCode)
      {
        if(7==m_uiState)
        {
          //current state must be lower, not higher
          bAssumeHigher = false;
        }

        if(bAssumeHigher)
        {
          //assume actual state is higher, increase the state
          m_uiState = min(7, m_uiState+1);
        }
        else
        {
          //assume the actual state is lower, decrement the state
          m_uiState = max(1, m_uiState-1);
        }
      }
      else
      {
        #pragma message("TODO: Consider treating all errors like Sequence Errors")
        //non-seq errors are invalid failures, just decrement the state
        m_uiState = max(1, m_uiState-1);
      }
    }
  }
  //success depends on reaching State 5
  return (twDesiredState==m_uiState)?true:false;
}

void CTWAINSession::OnSignalDSRequest()
{
  //Default implementation does nothing
  return;
}

void CTWAINSession::SignalDSRequest(TW_UINT16 twMsg)
{
  TraceMessage("Queueing Data Source Signal: %s", convertMessage_toString(twMsg));
  TW_UINT16 twPrevState = m_uiState;
  //Define the scope of the critical section
  {
    CAutoCriticalSection acsObject(&m_csProtectRequestQueue);
    if(MSG_XFERREADY==twMsg)
    {
      m_uiState = 6;
    }
    //Put the request on the queue
    m_twDSRequestQueue.push(twMsg);
  }

  if(twPrevState!=m_uiState)
  {
    //state transition was detected
    TraceMessage("* State transition %d -> %d *", twPrevState, m_uiState);
  }
  //Signal parent
  OnSignalDSRequest();
  return;
}

TW_UINT16 FAR PASCAL CTWAINSession::DSMCallback(pTW_IDENTITY pOrigin, pTW_IDENTITY pDest, TW_UINT32 uiDG, TW_UINT16 uiDAT, TW_UINT16 uiMSG, TW_MEMREF pData)
{
  //Callback has been issued
  TW_UINT16 twRC = TWRC_FAILURE;
  //Look for our instance information
  if(m_mapInstance.end()!=m_mapInstance.find(reinterpret_cast<TW_UINT32>(pData)))
  {
    //Class has been found so dispatch the DataSource request
    twRC = TWRC_SUCCESS;
    m_mapInstance[(TW_UINT32)pData]->SignalDSRequest(uiMSG);
  }
  //Return the result
  return twRC;
}

void CTWAINSession::TraceMessage(const char *pszFormatString, ...)
{
  //setup the variable argument list
  va_list args;
  va_start(args, pszFormatString);

  int nCount = _vscprintf(pszFormatString, args ) + 1;
  //allocate the fromatted string
  char *pszFormatted = new char[nCount];
  if(pszFormatted)
  {  
    //format the string
    vsprintf_s(pszFormatted, nCount, pszFormatString, args);
    //Output the result
    OutputDebugStringA(pszFormatted);
    //cleanup local pointers
    delete [] pszFormatted;
    pszFormatted = NULL;
  }
  return;
}

TW_UINT16 CTWAINSession::DownOneState()
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_PENDINGXFERS twXfer = {0};
  switch(m_uiState)
  {
    case 2:
      //this should bring us to State 1
      twRC = FreeDSM();
      break;
    case 3:
      //this should bring us to State 2
      twRC = CloseDSM();
      break;
    case 4:
      //this should bring us to State 3
      twRC = CloseDS();
      break;
    case 5:
      //this should bring us to State 4
      twRC = DisableDS();
      break;
    case 6:
      //this should bring us to State 5
      twRC = DSM_Entry(DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, &twXfer, &m_twSourceIdentity);
      break;
    case 7:
      //this should bring us to State 6
      TW_PENDINGXFERS twXfer = {0};
      twRC = DSM_Entry(DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, &twXfer, &m_twSourceIdentity);
      break;
  }
  return twRC;
}

TW_HANDLE CTWAINSession::AllocateContainer(TW_UINT16 twConType, TW_UINT16 twItemType, TW_UINT32 twCount/*=1*/)
{
  TW_HANDLE hResult = NULL;
  //Allocate memory of the appropriate size
  switch(twConType)
  {
    case TWON_ONEVALUE:
      switch(twItemType)
      {
        default:
          //these fit in the TW_ONEVALUE size
          hResult = _DSM_Alloc(sizeof(TW_ONEVALUE));
          break;
        case TWTY_FRAME:
        case TWTY_STR32:
        case TWTY_STR64:
        case TWTY_STR128:
        case TWTY_STR255:
          //these are too big to fit in a simple TW_ONEVALUE
          hResult = _DSM_Alloc(sizeof(TW_ONEVALUE) + getTWTYsize(twItemType));
          break;
      }
      break;
    case TWON_ARRAY:
      hResult = _DSM_Alloc(sizeof(TW_ARRAY) + (twCount * getTWTYsize(twItemType)));
      break;
    case TWON_ENUMERATION:
      hResult = _DSM_Alloc(sizeof(TW_ENUMERATION) + (twCount * getTWTYsize(twItemType)));
      break;
    case TWON_RANGE:
      //All ranges are the same size Note:(sizeof(TW_FIX32)==sizeof(TW_UINT32))
      hResult = _DSM_Alloc(sizeof(TW_RANGE));
      break;
  }
  
  if(hResult)
  {
    //Leverage the container similarities to simplify initialization, obtain a pointer
    pTW_ONEVALUE pOne = static_cast<pTW_ONEVALUE>(_DSM_LockMemory(hResult));
    if(pOne)
    {
      //First item in all containers is the "Type"
      pOne->ItemType = twItemType;
      switch(twConType)
      {
        case TWON_ARRAY:
        case TWON_ENUMERATION:
        {
          //For multi-value containers the second item is always the "NumItems"
          pTW_ARRAY pArray = reinterpret_cast<pTW_ARRAY>(pOne);
          pArray->NumItems = twCount;
          pArray = NULL;
        }
      }
      //Unlock the memory
      _DSM_UnlockMemory(hResult);
      pOne = NULL;
    }
  }

  return hResult;
}

bool CTWAINSession::FillCapability(TW_UINT16 twCapId, TW_UINT16 twConType, TW_UINT16 twItemType, TW_CAPABILITY &twCap, TW_UINT32 twCount/*=1*/)
{
  //Setup the capability ID
  twCap.Cap = twCapId;
  //Setup the Container type
  twCap.ConType = twConType;
  //Allocate and prep the container
  twCap.hContainer = AllocateContainer(twConType, twItemType, twCount);
  //Success depends on successful creation of the container
  return (NULL!=twCap.hContainer)?true:false;
}

TW_UINT16 CTWAINSession::SetCapabilityOneValue(TW_UINT16 twCapId, const bool &bValue)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_CAPABILITY twCap = {0};
  TW_BOOL twVal = (true==bValue)?TRUE:FALSE;

  //Prepare the capability structure
  if(FillCapability(twCapId, TWON_ONEVALUE, TWTY_BOOL, twCap))
  {
    //Delegate to the next level
    twRC = SetCapabilityAnyOneValue(twCap, &twVal);
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapabilityOneValue(TW_UINT16 twCapId, const TW_INT16 &twVal)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_CAPABILITY twCap = {0};
  //Prepare the capability structure
  if(FillCapability(twCapId, TWON_ONEVALUE, TWTY_INT16, twCap))
  {
    //Delegate to the next level
    twRC = SetCapabilityAnyOneValue(twCap, &twVal);
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapabilityOneValue(TW_UINT16 twCapId, const TW_UINT16 &twVal)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_CAPABILITY twCap = {0};
  //Prepare the capability structure
  if(FillCapability(twCapId, TWON_ONEVALUE, TWTY_UINT16, twCap))
  {
    //Delegate to the next level
    twRC = SetCapabilityAnyOneValue(twCap, &twVal);
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapabilityOneValue(TW_UINT16 twCapId, const TW_UINT32 &twVal)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_CAPABILITY twCap = {0};
  //Prepare the capability structure
  if(FillCapability(twCapId, TWON_ONEVALUE, TWTY_UINT32, twCap))
  {
    //Delegate to the next level
    twRC = SetCapabilityAnyOneValue(twCap, &twVal);
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapabilityOneValue(TW_UINT16 twCapId, const TW_FIX32 &twVal)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_CAPABILITY twCap = {0};
  //Prepare the capability structure
  if(FillCapability(twCapId, TWON_ONEVALUE, TWTY_FIX32, twCap))
  {
    //Delegate to the next level
    twRC = SetCapabilityAnyOneValue(twCap, &twVal);
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapabilityOneValue(TW_UINT16 twCapId, const TW_FRAME &twVal)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  TW_CAPABILITY twCap = {0};
  //Prepare the capability structure
  if(FillCapability(twCapId, TWON_ONEVALUE, TWTY_FRAME, twCap))
  {
    //Delegate to the next level
    twRC = SetCapabilityAnyOneValue(twCap, &twVal);
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapabilityAnyOneValue(TW_CAPABILITY &twCap, const void *pValue)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  
  pTW_ONEVALUE pOne = static_cast<pTW_ONEVALUE>(_DSM_LockMemory(twCap.hContainer));
  if(pOne)
  {
    //Fill in the one value
    memcpy(&pOne->Item, pValue, getTWTYsize(pOne->ItemType));
    _DSM_UnlockMemory(twCap.hContainer);
    //Call the base implementation
    twRC = SetCapability(twCap);
    //Free the memory
    _DSM_Free(twCap.hContainer);
    twCap.hContainer = NULL;
  }
  return twRC;
}

TW_UINT16 CTWAINSession::SetCapability(TW_CAPABILITY &twCap)
{
  return DSM_Entry(DG_CONTROL, DAT_CAPABILITY, MSG_SET, &twCap, &m_twSourceIdentity);
}

TW_UINT16 CTWAINSession::GetCapabilityOneValue(TW_UINT16 twCapId, bool &bValue, TW_UINT16 twMsg/*=MSG_GETCURRENT*/)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  TW_BOOL twValue = FALSE;
  TW_UINT16 twRC = GetAnyCapabilityOneValue(twCap, &twValue, twMsg);
  if(TWRC_SUCCESS==twRC)
  {
    bValue = (TRUE==twValue)?true:false;
  }
  return twRC;
}

TW_UINT16 CTWAINSession::GetCapabilityOneValue(TW_UINT16 twCapId, TW_INT16 &twValue, TW_UINT16 twMsg/*=MSG_GETCURRENT*/)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  return GetAnyCapabilityOneValue(twCap, &twValue, twMsg);
}

TW_UINT16 CTWAINSession::GetCapabilityOneValue(TW_UINT16 twCapId, TW_UINT16 &twValue, TW_UINT16 twMsg/*=MSG_GETCURRENT*/)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  return GetAnyCapabilityOneValue(twCap, &twValue, twMsg);
}

TW_UINT16 CTWAINSession::GetCapabilityOneValue(TW_UINT16 twCapId, TW_UINT32 &twValue, TW_UINT16 twMsg/*=MSG_GETCURRENT*/)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  return GetAnyCapabilityOneValue(twCap, &twValue, twMsg);
}

TW_UINT16 CTWAINSession::GetCapabilityOneValue(TW_UINT16 twCapId, TW_FIX32 &twValue, TW_UINT16 twMsg/*=MSG_GETCURRENT*/)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  return GetAnyCapabilityOneValue(twCap, &twValue, twMsg);
}

TW_UINT16 CTWAINSession::GetCapabilityOneValue(TW_UINT16 twCapId, TW_FRAME &twValue, TW_UINT16 twMsg/*=MSG_GETCURRENT*/)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  return GetAnyCapabilityOneValue(twCap, &twValue, twMsg);
}

TW_UINT16 CTWAINSession::GetAnyCapabilityOneValue(TW_CAPABILITY twCap, void *pValue, TW_UINT16 twMsg)
{
  TW_UINT16 twRC = TWRC_FAILURE;
  if(TWRC_SUCCESS==GetCapability(twCap, twMsg))
  {
    switch(twCap.ConType)
    {
      case TWON_ONEVALUE:
        {
          pTW_ONEVALUE pOne = static_cast<pTW_ONEVALUE>(_DSM_LockMemory(twCap.hContainer));
          if(pOne)
          {
            switch(pOne->ItemType)
            {
              //note: for all sizeof <= TW_UINT32 casting is an effective cross platform technique
              case TWTY_INT8:
                *static_cast<pTW_INT8>(pValue) = static_cast<TW_INT8>(pOne->Item);
                break;
              case TWTY_INT16:
                *static_cast<pTW_INT16>(pValue) = static_cast<TW_INT16>(pOne->Item);
                break;
              case TWTY_INT32:
                *static_cast<pTW_INT32>(pValue) = static_cast<TW_INT32>(pOne->Item);
                break;
              case TWTY_UINT8:
                *static_cast<pTW_UINT8>(pValue) = static_cast<TW_UINT8>(pOne->Item);
                break;
              case TWTY_UINT16:
                *static_cast<pTW_UINT16>(pValue) = static_cast<TW_UINT16>(pOne->Item);
                break;
              case TWTY_UINT32:
                *static_cast<pTW_UINT32>(pValue) = static_cast<TW_UINT32>(pOne->Item);
                break;
              case TWTY_BOOL:
                *static_cast<pTW_BOOL>(pValue) = static_cast<TW_BOOL>(pOne->Item);
                break;
              default:
                //note: for all sizeof > TW_UINT32 copying is the best extraction technique
                memcpy(pValue, &pOne->Item, getTWTYsize(pOne->ItemType));
                break;
            }
            twRC = TWRC_SUCCESS;
            //release the memory
            _DSM_UnlockMemory(twCap.hContainer);
            pOne = NULL;
          }
        }
        break;
      case TWON_RANGE:
        {
          pTW_RANGE pRange = static_cast<pTW_RANGE>(_DSM_LockMemory(twCap.hContainer));
          if(pRange)
          {
            TW_UINT32 twInterim = 0;
            twRC = TWRC_SUCCESS;
            switch(twMsg)
            {
              case MSG_RESET:
              case MSG_GETCURRENT:
                twInterim = pRange->CurrentValue;
                break;
              case MSG_GETDEFAULT:
                twInterim = pRange->DefaultValue;
                break;
              default:
                //note: for all sizeof > TW_UINT32 copying is the best extraction technique
                twRC = TWRC_FAILURE;
                break;
            }

            if(TWRC_SUCCESS==twRC)
            {
              twRC = TWRC_SUCCESS;
              switch(pRange->ItemType)
              {
                //note: for all sizeof <= TW_UINT32 casting is an effective cross platform technique
                case TWTY_INT8:
                  *static_cast<pTW_INT8>(pValue) = static_cast<TW_INT8>(twInterim);
                  break;
                case TWTY_INT16:
                  *static_cast<pTW_INT16>(pValue) = static_cast<TW_INT16>(twInterim);
                  break;
                case TWTY_INT32:
                  *static_cast<pTW_INT32>(pValue) = static_cast<TW_INT32>(twInterim);
                  break;
                case TWTY_UINT8:
                  *static_cast<pTW_UINT8>(pValue) = static_cast<TW_UINT8>(twInterim);
                  break;
                case TWTY_UINT16:
                  *static_cast<pTW_UINT16>(pValue) = static_cast<TW_UINT16>(twInterim);
                  break;
                case TWTY_UINT32:
                  *static_cast<pTW_UINT32>(pValue) = static_cast<TW_UINT32>(twInterim);
                  break;
                case TWTY_BOOL:
                  *static_cast<pTW_BOOL>(pValue) = static_cast<TW_BOOL>(twInterim);
                  break;
                default:
                  //note: for all sizeof > TW_UINT32 copying is the best extraction technique
                  twRC = TWRC_FAILURE;
                  break;
              }
            }
            //release the memory
            _DSM_UnlockMemory(twCap.hContainer);
            pRange = NULL;
          }
        }
        break;
      case TWON_ENUMERATION:
        {
          pTW_ENUMERATION pEnum = static_cast<pTW_ENUMERATION>(_DSM_LockMemory(twCap.hContainer));
          if(pEnum)
          {
            TW_UINT32 twIndex = 0;
            twRC = TWRC_SUCCESS;
            switch(twMsg)
            {
              case MSG_RESET:
              case MSG_GETCURRENT:
                twIndex = pEnum->CurrentIndex;
                break;
              case MSG_GETDEFAULT:
                twIndex = pEnum->DefaultIndex;
                break;
              default:
                //note: for all sizeof > TW_UINT32 copying is the best extraction technique
                twRC = TWRC_FAILURE;
                break;
            }

            if(TWRC_SUCCESS==twRC)
            {
              //copy the result from the array
              memcpy(pValue, &pEnum->ItemList[getTWTYsize(pEnum->ItemType) * twIndex], getTWTYsize(pEnum->ItemType));
            }
            //release the memory
            _DSM_UnlockMemory(twCap.hContainer);
            pEnum = NULL;
          }
        }
        break;
      //note: array does not have concept of a one value 'current' or 'default'
    }
    //Free the container
    _DSM_Free(twCap.hContainer);
    twCap.hContainer = NULL;
  }
  return twRC;
}

TW_UINT16 CTWAINSession::GetCapabilitySupported(TW_UINT16 twCapId, TW_HANDLE &hContainer, TW_UINT16 &twConType)
{
  TW_CAPABILITY twCap =
  {
    twCapId,
    TWON_DONTCARE16,
    NULL,
  };
  TW_UINT16 twRC = GetCapability(twCap);
  if(TWRC_SUCCESS==twRC)
  {
    hContainer = twCap.hContainer;
    twConType = twCap.ConType;
  }
  return twRC;
}

TW_UINT16 CTWAINSession::GetCapability(TW_CAPABILITY &twCap, TW_UINT16 twMsg/*=MSG_GET*/)
{
  return DSM_Entry(DG_CONTROL, DAT_CAPABILITY, twMsg, &twCap, &m_twSourceIdentity);
}
