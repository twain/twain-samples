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
* @file CTWAINDS_Base.h
* Base class describing a TWAIN Data Source.
* @author TWAIN Working Group
* @date April 2007
*/

#ifndef __CTWAINBASE_H__
#define __CTWAINBASE_H__

#include "CommonDS.h"

/**
* possible States of the DS.
* The five possible states of the Data Source Manager
*/
typedef enum
{
  dsState_Loaded = 3,   /**< Source is loaded, but not open. */
  dsState_Open,         /**< Source is open, and ready to: List & Negotiate Capabilities, 
                         *   Request the Acquisition of data, and Close. */
  dsState_Enabled,      /**< If UI is being used it is displayed. */
  dsState_XferReady,    /**< Transfers are ready. */
  dsState_Xferring      /**< Transfering data. */
} DS_State;

/**
* This is a TWAIN compliant base class. It contains all of the capabilities
* and functions that are required to be a TWAIN compliant source. Simply
* inherit this class and implement the pure virtual functions.
* This basic version of a Data Source only supports connection of one 
* application at a time.
*/
class CTWAINDS_Base
{
public:
  CTWAINDS_Base();
  virtual ~CTWAINDS_Base();

  /**
  * This is the same as the main DS_Entry function. Routes traffic
  * to the proper location.
  * @param[in] _pOrigin Identifies the source application of the message.
  * @param[in] _DG      The Data Group. 
  * @param[in] _DAT     The Data Attribute Type.
  * @param[in] _MSG     The message with respect to the Data Group and the Data Attribute Type.
  * @param[in,out] _pData   A pointer to the data structure or variable identified 
  *          by the Data Attribute Type.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_UINT16 DS_Entry( pTW_IDENTITY _pOrigin,
                              TW_UINT32    _DG,
                              TW_UINT16    _DAT,
                              TW_UINT16    _MSG,
                              TW_MEMREF    _pData);

  /**
  * Fills _pStatus with the current condition code then resets the condition code.
  * @param[in] _MSG valid message for DAT_STATUS.
  * @param[out] _pStatus current condition code as pointer to TW_STATUS.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_status(TW_UINT16     _MSG,
                              pTW_STATUS    _pStatus);

  /**
  * handles DAT_EVENT's
  * @param[in] _MSG the message to handle.
  * @param[in] _pEvent a pointer to a TW_EVENT structure of the data.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_event(TW_UINT16    _MSG,
                             pTW_EVENT    _pEvent);

  /**
  * handles DAT_IDENTITY requests.
  * @param[in] _pOrigin a pointer to a TW_IDENTITY of the sender of the message.
  * @param[in] _MSG the message to handle.
  * @param[in,out] _pData pointer to a TW_IDENTITY structure to pass or retrieve data based on message.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_identity(pTW_IDENTITY _pOrigin,
                                TW_UINT16    _MSG,
                                pTW_IDENTITY _pData);

  /**
  * handles DAT_PENDINGXFERS requests.
  * @param[in] _MSG the message to handle.
  * @param[out] _pXfers a pointer to a TW_PENDINGXFERS structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_pendingxfers(TW_UINT16        _MSG,
                                    pTW_PENDINGXFERS _pXfers);

  /**
  * handles DAT_CAPABILITY requests.
  * @param[in] _MSG the message to handle.
  * @param[in] _pCap pointer to TW_CAPABILITY structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_capability(TW_UINT16      _MSG,
                                  pTW_CAPABILITY _pCap);

  /**
  * handles DAT_USERINTERFACE requests.
  * @param[in] _MSG the message to handle.
  * @param[in,out] _pData a pointer to a TW_USERINTERFACE structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_userinterface(TW_UINT16         _MSG,
                                     pTW_USERINTERFACE _pData);

  /**
  * handles DAT_SETUPFILEXFER requests.
  * @param[in] _MSG the message to handle.
  * @param[in] _pData a pointer to a TW_SETUPFILEXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_setupfilexfer(TW_UINT16         _MSG,
                                     pTW_SETUPFILEXFER _pData);

  /**
  * handles DAT_SETUPMEMXFER requests.
  * @param[in] _MSG the message to handle.
  * @param[in] _pData a pointer to a TW_SETUPMEMXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_setupmemxfer(TW_UINT16        _MSG,
                                    pTW_SETUPMEMXFER _pData);

  /**
  * handles DAT_IMAGENATIVEXFER requests.
  * @param[in] _MSG the message to handle.
  * @param[in] _pData a handle to the memory.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_imagenativexfer(TW_UINT16    _MSG,
                                       TW_MEMREF*   _pData);

  /**
  * handles DAT_IMAGEFILEXFER requests.
  * @param[in] _MSG the message to handle.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_imagefilexfer(TW_UINT16      _MSG);

  /**
  * handles DAT_IMAGEMEMXFER requests.
  * @param[in] _MSG the message to handle.
  * @param[in] _pData a pointer to a TW_IMAGEMEMXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_imagememxfer(TW_UINT16         _MSG,
                                    pTW_IMAGEMEMXFER  _pData);

  /**
  * handles DAT_IMAGELAYOUT requests.
  * @param[in] _MSG the message to handle.
  * @param[in] _pData a pointer to a TW_IMAGELAYOUT structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_imagelayout(TW_UINT16         _MSG,
                                   pTW_IMAGELAYOUT   _pData);

  /**
  * Figures out what to do with the DAT_IMAGEINFO request.
  * @param[in] _MSG the message to handle.
  * @param[in] _pImageInfo a pointer to a TW_IMAGEINFO structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_imageinfo(TW_UINT16     _MSG,
                                 pTW_IMAGEINFO _pImageInfo);

  /**
  * Figures out what to do with the DAT_EXTIMAGEINFO request.
  * @param[in] _MSG the message to handle.
  * @param[in] _pData a pointer to a TW_EXTIMAGEINFO structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_extimageinfo(TW_UINT16         _MSG,
                                    pTW_EXTIMAGEINFO  _pData);


  /**
  * Figures out what to do with the DAT_ENTRYPOINT request.
  * @param[in] _MSG the message to handle.
  * @param[in] _pEntryPoints a pointer to a TW_ENTRYPOINT structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_entrypoint(TW_UINT16        _MSG,
                                    pTW_ENTRYPOINT _pEntryPoints);

  /**
  * Figures out what to do with the DAT_XFERGROUP request.
  * @param[in] _MSG the message to handle.
  * @param[in] _pXferGroup a pointer to a TW_UINT32 value.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 dat_xfergroup(TW_UINT16   _MSG,
                                 pTW_UINT32  _pXferGroup);


  //////////////////////////////////////////////////////////////////////////////
  /**
  * @name Pure Virtuals
  * @{
  */

  /**
  * Fills the identity structure with our information.
  * Overload this function in derived classes to uniquely identify them selves.
  * @param[out] _idStruct our TW_IDENTITY information.
  */
  virtual void fillIdentityStructure(TW_IDENTITY& _idStruct) = 0;

  /**
  * Initialize the Datasource.  Allocate memory for Capabilities.
  * Sets condition code if had problem.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 Initialize() = 0;

  /**
  * Enable the Data Source.
  * Called when a DG_CONTROL / DAT_USERINTERFACE / MSG_ENABLEDS op is sent.
  * @param[in] _pData a pointer to a TW_USERINTERFACE structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 enableDS(pTW_USERINTERFACE _pData) = 0;

  /**
  * Disable the Data Source.
  * Called when a DG_CONTROL / DAT_USERINTERFACE / MSG_DISABLEDS op is sent.
  * @param[in] _pData a pointer to a TW_USERINTERFACE structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 disableDS(pTW_USERINTERFACE _pData) = 0;

  /**
  * Get image information.
  * Called when a DG_IMAGE / DAT_IMAGEINFO / MSG_GET op is sent.
  * -If in state 6, general info is provided about the image about to be transferred.
  * -If in state 7, specific info is provided about the current image just transferred.
  * @param[out] _pImageInfo a pointer to TW_IMAGEINFO structure to return image information.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 getImageInfo(pTW_IMAGEINFO _pImageInfo) = 0;

  /**
  * Open the Data Source.
  * Called when a DG_CONTROL / DAT_IDENTITY / MSG_OPENDS op is sent.
  * @param[in] _pOrigin a pointer to TW_IDENTITY structure of the Application identity.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 openDS(pTW_IDENTITY _pOrigin) = 0;

  /**
  * Close the Data Source.
  * Called when a DG_CONTROL / DAT_IDENTITY / MSG_CLOSEDS op is sent.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 closeDS() = 0;

  /**
  * get data for memory transfer.
  * Called when a DG_CONTROL / DAT_SETUPMEMXFER / MSG_GET op is sent.
  * @param[in] _pData a pointer to TW_SETUPMEMXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 getMemoryXfer(pTW_SETUPMEMXFER _pData) = 0;

  /**
  * Process events.
  * Called when a DG_CONTROL / DAT_EVENT / MSG_PROCESSEVENT op is sent.
  * @param[in] _pEvent a pointer to TW_EVENT structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 processEvent(pTW_EVENT _pEvent) = 0;

  /**
  * Stop currect transfer if not done.  Single from application that application is 
  * done with all data with current image.
  * Check to see if there is still documents or data remaining to transfer.
  * Called when a DG_CONTROL / DAT_PENDINGXFERS / MSG_ENDXFER op is sent.
  * @param[out] _pXfers a pointer to TW_PENDINGXFERS structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 endXfer(pTW_PENDINGXFERS _pXfers) = 0;

  /**
  * Check to see if there is still documents or data remaining to transfer.
  * Called when a DG_CONTROL / DAT_PENDINGXFERS / MSG_GET op is sent.
  * @param[out] _pXfers a pointer to TW_PENDINGXFERS structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 getXfer(pTW_PENDINGXFERS _pXfers) = 0;

  /**
  * Flush all pending transfers from the Source..
  * Called when a DG_CONTROL / DAT_PENDINGXFERS / MSG_RESET op is sent.
  * @param[out] _pXfers a pointer to TW_PENDINGXFERS structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 resetXfer(pTW_PENDINGXFERS _pXfers) = 0;

  /**
  * Transfer image data from scanner to memory.  
  * Called during one of the transfer methods.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 transfer() = 0;

  // END Pure Virtuals
  /**
  * @}
  */

  //////////////////////////////////////////////////////////////////////////////
  /**
  * @name Capabilities
  * @{
  */

  /**
  * Start the transfer of image data natively.
  * Called when a DG_IMAGE / DAT_IMAGENATIVEXFER / MSG_GET op is sent.
  * @param[out] _pData a pointer to store the image locaton.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 transferNativeImage(TW_MEMREF* _pData);

  /**
  * Return info about the file that the Source will write the acquired data into
  * Called when a DG_CONTROL / DAT_SETUPFILEXFER / MSG_GET op is sent.
  * @param[in] _pData a pointer to TW_SETUPFILEXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 getFileXfer(pTW_SETUPFILEXFER _pData);

  /**
  * Return the default file transfer information
  * Called when a DG_CONTROL / DAT_SETUPFILEXFER / MSG_GETDEFAULT op is sent.
  * @param[in] _pData a pointer to TW_SETUPFILEXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 getDefaultFileXfer(pTW_SETUPFILEXFER _pData);

  /**
  * Reset current file information to default values
  * Called when a DG_CONTROL / DAT_SETUPFILEXFER / MSG_RESET op is sent.
  * @param[in] _pData a pointer to TW_SETUPFILEXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 resetFileXfer(pTW_SETUPFILEXFER _pData);

  /**
  * Set file transfer information for next file transfer
  * Called when a DG_CONTROL / DAT_SETUPFILEXFER / MSG_SET op is sent.
  * @param[in] _pData a pointer to TW_SETUPFILEXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 setFileXfer(pTW_SETUPFILEXFER _pData);

  /**
  * Save image data to file.
  * Called when a DG_IMAGE / DAT_IMAGEFILEXFER / MSG_GET op is sent.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 saveImageFile();

  /**
  * Transfer memory buffers.
  * Called when a DG_IMAGE / DAT_IMAGEMEMXFER / MSG_GET op is sent.
  * @param[out] _pData a pointer to TW_IMAGEMEMXFER structure.
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 transferMemoryBuffers(pTW_IMAGEMEMXFER _pData);

  /**
  * get the Internal Image format.
  * Called when a DG_IMAGE / DAT_IMAGENATIVEXFER / MSG_GET op is sent.
  * also used when creating a BMP for saving when DG_IMAGE / DAT_IMAGEFILEXFER / MSG_GET op is sent.
  * @param[out] _pImage a pointer to store the image locaton.
  * @return a valid TWRC_xxxx return code.
  */
  TW_INT16 getDIBImage(TW_MEMREF* _pImage);

  /**
  * Save the current image data as a BMP file
  * @return a valid TWRC_xxxx return code, TWRC_XFERDONE on success.
  */
  TW_INT16 saveImageFileAsBMP();

  /**
  * Save the current image data as a TIFF file
  * @return a valid TWRC_xxxx return code, TWRC_XFERDONE on success.
  */
  TW_INT16 saveImageFileAsTIFF();

  /**
  * Request the DSM that we are ready to send images.
  * @return true if successful.
  */
  virtual bool DoXferReadyEvent();

  /**
  * Request the DSM the most likly user clicks on the “OK” button on GUI.
  * @return true if successful.
  */
  virtual bool DoCloseDSOkEvent();

  /**
  * Request the DSM that the Source’s user interface be disabled.  
  * @return true if successful.
  */
  virtual bool DoCloseDSRequestEvent();

  /**
  * Report to the DSM that a device event has occurred.
  * @return true if successful.
  */
  virtual bool DoDeviceEvent();

  // END Capabilities 
  /**
  * @}
  */

  //////////////////////////////////////////////////////////////////////////////
  /**
  * @name Accessors
  * @{
  */

  /**
  * Returns the current condition code.
  * @return a valid TWCC_xxxxxx condition code.
  */
  virtual TW_INT16 getConditionCode() const;

  /**
  * Sets the current condition code.
  * @param[in] _cc a valid TWCC_xxxxxx condition code.
  */
  virtual void setConditionCode(TW_INT16 _cc);

  /**
  * Returns a pointer to our identity structure. 
  * @return pointer to out TW_IDENTITY identity structure.
  */
  pTW_IDENTITY getIdentity();

  /**
  * Returns a pointer to the source application controlling us.
  * @return pointer to  TW_IDENTITY identity structure.
  */
  pTW_IDENTITY getApp();

  /**
  * Returns the current state of the Data Source.
  * @return a DS_State of the current state.
  */
  DS_State getState() { return m_CurrentState; }

  /** 
  * This is a template class that will actually operate on the capability. 
  * It supports all valid CTWAINContainer types which are currently INTS, FIX32 and FRAME's.
  * @param[in] _MSG the message
  * @param[in] _pContainer the container type
  * @param[in] _pCap the capability
  * @return a valid TWRC_xxxx return code.
  */
  template<class TWAINContainerType> 
  TW_INT16 handleCap(TW_UINT16 _MSG, TWAINContainerType* _pContainer, pTW_CAPABILITY _pCap);

 /** 
  * Return a CTWAINContainer class of the capability. 
  * @param[in] _unCap the capability looking for
  * @return the capability if found
  */
  virtual CTWAINContainer* findCapability(const TW_UINT16 _unCap);

  /**
  * if the CTWAINContainer is dependend on another Capabiltiy or settings. 
  * This function is used to get the data it needs.
  * @param[in] _pContainer the container
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 updatePreDependencies(CTWAINContainer* _pContainer);

    /**
  * if the CTWAINContainer is dependend on another Capabiltiy. 
  * This function is used to get the data it needs.
  * @param[in] MSG the message that was sent
  * @param[in] Cap the Capability that it was sent to
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 updatePostDependencies(TW_UINT16 MSG, TW_UINT16 Cap);

  /**
  * get the current unit and resolution.  A helper function used by Unit dependent cap
  * @param[out] Unit the current unit value.
  * @param[out] Xres the current X resolution value.
  * @param[out] Yres the current Y resolution value.
  * @return a CTWAINContainer for the capability.
  */
  TW_INT16 getCurrentUnits(int &Unit, float &Xres, float &Yres);

// END Accessors
  /** 
  * @}
  */
  static TW_IDENTITY          m_TheIdentity;               /**< default Identity */

protected:
  TWAINCapabilitiesMap m_IndependantCapMap;   /**< Stores a list of all capacitites that each instance support. */

  TW_INT16          m_DSConditionCode;        /**< Current global condition. */
  TW_IDENTITY       m_MyIdentity;             /**< Detail information of our information.*/
  TW_IDENTITY       m_App;                    /**< Detail information of the application source. */
  DS_State          m_CurrentState;           /**< Current state of the Data Source. */
  TW_STR255         m_CurFileExferName;       /**< Current File Transfer Name. */
  TW_STR255         m_DefFileExferName;       /**< Default File Transfer Name. */
  TW_IMAGEINFO      m_ImageInfo;              /**< Current Image Info data. */
  TW_UINT32         m_DocumentNumber;         /**< Current Document Number */
  TW_UINT32         m_PageNumber;             /**< Current Page Number */
  TW_PENDINGXFERS   m_Xfers;                  /**< Number of Transfers remianing in this batch */
  DWORD             m_nDestScanLine;          /**< Current Scanline used for memory transfer */
  TW_HANDLE         m_hImageData;             /**< Handle to Current Image Data */

};

#endif // __CTWAINBASE_H__

