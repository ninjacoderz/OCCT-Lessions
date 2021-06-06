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
#include <OcafEx_OBBDriver.h>

// OcafEx includes
#include <OcafEx_OBBAttr.h>

// OCCT includes
#include <BinObjMgt_Persistent.hxx>

//-----------------------------------------------------------------------------

OcafEx_OBBDriver::OcafEx_OBBDriver(const Handle(Message_Messenger)& messenger)
: BinMDF_ADriver(messenger)
{}

//-----------------------------------------------------------------------------

Handle(TDF_Attribute) OcafEx_OBBDriver::NewEmpty() const
{
  return new OcafEx_OBBAttr;
}

//-----------------------------------------------------------------------------

bool
  OcafEx_OBBDriver::Paste(const BinObjMgt_Persistent&  FromPersistent,
                          const Handle(TDF_Attribute)& ToTransient,
                          BinObjMgt_RRelocationTable&  RelocTable) const
{
  OcafEx_NotUsed(RelocTable);

  Handle(OcafEx_OBBAttr) OBBAttr = Handle(OcafEx_OBBAttr)::DownCast(ToTransient);
  //
  if ( OBBAttr.IsNull() )
  {
    myMessageDriver->Send("ERROR: NULL OBB Attribute", Message_Fail);
    return false;
  }

  /* ==========================================================
   *  Read number of nodes and elements from the binary buffer
   * ========================================================== */

  double pos_X,       pos_Y,       pos_Z;
  double OZ_X,        OZ_Y,        OZ_Z;
  double OX_X,        OX_Y,        OX_Z;
  double MinCorner_X, MinCorner_Y, MinCorner_Z;
  double MaxCorner_X, MaxCorner_Y, MaxCorner_Z;

  // Restore data from buffer
  FromPersistent >> pos_X       >> pos_Y       >> pos_Z
                 >> OZ_X        >> OZ_Y        >> OZ_Z
                 >> OX_X        >> OX_Y        >> OX_Z
                 >> MinCorner_X >> MinCorner_Y >> MinCorner_Z
                 >> MaxCorner_X >> MaxCorner_Y >> MaxCorner_Z;

  // Create OBB
  gp_Ax3 ax3( gp_Pnt(pos_X, pos_Y, pos_Z), gp_Dir(OZ_X, OZ_Y, OZ_Z), gp_Dir(OX_X, OX_Y, OX_Z) );
  //
  OcafEx_OBB obb;
  obb.Placement      = ax3;
  obb.LocalCornerMin = gp_Pnt(MinCorner_X, MinCorner_Y, MinCorner_Z);
  obb.LocalCornerMax = gp_Pnt(MaxCorner_X, MaxCorner_Y, MaxCorner_Z);

  // Store OBB
  OBBAttr->SetOBB(obb);

  return true;
}

//-----------------------------------------------------------------------------

void OcafEx_OBBDriver::Paste(const Handle(TDF_Attribute)& FromTransient,
                             BinObjMgt_Persistent&        ToPersistent,
                             BinObjMgt_SRelocationTable&  RelocTable) const
{
  OcafEx_NotUsed(RelocTable);

  /* ======================
   *  Access OBB Attribute
   * ====================== */

  Handle(OcafEx_OBBAttr) OBBAttr = Handle(OcafEx_OBBAttr)::DownCast(FromTransient);
  //
  if ( OBBAttr.IsNull() )
  {
    myMessageDriver->Send("ERROR: NULL OBB Attribute", Message_Fail);
    return;
  }

  const OcafEx_OBB& obb = OBBAttr->GetOBB();

  /* =====================
   *  Push data to buffer
   * ===================== */

  // Access data
  const gp_Pnt& pos = obb.Placement.Location();
  const gp_Dir& OZ  = obb.Placement.Direction();
  const gp_Dir& OX  = obb.Placement.XDirection();
  //
  const gp_Pnt& MinCorner = obb.LocalCornerMin;
  const gp_Pnt& MaxCorner = obb.LocalCornerMax;

  // Push data
  ToPersistent << pos.X()       << pos.Y()       << pos.Z()
               << OZ.X()        << OZ.Y()        << OZ.Z()
               << OX.X()        << OX.Y()        << OX.Z()
               << MinCorner.X() << MinCorner.Y() << MinCorner.Z()
               << MaxCorner.X() << MaxCorner.Y() << MaxCorner.Z();
}
