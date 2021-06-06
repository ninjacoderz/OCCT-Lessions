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

// Own include
#include <OcafEx_OBBAttr.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------

OcafEx_OBBAttr::OcafEx_OBBAttr() : TDF_Attribute()
{}

//-----------------------------------------------------------------------------

Handle(OcafEx_OBBAttr) OcafEx_OBBAttr::Set(const TDF_Label& Label)
{
  Handle(OcafEx_OBBAttr) A;
  //
  if ( !Label.FindAttribute(GUID(), A) )
  {
    A = new OcafEx_OBBAttr();
    Label.AddAttribute(A);
  }
  return A;
}

//-----------------------------------------------------------------------------

Handle(OcafEx_OBBAttr) OcafEx_OBBAttr::Set(const TDF_Label&  Label,
                                           const OcafEx_OBB& obb)
{
  Handle(OcafEx_OBBAttr) attr = Set(Label);
  attr->SetOBB(obb);

  return attr;
}

//-----------------------------------------------------------------------------
// Accessors for Attribute's GUID
//-----------------------------------------------------------------------------

const Standard_GUID& OcafEx_OBBAttr::GUID()
{
  static Standard_GUID AttrGUID("86BFC9DD-0B5C-4F41-B179-C2932DE9268F");
  return AttrGUID;
}

//-----------------------------------------------------------------------------

const Standard_GUID& OcafEx_OBBAttr::ID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------
// Attribute's kernel methods:
//-----------------------------------------------------------------------------

Handle(TDF_Attribute) OcafEx_OBBAttr::NewEmpty() const
{
  return new OcafEx_OBBAttr();
}

//-----------------------------------------------------------------------------

void OcafEx_OBBAttr::Restore(const Handle(TDF_Attribute)& MainAttr)
{
  Handle(OcafEx_OBBAttr) fromCasted = Handle(OcafEx_OBBAttr)::DownCast(MainAttr);
  m_obb = fromCasted->GetOBB();
}

//-----------------------------------------------------------------------------

void OcafEx_OBBAttr::Paste(const Handle(TDF_Attribute)&       Into,
                           const Handle(TDF_RelocationTable)& RelocTable) const
{
  OcafEx_NotUsed(RelocTable);

  Handle(OcafEx_OBBAttr) intoCasted = Handle(OcafEx_OBBAttr)::DownCast(Into);
  intoCasted->SetOBB(m_obb);
}

//-----------------------------------------------------------------------------
// Accessors for domain-specific data
//-----------------------------------------------------------------------------

void OcafEx_OBBAttr::SetOBB(const OcafEx_OBB& obb)
{
  this->Backup();

  m_obb = obb;
}

//-----------------------------------------------------------------------------

const OcafEx_OBB& OcafEx_OBBAttr::GetOBB() const
{
  return m_obb;
}
