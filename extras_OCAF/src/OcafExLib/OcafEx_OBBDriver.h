//-----------------------------------------------------------------------------
// Creation date: 13 March 2019
// Author:        Sergey Slyadnev
//-----------------------------------------------------------------------------
// Copyright (c) 2019, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef OcafEx_OBBDriver_h
#define OcafEx_OBBDriver_h

// OcafEx includes
#include <OcafEx.h>

// OCCT includes
#include <BinMDF_ADriver.hxx>
#include <Message_Messenger.hxx>

//-----------------------------------------------------------------------------

//! Storage/Retrieval Driver for OBB Attribute.
class OcafEx_OBBDriver : public BinMDF_ADriver
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(OcafEx_OBBDriver, BinMDF_ADriver)

// Construction:
public:

  //! Constructor accepting Message Driver for the parent class.
  //! \param[in] messenger messenger for parent.
  OcafExLib_EXPORT
    OcafEx_OBBDriver(const Handle(Message_Messenger)& messenger);

// Kernel:
public:

  //! Creates an empty instance of OBB Attribute for data transferring.
  //! \return empty instance of OBB Attribute.
  OcafExLib_EXPORT virtual Handle(TDF_Attribute)
    NewEmpty() const;

  //! Transfers data from PERSISTENT source of OBB Attribute into its TRANSIENT
  //! form. This method is a secondary one comparing to the dual Paste() method
  //! performing converse operation and defining the format rules (order and the
  //! membership of the items being recorded) so.
  //! \param[in] FromPersistent persistence buffer to transfer data into the
  //!                           transient instance of OBB Attribute.
  //! \param[in] ToTransient    transient instance of the Attribute being assembled.
  //! \param[in] RelocTable     relocation table.
  //! \return true in case of success, false -- otherwise.
  OcafExLib_EXPORT virtual bool
    Paste(const BinObjMgt_Persistent&  FromPersistent,
          const Handle(TDF_Attribute)& ToTransient,
          BinObjMgt_RRelocationTable&  RelocTable) const;
  
  //! Transfers data from transient instance of OBB Attribute into the
  //! persistence buffer for further binary storing.
  //! \param[in] FromTransient transient OBB Attribute source.
  //! \param[in] ToPersistent  persistence buffer to transfer data to.
  //! \param[in] RelocTable    relocation table.
  OcafExLib_EXPORT virtual void
    Paste(const Handle(TDF_Attribute)& FromTransient,
          BinObjMgt_Persistent&        ToPersistent,
          BinObjMgt_SRelocationTable&  RelocTable) const;

};

#endif
