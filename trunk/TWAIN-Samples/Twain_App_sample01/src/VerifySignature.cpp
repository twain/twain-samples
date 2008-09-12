//-------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Example of verifying the embedded signature of a PE file by using 
// the WinVerifyTrust function.

#define _UNICODE 1
#define UNICODE 1

#ifdef _WINDOWS
  #include "stdafx.h"
#endif

#include <afx.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>

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
  WinTrustData.dwUIChoice           = WTD_UI_ALL;//WTD_UI_NONE;      // Disable WVT UI.
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
      TRACE( "The file \"%s\" is signed and the signature was verified.", pwszSourceFile );
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
          TRACE( "The file \"%s\" is not signed.", pwszSourceFile );
        } 
        else 
        {
          // The signature was not valid or there was an error opening the file.
          TRACE( "An unknown error occurred trying to verify the signature of the \"%s\" file.", pwszSourceFile );
          lStatus = dwLastError;
        }
      }
      break;

    case TRUST_E_EXPLICIT_DISTRUST:
      // The hash that represents the subject or the publisher is not allowed by the admin or user.
      TRACE( "The signature is present, but specifically disallowed." );
      break;

    case TRUST_E_SUBJECT_NOT_TRUSTED:
      // The user clicked "No" when asked to install and run.
      TRACE( "The signature is present, but not trusted." );
      break;

    case CRYPT_E_SECURITY_SETTINGS:
      // The hash that represents the subject or the publisher was not explicitly trusted 
      // by the admin and the admin policy has disabled user trust. No signature, publisher 
      // or time stamp errors.
      TRACE( "CRYPT_E_SECURITY_SETTINGS - The hash representing the subject or the "
            "publisher wasn't explicitly trusted by the admin and admin policy has "
            "disabled user trust. No signature, publisher or timestamp errors." );
      break;

    case CERT_E_UNTRUSTEDROOT:
      TRACE( "A certificate chain processed, but terminated in a root "
           "certificate which is not trusted by the trust provider. " );
      break;

    default:
      // The UI was disabled in dwUIChoice or the admin policy 
      // has disabled user trust. lStatus contains the 
      // publisher or time stamp chain error.
      TRACE( "Error is:%d ", lStatus );
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
    TRACE("Error: %s", (LPCTSTR)lpMsgBuf);
    
    {
      HWND hWnd = NULL;
      ::MessageBox( hWnd, (LPCTSTR)lpMsgBuf, L"signature", MB_OK|MB_ICONEXCLAMATION);
    }
    // Free the buffer.
    LocalFree( lpMsgBuf );
  }

  return lStatus==ERROR_SUCCESS;
}
