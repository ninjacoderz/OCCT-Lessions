//-----------------------------------------------------------------------------
// Creation date: 12 March 2019
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

#ifndef OcafEx_IObject_HeaderFile
#define OcafEx_IObject_HeaderFile

// OcafExLib includes
#include <OcafEx.h>

// OCCT includes
#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>

//-----------------------------------------------------------------------------

// Base class for all data access objects.
class OcafEx_IObject : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(OcafEx_IObject, Standard_Transient)

public:

  //! Ctor accepting the root label of the object.
  //! \param[in] label OCAF label to initialize DAO.
  OcafEx_IObject(const TDF_Label& label) : Standard_Transient(), m_label(label) {}

public:

  //! \return root label.
  const TDF_Label& GetLabel() const
  {
    return m_label;
  }

  //! Sets root label for the interface.
  //! \param[in] label OCAF label to set.
  void SetLabel(const TDF_Label& label)
  {
    m_label = label;
  }

  //! \return name of the object.
  TCollection_ExtendedString GetName() const
  {
    Handle(TDataStd_Name) nameAttr;
    if ( !m_label.FindAttribute(TDataStd_Name::GetID(), nameAttr) )
      return "";

    return nameAttr->Get();
  }

  void SetName(const TCollection_ExtendedString& name)
  {
    TDataStd_Name::Set(m_label, name);
  }

protected:

  //! Root label of the object.
  TDF_Label m_label;

};

#endif
