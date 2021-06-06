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
#include <OcafEx_ILimb.h>

// OcafEx includes
#include <OcafEx_OBBAttr.h>

// OCCT includes
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataXtd_Triangulation.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>

//-----------------------------------------------------------------------------

void OcafEx_ILimb::SetOBB(const OcafEx_OBB& obb)
{
  Handle(OcafEx_OBBAttr) obbAttr;
  if ( !m_label.FindAttribute(OcafEx_OBBAttr::GUID(), obbAttr) )
    OcafEx_OBBAttr::Set(m_label, obb);
  else
    obbAttr->SetOBB(obb);
}

//-----------------------------------------------------------------------------

OcafEx_OBB OcafEx_ILimb::GetOBB() const
{
  Handle(OcafEx_OBBAttr) obbAttr;
  if ( !m_label.FindAttribute(OcafEx_OBBAttr::GUID(), obbAttr) )
    return OcafEx_OBB();

  return obbAttr->GetOBB();
}

//-----------------------------------------------------------------------------

void OcafEx_ILimb::SetMesh(const Handle(Poly_Triangulation)& mesh)
{
  Handle(TDataXtd_Triangulation) meshAttr;
  if ( !m_label.FindAttribute(TDataXtd_Triangulation::GetID(), meshAttr) )
    TDataXtd_Triangulation::Set(m_label, mesh);
  else
    meshAttr->Set(mesh);
}

//-----------------------------------------------------------------------------

Handle(Poly_Triangulation) OcafEx_ILimb::GetMesh() const
{
  Handle(TDataXtd_Triangulation) meshAttr;
  if ( !m_label.FindAttribute(TDataXtd_Triangulation::GetID(), meshAttr) )
    return NULL;

  return meshAttr->Get();
}

//-----------------------------------------------------------------------------

void OcafEx_ILimb::SetShape(const TopoDS_Shape& shape)
{
  TNaming_Builder builder(m_label);
  builder.Generated(shape);
}

//-----------------------------------------------------------------------------

TopoDS_Shape OcafEx_ILimb::GetShape() const
{
  Handle(TNaming_NamedShape) shapeAttr;
  if ( !m_label.FindAttribute(TNaming_NamedShape::GetID(), shapeAttr) )
    return TopoDS_Shape();

  return shapeAttr->Get();
}

//-----------------------------------------------------------------------------

void OcafEx_ILimb::SetTransform(const double tx,
                                const double ty,
                                const double tz,
                                const double rx,
                                const double ry,
                                const double rz)
{
  // Set the attribute or modify the existing one.
  Handle(TDataStd_RealArray) arrAttr;
  if ( !m_label.FindAttribute(TDataStd_RealArray::GetID(), arrAttr) )
    arrAttr = TDataStd_RealArray::Set(m_label, 0, 5);
  //
  arrAttr->SetValue(0, tx);
  arrAttr->SetValue(1, ty);
  arrAttr->SetValue(2, tz);
  arrAttr->SetValue(3, rx);
  arrAttr->SetValue(4, ry);
  arrAttr->SetValue(5, rz);
}

//-----------------------------------------------------------------------------

void OcafEx_ILimb::GetTransform(double& tx,
                                double& ty,
                                double& tz,
                                double& rx,
                                double& ry,
                                double& rz) const
{
  Handle(TDataStd_RealArray) arrAttr;
  if ( !m_label.FindAttribute(TDataStd_RealArray::GetID(), arrAttr) )
    return;

  tx = arrAttr->Value(0);
  ty = arrAttr->Value(1);
  tz = arrAttr->Value(2);
  rx = arrAttr->Value(3);
  ry = arrAttr->Value(4);
  rz = arrAttr->Value(5);
}
