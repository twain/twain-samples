/***************************************************************************
* Copyright © 2010 TWAIN Working Group:  
*   Adobe Systems Incorporated, AnyDoc Software Inc., Eastman Kodak Company, 
*   Fujitsu Computer Products of America, JFL Peripheral Solutions Inc., 
*   Avision Inc., and Atalasoft, Inc.
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
* @file ContainerWrapper.h
* @brief  Contains the Declaration of [DESCRIPTION]
* @author JFL Peripheral Solutions Inc.
* @date September 2010
*/
#ifndef _inc_ContainerWrapper_h
#define _inc_ContainerWrapper_h
#include "twain.h"

/**
* Loose wrapper over a TWAIN container
* Accessing values in this class requires some casting savvy
*/
class CContainerWrapper
{
  public:
  /**
  * Constructor cracks the container and prepares internal member variables
  * @param[in] hContainer TWAIN handle that refers to the TWAIN Container in question
  * @param[in] twConType the type of the supplied container (must be a match for hContainer)
  * @param[in] bOwn default value is true, decides if the class should free the container on destruction
  */
  CContainerWrapper(TW_HANDLE hContainer, TW_UINT16 twConType, bool bOwn=true);
  /**
  * Destructor deletes the wrapped TWAIN container if bOwn was specified during Construction
  */
  ~CContainerWrapper(void);

  /**
  * Method for checking if the wrapped container is valid
  * @return true if the container is valid
  */
  inline bool IsValid();

  /**
  * Returns the container type
  * @return a valid container type, or TWON_DONTCARE if container is invalid
  */
  inline TW_UINT16 GetConType();
  /**
  * Returns the type of the items found inside the container
  * @return the type items stored in the container, or TWON_DONTCARE if container is invalid
  */
  TW_UINT16 GetItemType();
  /**
  * Returns the number of items stored in the container
  * @return the number of items stored in the container or 0 if container is invalid
  */
  TW_UINT32 GetCount();
  /**
  * Searches the container for the supplied value
  * @param[in] twValue the value to look for
  * @return true if successful
  */
  bool ContainsItem(TW_UINT16 twValue);
  /**
  * Returns a pointer to the item at the requested index for an Array or Enum container
  * @param[in] uiItemIndex the index of the item to get
  * @return a valid pointer if successful, null if outside the valid range or incorrect container
  */
  void *GetItem(TW_UINT32 uiItemIndex);
  /**
  * Returns a pointer to the items if container is a valid One/Array or Enum type
  * @return if successful a valid pointer to 'GetCount' items of 'GetItemType', NULL if the container is invalid or a Range type
  */
  inline const void *GetItems();
  /**
  * Returns a pointer to the Current item
  * @return if successful a valid pointer to the Current item of 'GetItemType', NULL if the container is invalid
  */
  inline const void *GetCurrent();
  /**
  * Returns a pointer to the Default item
  * @return if successful a valid pointer to the Default item of 'GetItemType', NULL if the container is invalid
  */
  inline const void *GetDefault();
  /**
  * Returns a pointer to the Minimum value item
  * @return if successful a valid pointer to the Minimum item of 'GetItemType', NULL if the container is invalid or not a Range type
  */
  inline const void *GetMin();
  /**
  * Returns a pointer to the Maximum value item
  * @return if successful a valid pointer to the Maximum item of 'GetItemType', NULL if the container is invalid or not a Range type
  */
  inline const void *GetMax();
  /**
  * Returns a pointer to the Step value item
  * @return if successful a valid pointer to the Step item of 'GetItemType', NULL if the container is invalid or not a Range type
  */
  inline const void *GetStep();
  /**
  * Utility function for correctly addressing pointers in RANGE and ONEVALUE types
  * Necessary for coding to support BIGENDIAN systems
  * @param[in] pItem pointer to convert
  * @param[in] twType the type of item pointed to by pItem
  * @return a valid pointer to the correct type specified by twType
  */
  static void *CContainerWrapper::ConvertContainerAddressByteType(void *pItem, TW_UINT16 twType);

  protected:
  bool m_bValid; /**< flag indicates that class believes the container to be valid */
  bool m_bOwn; /**< indicates if destructor should delete the container  */
  TW_HANDLE m_hContainer; /**< container wrapped by this class */
  TW_UINT32 m_twConType; /**< type of contianer wrapped by this class */
  void *m_pCurrent; /**< pointer to the current value stored in the container */
  void *m_pDefault; /**< pointer to the default value stored in the container */
  void *m_pItems; /**< pointer to all the values stored in the container */
  void *m_pContainer; /**< pointer to the start of the container memory */
  void *m_pMinimum; /**< pointer to the default value stored in the container */
  void *m_pMaximum; /**< pointer to the default value stored in the container */
  void *m_pStep; /**< pointer to the default value stored in the container */
};

inline TW_UINT16 CContainerWrapper::GetConType()
{
  return m_twConType;
}

inline bool CContainerWrapper::IsValid()
{
  return m_bValid;
}

inline const void *CContainerWrapper::GetItems()
{
  return m_pItems;
}

inline const void *CContainerWrapper::GetCurrent()
{
  return m_pCurrent;
}

inline const void *CContainerWrapper::GetDefault()
{
  return m_pDefault;
}

inline const void *CContainerWrapper::GetMin()
{
  return m_pMinimum;
}

inline const void *CContainerWrapper::GetMax()
{
  return m_pMaximum;
}

inline const void *CContainerWrapper::GetStep()
{
  return m_pStep;
}

#endif //#ifndef _inc_ContainerWrapper_h
