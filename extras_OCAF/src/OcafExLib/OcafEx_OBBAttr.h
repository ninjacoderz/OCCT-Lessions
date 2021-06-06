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

#ifndef OcafEx_OBBAttr_h
#define OcafEx_OBBAttr_h

// OcafEx includes
#include <OcafEx_BuildOBB.h>

// OCCT includes
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

//-----------------------------------------------------------------------------

//! OCAF Attribute representing Oriented Bounding Box (OBB).
class OcafEx_OBBAttr : public TDF_Attribute
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(OcafEx_OBBAttr, TDF_Attribute)

// Construction & settling-down routines:
public:

  //! Default constructor.
  OcafExLib_EXPORT
    OcafEx_OBBAttr();

public:

  //! Settles down new OBB Attribute to the given OCAF Label.
  //! \param[in] Label TDF Label to settle down the new Attribute to.
  //! \return newly created Attribute settled down onto the target Label.
  OcafExLib_EXPORT static Handle(OcafEx_OBBAttr)
    Set(const TDF_Label& Label);

  //! Settles down new OBB Attribute to the given OCAF Label.
  //! \param[in] Label TDF Label to settle down the new Attribute to.
  //! \param[in] obb   OBB to set.
  //! \return newly created Attribute settled down onto the target Label.
  OcafExLib_EXPORT static Handle(OcafEx_OBBAttr)
    Set(const TDF_Label&  Label,
        const OcafEx_OBB& obb);

// GUID accessors:
public:

  //! Returns statically defined GUID for OBB Attribute.
  //! \return statically defined GUID.
  OcafExLib_EXPORT static const Standard_GUID&
    GUID();

  //! Accessor for GUID associated with this kind of OCAF Attribute.
  //! \return GUID of the OCAF Attribute.
  OcafExLib_EXPORT virtual const Standard_GUID&
    ID() const;

// Attribute's kernel methods:
public:

  //! Creates new instance of OBB Attribute which is not initially populated
  //! with any data structures.
  //! \return new instance of OBB Attribute.
  OcafExLib_EXPORT virtual Handle(TDF_Attribute)
    NewEmpty() const;

  //! Performs data transferring from the given OCAF Attribute to this one.
  //! This method is mainly used by OCAF Undo/Redo kernel as a part of
  //! backup functionality.
  //! \param[in] MainAttr OCAF Attribute to copy data from.
  OcafExLib_EXPORT virtual void
    Restore(const Handle(TDF_Attribute)& MainAttr);

  //! Supporting method for Copy/Paste functionality. Performs full copying of
  //! the underlying data.
  //! \param[in] Into       where to paste.
  //! \param[in] RelocTable relocation table.
  OcafExLib_EXPORT virtual void
    Paste(const Handle(TDF_Attribute)& Into,
          const Handle(TDF_RelocationTable)& RelocTable) const;

// Accessors for domain-specific data:
public:

  //! Sets OBB.
  OcafExLib_EXPORT void
    SetOBB(const OcafEx_OBB& obb);

  //! Returns the stored OBB.
  //! \return stored OBB.
  OcafExLib_EXPORT const OcafEx_OBB&
    GetOBB() const;

// Members:
private:

  //! Stored OBB.
  OcafEx_OBB m_obb;

};

#endif
