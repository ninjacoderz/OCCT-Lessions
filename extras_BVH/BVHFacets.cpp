//-----------------------------------------------------------------------------
// Created on: 21 September 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Quaoar, https://analysissitus.org
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
//    * Neither the name of the copyright holder(s) nor the
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
#include "BVHFacets.h"

// BVH includes
#include "BVHIterator.h"

// Viewer includes
#include "Viewer.h"

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <BVH_LinearBuilder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

// Standard includes
#include <map>

#define DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif


//-----------------------------------------------------------------------------

BVHFacets::BVHFacets(const TopoDS_Shape&  model,
                     const BVHBuilderType builderType,
                     Viewer*              pViewer)
: BVH_PrimitiveSet<double, 3> (),
  m_fBoundingDiag             (0.0),
  m_pViewer                   (pViewer)
{
  this->init(model, builderType);
  this->MarkDirty();
}

//-----------------------------------------------------------------------------

BVHFacets::BVHFacets(const Handle(Poly_Triangulation)& mesh,
                     const BVHBuilderType              builderType,
                     Viewer*                           pViewer)
: BVH_PrimitiveSet<double, 3> (),
  m_fBoundingDiag             (0.0),
  m_pViewer                   (pViewer)
{
  this->init(mesh, builderType);
  this->MarkDirty();
}

//-----------------------------------------------------------------------------

BVHFacets::~BVHFacets()
{
}

//-----------------------------------------------------------------------------

void BVHFacets::SetViewer(Viewer* pViewer)
{
  m_pViewer = pViewer;
}

//-----------------------------------------------------------------------------

int BVHFacets::Size() const
{
  return (int) m_facets.size();
}

//-----------------------------------------------------------------------------

BVH_Box<double, 3> BVHFacets::Box(const int index) const
{
  BVH_Box<double, 3> box;
  const t_facet& facet = m_facets[index];

  box.Add(facet.P0);
  box.Add(facet.P1);
  box.Add(facet.P2);

  return box;
}

//-----------------------------------------------------------------------------

double BVHFacets::Center(const int index, const int axis) const
{
  const t_facet& facet = m_facets[index];

  if ( axis == 0 )
    return (1.0 / 3.0) * ( facet.P0.x() + facet.P1.x() + facet.P2.x() );
  else if ( axis == 1 )
    return (1.0 / 3.0) * ( facet.P0.y() + facet.P1.y() + facet.P2.y() );

  // The last possibility is "axis == 2"
  return (1.0 / 3.0) * ( facet.P0.z() + facet.P1.z() + facet.P2.z() );
}

//-----------------------------------------------------------------------------

void BVHFacets::Swap(const int index1, const int index2)
{
  std::swap(m_facets[index1], m_facets[index2]);
}

//-----------------------------------------------------------------------------

inline void BVHFacets::GetVertices(const int  index,
                                           BVH_Vec3d& vertex1,
                                           BVH_Vec3d& vertex2,
                                           BVH_Vec3d& vertex3) const
{
  const t_facet& facet = m_facets[index];

  vertex1 = facet.P0;
  vertex2 = facet.P1;
  vertex3 = facet.P2;
}

//-----------------------------------------------------------------------------

double BVHFacets::GetBoundingDiag() const
{
  return m_fBoundingDiag;
}

//-----------------------------------------------------------------------------

bool BVHFacets::init(const TopoDS_Shape&  model,
                     const BVHBuilderType builderType)
{
  if ( model.IsNull() )
    return false;

  // Prepare builder
  if ( builderType == BVHBuilder_Binned )
    myBuilder = new BVH_BinnedBuilder<double, 3, 32>(5, 32);
  else if ( builderType == BVHBuilder_Linear )
    myBuilder = new BVH_LinearBuilder<double, 3>(5, 32);

  // Explode shape on faces to get face indices
  if ( m_faces.IsEmpty() )
    TopExp::MapShapes(model, TopAbs_FACE, m_faces);

  // Initialize with facets taken from faces
  for ( int fidx = 1; fidx <= m_faces.Extent(); ++fidx )
  {
    const TopoDS_Face& face = TopoDS::Face( m_faces(fidx) );
    //
    if ( !this->addFace(face, fidx) )
      continue; // Do not return false, just skip as otherwise
                // BVH will be incorrect for faulty shapes!
  }

  // Calculate bounding diagonal
  Bnd_Box aabb;
  BRepBndLib::Add(model, aabb);
  //
  m_fBoundingDiag = ( aabb.CornerMax().XYZ() - aabb.CornerMin().XYZ() ).Modulus();

  return true;
}

//-----------------------------------------------------------------------------

bool BVHFacets::init(const Handle(Poly_Triangulation)& mesh,
                     const BVHBuilderType              builderType)
{
  // Prepare builder
  if ( builderType == BVHBuilder_Binned )
    myBuilder = new BVH_BinnedBuilder<double, 3, 32>(5, 32);
  else if ( builderType == BVHBuilder_Linear )
    myBuilder = new BVH_LinearBuilder<double, 3>(5, 32);

  // Initialize with the passed facets
  if ( !this->addTriangulation(mesh, TopLoc_Location(), -1, false) )
    return false;

  // Calculate bounding diagonal using fictive face to satisfy OpenCascade's API
  BRep_Builder BB;
  TopoDS_Face F;
  BB.MakeFace(F, mesh);
  Bnd_Box aabb;
  BRepBndLib::Add(F, aabb);
  //
  m_fBoundingDiag = ( aabb.CornerMax().XYZ() - aabb.CornerMin().XYZ() ).Modulus();

  return true;
}

//-----------------------------------------------------------------------------

bool BVHFacets::addFace(const TopoDS_Face& face,
                        const int          face_idx)
{
  TopLoc_Location loc;
  const Handle(Poly_Triangulation)& tris = BRep_Tool::Triangulation(face, loc);

  return this->addTriangulation( tris, loc, face_idx, (face.Orientation() == TopAbs_REVERSED) );
}

//-----------------------------------------------------------------------------

bool BVHFacets::addTriangulation(const Handle(Poly_Triangulation)& triangulation,
                                 const TopLoc_Location&            loc,
                                 const int                         face_idx,
                                 const bool                        isReversed)
{
  if ( triangulation.IsNull() )
    return false;

  // Internal collections of triangles and nodes
  Handle(Poly_HArray1OfTriangle) triangles = triangulation->MapTriangleArray();
  Handle(TColgp_HArray1OfPnt)    nodes     = triangulation->MapNodeArray();

  for ( int elemId = triangles->Lower(); elemId <= triangles->Upper(); ++elemId )
  {
    const Poly_Triangle& tri = triangles->Value(elemId);

    int n1, n2, n3;
    tri.Get(n1, n2, n3);

    gp_Pnt P0 = nodes->Value(isReversed ? n3 : n1);
    P0.Transform(loc);
    //
    gp_Pnt P1 = nodes->Value(n2);
    P1.Transform(loc);
    //
    gp_Pnt P2 = nodes->Value(isReversed ? n1 : n3);
    P2.Transform(loc);

    // Create a new facet
    t_facet facet(face_idx == -1 ? elemId : face_idx);

    // Initialize nodes
    facet.P0 = BVH_Vec3d( P0.X(), P0.Y(), P0.Z() );
    facet.P1 = BVH_Vec3d( P1.X(), P1.Y(), P1.Z() );
    facet.P2 = BVH_Vec3d( P2.X(), P2.Y(), P2.Z() );

    /* Initialize normal */

    gp_Vec V1(P0, P1);
    //
    if ( V1.SquareMagnitude() < 1e-8 )
    {
#if defined DRAW_DEBUG
      (*m_pViewer) << P0;
      (*m_pViewer) << P1;
      (*m_pViewer) << P2;
#endif

      std::cerr << "V1.SquareMagnitude() < epsilon." << std::endl;

      continue; // Skip invalid facet.
    }
    //
    V1.Normalize();

    gp_Vec V2(P0, P2);
    //
    if ( V2.SquareMagnitude() < 1e-8 )
    {
#if defined DRAW_DEBUG
      (*m_pViewer) << P0;
      (*m_pViewer) << P1;
      (*m_pViewer) << P2;
#endif

      std::cerr << "V2.SquareMagnitude() < epsilon." << std::endl;

      continue; // Skip invalid facet.
    }
    //
    V2.Normalize();

    // Compute norm
    facet.N = V1.Crossed(V2);
    //
    if ( facet.N.SquareMagnitude() < 1e-8 )
    {
#if defined DRAW_DEBUG
      (*m_pViewer) << P0;
      (*m_pViewer) << P1;
      (*m_pViewer) << P2;
#endif

      std::cerr << "facet.N.SquareMagnitude() < epsilon." << std::endl;

      continue; // Skip invalid facet
    }
    //
    facet.N.Normalize();

    // Store facet in the internal collection
    m_facets.push_back(facet);
  }

  return true;
}
