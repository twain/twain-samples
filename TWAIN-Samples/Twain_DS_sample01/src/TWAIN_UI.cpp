/***************************************************************************
* Copyright � 2007 TWAIN Working Group:  
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
* @file CTWAINDS_FreeImage.cpp
* The main Data Source class.
* This class is derived from the Base class describing a TWAIN DS.
* @author TWAIN Working Group
* @date April 2007
*/
#include "CommonDS.h"
#include "CTWAINDS_FreeImage.h"
#include "TWAIN_UI.h"
#include <typeinfo>

#ifdef TWH_CMP_GNU
  #include <unistd.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <dirent.h>
  #include <pwd.h>
  #define MAX_PATH 1024
#elif defined(TWNDS_OS_WIN)
  #include <shlobj.h>
#else

#endif

CTWAIN_UI::CTWAIN_UI(CTWAINDS_FreeImage *pDS)
{
  m_pDS = pDS;
  m_bScanning=false;
  memset(&m_EnableDSdata,0,sizeof(TW_USERINTERFACE));
  m_strProfilesDirectory=="";

  char strProfilesPath[MAX_PATH];
  char strOldPath[MAX_PATH];
#ifdef TWH_CMP_GNU
  getcwd(strOldPath,MAX_PATH);

  strcpy(strProfilesPath , getenv ("HOME"));
#elif defined(TWNDS_OS_WIN)
  GetCurrentDirectory(MAX_PATH, strOldPath);
  // get temp Application data directory for all users
  if(!SHGetSpecialFolderPath(NULL, strProfilesPath, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, TRUE)) 
  {
    return;
  }
#else

#endif
  
  if(strProfilesPath[strlen(strProfilesPath)-1] != '/')
  {
    SSTRCAT(strProfilesPath, MAX_PATH, "/");
  }

  SSTRCAT(strProfilesPath, MAX_PATH, PROFILELOCATION);
  if(strProfilesPath[strlen(strProfilesPath)-1] != '/')
  {
    SSTRCAT(strProfilesPath, MAX_PATH, "/");
  }
  SSTRCAT(strProfilesPath, MAX_PATH, kCUSTOMDSGUI);
  if(strProfilesPath[strlen(strProfilesPath)-1] != '/')
  {
    SSTRCAT(strProfilesPath, MAX_PATH, "/");
  }

  // Set current directory to profiles directory 
#ifdef TWH_CMP_GNU
  if(chdir(strProfilesPath))
#elif defined(TWNDS_OS_WIN)
  if(!SetCurrentDirectory(strProfilesPath))
#else

#endif
  {
    char     szPath[MAX_PATH];
    char     szTempPath[MAX_PATH];
    char     *pPath;
    char     *pSeparator;

    SSTRCPY(szPath, MAX_PATH, strProfilesPath);
    pPath    = szPath;
    // creates profiles path if it is needed
    while( pSeparator = strchr(pPath, '/') )
    {
      *pSeparator = '\0';
      SSTRCPY(szTempPath, MAX_PATH, pPath);
      SSTRCAT(szTempPath, MAX_PATH, "/");

#ifdef TWH_CMP_GNU
      if(chdir(szTempPath))
#elif defined(TWNDS_OS_WIN)
      if(!SetCurrentDirectory(szTempPath))
#else

#endif
      {
#ifdef TWH_CMP_GNU
        if(mkdir(szTempPath,S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH))
#elif defined(TWNDS_OS_WIN)
        if( !CreateDirectory(szTempPath, NULL) )
#else

#endif
        {
          return;
        }
#ifdef TWH_CMP_GNU
        chmod(szTempPath,S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
        if(chdir(szTempPath))
#elif defined(TWNDS_OS_WIN)
        if(!SetCurrentDirectory(szTempPath))
#else

#endif
        {
          return;
        }
      }
      pPath = pSeparator+1;
    }
  }

  m_strProfilesDirectory = strProfilesPath;

#ifdef TWH_CMP_GNU
  chdir(strOldPath);
#elif defined(TWNDS_OS_WIN)
  SetCurrentDirectory(strOldPath);
#else

#endif
}

TW_INT16 CTWAIN_UI::DisplayTWAINGUI(TW_USERINTERFACE Data, bool bSetup, bool bIndicators)
{
  m_bIndicators = bIndicators;
  m_bSetup = bSetup;
  m_EnableDSdata = Data;
  m_bScanning =(Data.ShowUI==0);
  return 0;
}
void CTWAIN_UI::DestroyTWAINGUI()
{
  m_bScanning=false;
  memset(&m_EnableDSdata,0,sizeof(TW_USERINTERFACE));
}
void CTWAIN_UI::UpdateProgress(bool bShow, unsigned char ucProgress, unsigned int unPageNo, string strProgressTitle)
{
  ;
}
void CTWAIN_UI::Scan()
{
  if(m_pDS->StartScanning())
  {
    m_bScanning = m_pDS->DoXferReadyEvent();
  }
}
void CTWAIN_UI::Cancel()
{
  if(m_bScanning)
  {
    m_pDS->StopScanning();
  }
  m_pDS->DoCloseDSRequestEvent();
}

void CTWAIN_UI::Save()
{
  m_pDS->DoCloseDSOkEvent();
}
unsigned int CTWAIN_UI::MessageBox(string strMessage,string strTitle, unsigned int unIconID)
{
  return 0;
}

bool CTWAIN_UI::processEvent(pTW_EVENT _pEvent)
{
  return false;
}

int CTWAIN_UI::GetCurrentCapIndex(const TW_UINT16 _unCap)
{
  CTWAINContainer  *pfCap = m_pDS->findCapability(_unCap);
  if(pfCap==NULL)
  {
    assert(0);
    return 0;//@TODO
  }

  return pfCap->m_nCurrent;
}

TW_FRAME CTWAIN_UI::GetCurrentFrame()
{
  CTWAINContainerFrame  *pfCap = (CTWAINContainerFrame  *)m_pDS->findCapability(ICAP_FRAMES);
  if(pfCap==NULL)
  {
    assert(0);
    return TW_FRAME();//@TODO
  }
  InternalFrame frame = pfCap->m_listFrames.at(pfCap->m_nCurrent);
  int   unit = TWUN_PIXELS;
  float Xres = 100;
  float Yres = 100;
  m_pDS->getCurrentUnits(unit, Xres, Yres);
  return frame.AsTW_FRAME(unit, Xres, Yres);
}

const IntVector* CTWAIN_UI::GetValidCap(const TW_UINT16 _unCap)
{
  CTWAINContainer  *pfCap = m_pDS->findCapability(_unCap);
  if(pfCap==NULL)
  {
    assert(0);
    return 0;//@TODO
  }
  if(typeid(*pfCap) == typeid(CTWAINContainerBool))
  {
    CTWAINContainerBool *pfBoolCap = (CTWAINContainerBool*)pfCap;
    return &pfBoolCap->m_listBools;
  }
  else if(typeid(*pfCap) == typeid(CTWAINContainerInt))
  {
    CTWAINContainerInt *pfIntCap = (CTWAINContainerInt*)pfCap;
    return &pfIntCap->m_listInts;
  } 
  assert(0);
  return 0;
}

const FloatVector* CTWAIN_UI::GetValidCapFloat(const TW_UINT16 _unCap)
{
  CTWAINContainer  *pfCap = m_pDS->findCapability(_unCap);
  if(pfCap==NULL)
  {
    assert(0);
    return 0;//@TODO
  }
  if(typeid(*pfCap) == typeid(CTWAINContainerFix32))
  {
    CTWAINContainerFix32 *pfFix32Cap = (CTWAINContainerFix32*)pfCap;
    return &pfFix32Cap->m_listFloats;
  }
  assert(0);
  return 0;
}

float CTWAIN_UI::GetCapValueFloat(const TW_UINT16 _unCap)
{
  CTWAINContainer  *pfCap = m_pDS->findCapability(_unCap);
  if(pfCap==NULL)
  {
    assert(0);
    return 0;//@TODO
  }
  
  float fVal;
  if(typeid(*pfCap) == typeid(CTWAINContainerFix32))
  {
    CTWAINContainerFix32 *pfFix32Cap = (CTWAINContainerFix32*)pfCap;
    if(pfFix32Cap->GetCurrent(fVal))
    {
      return fVal;
    }
  }
  else if(typeid(*pfCap) == typeid(CTWAINContainerFix32Range))
  {
    CTWAINContainerFix32Range *pfFix32Cap = (CTWAINContainerFix32Range*)pfCap;
    if(pfFix32Cap->GetCurrent(fVal))
    {
      return fVal;
    }
  }  
  assert(0);
  return 0;
}

bool CTWAIN_UI::SetCurrentFrame(float fTop, float fLeft, float fBottom, float fRight)
{
  TW_CAPABILITY cap;
  cap.Cap = ICAP_FRAMES;
  cap.ConType = TWON_ONEVALUE;
  cap.hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE_FRAME));
  TW_INT16 twRC;  
  if(0 != cap.hContainer)
  {
    TW_ONEVALUE_FRAME* pCap = (TW_ONEVALUE_FRAME*)_DSM_LockMemory(cap.hContainer);
    pCap->ItemType = TWTY_FRAME;
    pCap->Item.Top = FloatToFIX32(fTop);
    pCap->Item.Left = FloatToFIX32(fLeft);
    pCap->Item.Bottom = FloatToFIX32(fBottom);
    pCap->Item.Right = FloatToFIX32(fRight);

    _DSM_UnlockMemory(cap.hContainer);
    twRC = m_pDS->dat_capability(MSG_SET,&cap);
    _DSM_Free(cap.hContainer);
  }
  else
  {
    return false;
  }

  m_pDS->setConditionCode(TWCC_SUCCESS); // clear condition code 
  if(twRC==TWRC_SUCCESS || (twRC==TWRC_CHECKSTATUS))
  {
    return true;
  }

  return false;
}

bool CTWAIN_UI::GetCapRangeFloat(const TW_UINT16 _unCap, float &fMin, float &fMax, float &fStep)
{
  CTWAINContainer  *pfCap = m_pDS->findCapability(_unCap);
  if(pfCap==NULL)
  {
    return false;
  }
  if(typeid(*pfCap) == typeid(CTWAINContainerFix32Range))
  {
    CTWAINContainerFix32Range *pfFloatCap = (CTWAINContainerFix32Range*)pfCap;
    pfFloatCap->GetMinMaxStep(fMin, fMax, fStep);
    return true;
  } 
  return false;
}

bool CTWAIN_UI::SetCapValueFloat(const TW_UINT16 _unCap, float fVal)
{
  TW_CAPABILITY cap;
  cap.Cap = _unCap;
  cap.ConType = TWON_ONEVALUE;
  cap.hContainer = _DSM_Alloc(sizeof(TW_FRAME));
  TW_INT16 twRC;  
  if(0 != cap.hContainer)
  {
    TW_ONEVALUE_FIX32* pCap = (TW_ONEVALUE_FIX32*)_DSM_LockMemory(cap.hContainer);
    pCap->ItemType = TWTY_FIX32;
    pCap->Item = FloatToFIX32(fVal);
    _DSM_UnlockMemory(cap.hContainer);
    twRC = m_pDS->dat_capability(MSG_SET,&cap);
    _DSM_Free(cap.hContainer);
  }
  else
  {
    return false;
  }

  m_pDS->setConditionCode(TWCC_SUCCESS); // clear condition code 
  if(twRC==TWRC_SUCCESS || (twRC==TWRC_CHECKSTATUS))
  {
    return true;
  }

  return false;
}

bool CTWAIN_UI::SetCapValueInt(const TW_UINT16 _unCap, int nVal)
{
  CTWAINContainer *pCapCont = m_pDS->findCapability(_unCap);

  if(pCapCont ==0)
  {
    return false;
  }

  TW_CAPABILITY cap;
  cap.Cap = _unCap;
  cap.ConType = TWON_ONEVALUE;
  cap.hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE));
  TW_INT16 twRC;  
  if(0 != cap.hContainer)
  {
    TW_ONEVALUE* pCap = (TW_ONEVALUE*)_DSM_LockMemory(cap.hContainer);
    pCap->ItemType = pCapCont->GetItemType();
    pCap->Item = nVal;
    _DSM_UnlockMemory(cap.hContainer);
    twRC = m_pDS->dat_capability(MSG_SET,&cap);
    _DSM_Free(cap.hContainer);
  }
  else
  {
    return false;
  }

  m_pDS->setConditionCode(TWCC_SUCCESS); // clear condition code 
  if(twRC==TWRC_SUCCESS || (twRC==TWRC_CHECKSTATUS))
  {
    return true;
  }

  return false;
}

bool CTWAIN_UI::ResetAllCaps()
{
  TW_CAPABILITY cap;
  cap.Cap = CAP_SUPPORTEDCAPS;
  cap.ConType = TWON_DONTCARE16;
  cap.hContainer = 0;
  TW_INT16 twRC;  
  twRC = m_pDS->dat_capability(MSG_RESETALL,&cap);
  m_pDS->setConditionCode(TWCC_SUCCESS); // clear condition code 
  if(twRC==TWRC_SUCCESS || (twRC==TWRC_CHECKSTATUS))
  {
    return true;
  }

  return false;
}

#ifdef TWH_CMP_GNU
void CTWAIN_UI::TW_GetAllProfiles(lstString& strFileNames)
{
  strFileNames.clear();
  // Set current directory to profiles directory 
  DIR *dir;
  dirent *dirInfo;
  if((dir  = opendir(m_strProfilesDirectory.c_str())) == NULL)
  {
    return;
  }
  TW_CUSTOMDSDATA DSDataBackUp;
  if(m_pDS->GetGustomDSData(&DSDataBackUp)!=TWRC_SUCCESS)
  {
    return ;
  }

  while(dirInfo=readdir(dir)) 
  {
    string strName = dirInfo->d_name;
    if(strName.size()>strlen(FILEEXTENTION) && strName.substr(strName.size()-strlen(FILEEXTENTION),strlen(FILEEXTENTION))==FILEEXTENTION)
    {
      strName = strName.substr(0,strName.size()-strlen(FILEEXTENTION));
      try
      {
        // check if profile is valid
        if(TW_LoadProfileFromFile(strName))
        {
          strFileNames.push_back(strName);
        }
      }
      catch(...)
      {
      }

    }
  }

  m_pDS->SetGustomDSData(&DSDataBackUp);

  closedir(dir);
}
#elif defined(TWNDS_OS_WIN)
void CTWAIN_UI::TW_GetAllProfiles(lstString& strFileNames)
{
  char strOldPath[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, strOldPath);

  strFileNames.clear();
  // Set current directory to profiles directory 
  if(!SetCurrentDirectory(m_strProfilesDirectory.c_str()))
  {
    return;
  }
  TW_CUSTOMDSDATA DSDataBackUp;
  if(m_pDS->GetGustomDSData(&DSDataBackUp)!=TWRC_SUCCESS)
  {
    return ;
  }

  WIN32_FIND_DATA FindFileData;
  HANDLE          hFind   = NULL;
  char           szFileName[MAX_PATH];
  char           *pDot;
  
  hFind = FindFirstFile("*" FILEEXTENTION, &FindFileData);
  while(hFind != INVALID_HANDLE_VALUE)
  {
    SSTRCPY(szFileName, MAX_PATH, FindFileData.cFileName);
    pDot = strchr(szFileName, '.');
    if(pDot)
    {
      *pDot = '\0';
    }

    try
    {
      // check if profile is valid
      if(TW_LoadProfileFromFile(szFileName))
      {
        strFileNames.push_back(szFileName);
      }
    }
    catch(...)
    {
    }
    if(!FindNextFile(hFind, &FindFileData))
    {
      FindClose(hFind);
      break;
    }
  }

  m_pDS->SetGustomDSData(&DSDataBackUp);
  SetCurrentDirectory(strOldPath);
}
#else

#endif

bool CTWAIN_UI::TW_LoadProfileFromFile(string strFileName)
{
  TW_HANDLE hData;
  strFileName  = m_strProfilesDirectory+strFileName+FILEEXTENTION;
  FILE *pFile = NULL;
  //open file
  FOPEN(pFile, strFileName.c_str(), "rb");
  if(pFile==0)
  {
    return false;
  }
  // get file size
  fseek(pFile, 0, SEEK_END);
  DWORD dwDataSize = (DWORD)ftell(pFile);
  rewind(pFile);
  bool bRes = true;
  // it has to contains at least DS identity
  if(dwDataSize==0)
  {
    fclose(pFile);
    return false;
  }
  // allocate storage and read CustomDSdata
  void * pData = NULL;
  if(hData = _DSM_Alloc(dwDataSize))
  {
    pData = _DSM_LockMemory(hData);
    if(pData)
    {
      if(fread(pData, dwDataSize, 1, pFile)!=1)
      {
        bRes = false;
      }
    }
    else
    {
      bRes = false;
    }
  }
  else
  {
    bRes = false;
  }
  fclose(pFile);
  
  if(hData)
  {
    _DSM_UnlockMemory(hData);
    if(bRes)
    {
      TW_CUSTOMDSDATA DSData;
      DSData.hData = hData;
      DSData.InfoLength = dwDataSize;
      bRes = m_pDS->SetGustomDSData(&DSData)==TWRC_SUCCESS;
    }
    // free resource on error
    _DSM_Free(hData);
  }
  
  return bRes;
}

bool CTWAIN_UI::TW_SaveProfileToFile(string strFileName)
{
  strFileName  = m_strProfilesDirectory+strFileName+FILEEXTENTION;

  TW_CUSTOMDSDATA DSData;
  if(m_pDS->GetGustomDSData(&DSData)!=TWRC_SUCCESS)
  {
    return false;
  }
  void* pData = _DSM_LockMemory(DSData.hData);
  if(!pData)
  {
    _DSM_Free(DSData.hData);
    return false;
  }

  FILE *pFile  = NULL;
  bool  bError = false;
  //opens file
  FOPEN(pFile, strFileName.c_str(), "wb");
  if(pFile)
  {
    bError = true;
    //store CustomDSdata
    if(fwrite(pData, DSData.InfoLength, 1, pFile)!=1)
    {
      bError = false;
    }
    fclose(pFile);
    //remove file on error
    if(!bError)
    {
      int i = remove(strFileName.c_str());
    }
  }
  
  _DSM_UnlockMemory(DSData.hData);
  _DSM_Free(DSData.hData);

  return bError;
}

bool CTWAIN_UI::TW_DeleteProfile(string strFileName)
{
  if(!strFileName.empty())// if profile is selected
  {
    strFileName  = m_strProfilesDirectory+strFileName+FILEEXTENTION;

    //delete file from the disk
    return remove(strFileName.c_str())==0;
  }
  else
  {
    return false;
  }
}

