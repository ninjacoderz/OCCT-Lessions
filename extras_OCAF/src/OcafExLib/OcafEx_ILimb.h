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

#ifndef OcafEx_ILimb_HeaderFile
#define OcafEx_ILimb_HeaderFile

// OcafEx includes
#include <OcafEx_BuildOBB.h>
#include <OcafEx_IObject.h>

// OCCT includes
#include <Poly_Triangulation.hxx>
#include <TopoDS_Shape.hxx>

//-----------------------------------------------------------------------------

//! Data access object for limbs.
class OcafEx_ILimb : public OcafEx_IObject
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(OcafEx_ILimb, OcafEx_IObject)

public:

  OcafExLib_EXPORT void
    SetOBB(const OcafEx_OBB& obb);

  OcafExLib_EXPORT OcafEx_OBB
    GetOBB() const;

  OcafExLib_EXPORT void
    SetMesh(const Handle(Poly_Triangulation)& mesh);

  OcafExLib_EXPORT Handle(Poly_Triangulation)
    GetMesh() const;

  OcafExLib_EXPORT void
    SetShape(const TopoDS_Shape& shape);

  OcafExLib_EXPORT TopoDS_Shape
    GetShape() const;

  OcafExLib_EXPORT void
    SetTransform(const double tx,
                 const double ty,
                 const double tz,
                 const double rx,
                 const double ry,
                 const double rz);

  OcafExLib_EXPORT void
    GetTransform(double& tx,
                 double& ty,
                 double& tz,
                 double& rx,
                 double& ry,
                 double& rz) const;

public:

  //! Ctor accepting the root label of the object.
  //! \param[in] label OCAF label to initialize DAO.
  OcafEx_ILimb(const TDF_Label& label) : OcafEx_IObject(label) {}

};

#endif
