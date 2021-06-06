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

// Own include
#include <OcafEx_ISocket.h>

// OcafEx includes
#include <OcafEx_ILimb.h>

// OCCT includes
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>

//-----------------------------------------------------------------------------

void OcafEx_ISocket::SetShape(const TopoDS_Shape& shape)
{
  TNaming_Builder builder(m_label);
  builder.Generated(shape);
}

//-----------------------------------------------------------------------------

TopoDS_Shape OcafEx_ISocket::GetShape() const
{
  Handle(TNaming_NamedShape) shapeAttr;
  if ( !m_label.FindAttribute(TNaming_NamedShape::GetID(), shapeAttr) )
    return TopoDS_Shape();

  return shapeAttr->Get();
}

//-----------------------------------------------------------------------------

void OcafEx_ISocket::SetLimb(const Handle(OcafEx_ILimb)& limb)
{
  Handle(TDF_Reference) refAttr;
  if ( !m_label.FindAttribute(TDF_Reference::GetID(), refAttr) )
    refAttr = TDF_Reference::Set( m_label, limb->GetLabel() );
  else
    refAttr->Set( limb->GetLabel() );
}

//-----------------------------------------------------------------------------

Handle(OcafEx_ILimb) OcafEx_ISocket::GetLimb() const
{
  Handle(TDF_Reference) refAttr;
  if ( !m_label.FindAttribute(TDF_Reference::GetID(), refAttr) )
    return NULL;

  return new OcafEx_ILimb( refAttr->Get() );
}
