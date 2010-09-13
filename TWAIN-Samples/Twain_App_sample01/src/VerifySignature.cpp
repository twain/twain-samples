//-------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Example of verifying the embedded signature of a PE file by using 
// the WinVerifyTrust function.

#define _UNICODE 1
#define UNICODE 1

#ifdef _WINDOWS
  #include "stdafx.h"
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>

void OutputDebugStringFmt2(LPCTSTR lpOutputString, ...)
{
  //setup the variable argument list
  va_list args;
  va_start(args, lpOutputString);
  int nCount = _vsctprintf(lpOutputString, args ) + 1;

  //allocate the fromatted string
  TCHAR *pszFormatted = new TCHAR[nCount];
  if(pszFormatted)
  {  
    //format the string
    _vstprintf_s(pszFormatted, nCount, lpOutputString, args);
    //copy to the output buffer
    OutputDebugString(pszFormatted);
    //cleanup local pointers
    delete [] pszFormatted;
    pszFormatted = NULL;
  }
  return;
}

#ifndef TRACE
#ifdef _DEBUG
#define TRACE OutputDebugStringFmt2
#else //#ifdef _DEBUG
#define TRACE
#endif //#ifdef _DEBUG
#endif //#ifndef TRACE

// Link with the Wintrust.lib file.
#pragma comment (lib, "wintrust")

BOOL VerifyEmbeddedSignature( LPCWSTR pwszSourceFile )
{
  // Initialize the WINTRUST_FILE_INFO structure.
  WINTRUST_FILE_INFO  FileData;
  memset(&FileData, 0, sizeof(FileData));
  FileData.cbStruct       = sizeof(WINTRUST_FILE_INFO);
  FileData.pcwszFilePath  = pwszSourceFile;
  FileData.hFile          = NULL;
  FileData.pgKnownSubject = NULL;

  /*
  WVTPolicyGUID specifies the policy to apply on the file
  WINTRUST_ACTION_GENERIC_VERIFY_V2 policy checks:
  
  1) The certificate used to sign the file chains up to a root 
  certificate located in the trusted root certificate store. This 
  implies that the identity of the publisher has been verified by 
  a certification authority.
  
  2) In cases where user interface is displayed (which this example
  does not do), WinVerifyTrust will check for whether the  
  end entity certificate is stored in the trusted publisher store,  
  implying that the user trusts content from this publisher.
  
  3) The end entity certificate has sufficient permission to sign 
  code, as indicated by the presence of a code signing EKU or no 
  EKU.
  */
  GUID            WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

  // Initialize the WinVerifyTrust input data structure.
  WINTRUST_DATA   WinTrustData;
  memset(&WinTrustData, 0, sizeof(WinTrustData));       // Default all fields to 0.
  WinTrustData.cbStruct             = sizeof(WinTrustData);
  WinTrustData.pPolicyCallbackData  = NULL;             // Use default code signing EKU.
  WinTrustData.pSIPClientData       = NULL;             // No data to pass to SIP.
  WinTrustData.dwUIChoice           = WTD_UI_NOGOOD;    // Disable WVT UI.    //WTD_UI_NONE; WTD_UI_ALL; WTD_UI_NOGOOD; //NOGOOD will not display a dialog for no signature
  WinTrustData.fdwRevocationChecks  = WTD_REVOKE_NONE;  // No revocation checking.
  WinTrustData.dwUnionChoice        = WTD_CHOICE_FILE;  // Verify an embedded signature on a file.
  WinTrustData.dwStateAction        = 0;                // Default verification.
  WinTrustData.hWVTStateData        = NULL;             // Not applicable for default verification of embedded signature.
  WinTrustData.pwszURLReference     = NULL;             // Not used.
  WinTrustData.dwProvFlags          = WTD_SAFER_FLAG;   // Default.
  WinTrustData.dwUIContext          = 0;                // This is not applicable if there is no UI because it changes the UI to accommodate running applications instead of installing applications.
  WinTrustData.pFile                = &FileData;        // Set pFile.

  // WinVerifyTrust verifies signatures as specified by the GUID and Wintrust_Data.
  LONG  lStatus = WinVerifyTrust( NULL, &WVTPolicyGUID, &WinTrustData );

  switch (lStatus) 
  {
    case ERROR_SUCCESS:
      /*
      Signed file:
          - Hash that represents the subject is trusted.
          - Trusted publisher without any verification errors.
          - UI was disabled in dwUIChoice. No publisher or time stamp chain errors.
          - UI was enabled in dwUIChoice and the user clicked "Yes" when asked to run the signed subject.
      */
      TRACE( _T("The file \"%s\" is signed and the signature was verified.\n"), pwszSourceFile );
      break;
  
    case TRUST_E_NOSIGNATURE:
      {
        // The file was not signed or had a signature that was not valid.
        // Get the reason for no signature.
        DWORD dwLastError = GetLastError();
        if( dwLastError == TRUST_E_NOSIGNATURE
         || dwLastError == TRUST_E_SUBJECT_FORM_UNKNOWN
         || dwLastError == TRUST_E_PROVIDER_UNKNOWN )
        {
          // The file was not signed.
          TRACE( _T("The file \"%s\" is not signed.\n"), pwszSourceFile );
          if(IDYES==::MessageBox( NULL, L"The TWAIN DSM is not signed.\nContinue to load this DSM?", L"The TWAIN DSM Signature", MB_YESNO|MB_ICONQUESTION))
          {
            lStatus = ERROR_SUCCESS;
          }
        } 
        else 
        {
          // The signature was not valid or there was an error opening the file.
          TRACE( _T("An unknown error occurred trying to verify the signature of the \"%s\" file.\n"), pwszSourceFile );
          lStatus = dwLastError;
        }
      }
      break;

    case TRUST_E_EXPLICIT_DISTRUST:
      // The hash that represents the subject or the publisher is not allowed by the admin or user.
      TRACE( _T("The signature is present, but specifically disallowed.\n") );
      break;

    case TRUST_E_SUBJECT_NOT_TRUSTED:
      // The user clicked "No" when asked to install and run.
      TRACE( _T("The signature is present, but not trusted.\n") );
      break;

    case CRYPT_E_SECURITY_SETTINGS:
      // The hash that represents the subject or the publisher was not explicitly trusted 
      // by the admin and the admin policy has disabled user trust. No signature, publisher 
      // or time stamp errors.
      TRACE( _T("CRYPT_E_SECURITY_SETTINGS - The hash representing the subject or the ")
             _T("publisher wasn't explicitly trusted by the admin and admin policy has ")
             _T("disabled user trust. No signature, publisher or timestamp errors.\n") );
      break;

    case CERT_E_UNTRUSTEDROOT:
      TRACE( _T("A certificate chain processed, but terminated in a root ")
             _T("certificate which is not trusted by the trust provider.\n") );
      break;

    default:
      // The UI was disabled in dwUIChoice or the admin policy 
      // has disabled user trust. lStatus contains the 
      // publisher or time stamp chain error.
      TRACE( _T("Error is:%d \n"), lStatus );
      break;
  }

  LPVOID lpMsgBuf;
  if( lStatus != ERROR_SUCCESS
    && FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        (DWORD)lStatus,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL ))
  {
    TRACE(_T("Error: %s \n"), (LPCTSTR)lpMsgBuf);
    
    {
      HWND hWnd = NULL;
      ::MessageBox( hWnd, (LPCTSTR)lpMsgBuf, L"signature", MB_OK|MB_ICONEXCLAMATION);
    }
    // Free the buffer.
    LocalFree( lpMsgBuf );
  }

  return lStatus==ERROR_SUCCESS;
}
