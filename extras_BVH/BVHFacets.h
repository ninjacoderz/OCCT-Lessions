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

#ifndef BVHFacets_h
#define BVHFacets_h

// OCCT includes
#include <BVH_Types.hxx>
#include <BVH_PrimitiveSet.hxx>
#include <NCollection_Vector.hxx>
#include <Poly_Triangulation.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------

//! BVH construction algorithm.
enum BVHBuilderType
{
  BVHBuilder_Binned = 0,
  BVHBuilder_Linear
};

//-----------------------------------------------------------------------------

//! BVH-based accelerating structure representing CAD model's
//! facets in computations.
class BVHFacets : public BVH_PrimitiveSet<double, 3>
{
public:

  //! Structure representing a single link.
  struct t_link
  {
    t_link()               : EdgeIndex(-1) {}
    t_link(const int eidx) : EdgeIndex(eidx) {}

    BVH_Vec3d P0, P1;    //!< Segment nodes.
    int       EdgeIndex; //!< Index of the host edge.
  };

  //! Structure representing a single facet.
  struct t_facet
  {
    t_facet()               : FaceIndex(-1) {}
    t_facet(const int fidx) : FaceIndex(fidx) {}

    BVH_Vec3d P0, P1, P2; //!< Triangle nodes.
    gp_Vec    N;          //!< Cached normal calculated by nodes.
    int       FaceIndex;  //!< Index of the host face.
  };

public:

  //! Creates the accelerating structure with immediate initialization.
  //! \param[in] model       the CAD model to create the accelerating structure for.
  //! \param[in] builderType the type of the builder to use.
  BVHFacets(const TopoDS_Shape&  model,
            const BVHBuilderType builderType = BVHBuilder_Binned);

  //! Creates the accelerating structure with immediate initialization.
  //! \param[in] mesh        the triangulation to create the accelerating structure for.
  //! \param[in] builderType the type of the builder to use.
  //! \param[in] progress    the progress notifier.
  //! \param[in] plotter     the imperative plotter.
  BVHFacets(const Handle(Poly_Triangulation)& mesh,
            const BVHBuilderType              builderType = BVHBuilder_Binned);

  //! Dtor.
  virtual
    ~BVHFacets();

public:

  //! \return number of stored facets.
  virtual int
    Size() const override;

  //! Builds an elementary box for a facet with the given index.
  //! \param[in] index index of the facet of interest.
  //! \return AABB for the facet of interest.
  virtual BVH_Box<double, 3>
    Box(const int index) const override;

  //! Calculates center point of a facet with respect to the axis of interest.
  //! \param[in] index index of the facet of interest.
  //! \param[in] axis  axis of interest.
  //! \return center parameter along the straight line.
  virtual double
    Center(const int index,
           const int axis) const override;

  //! Swaps two elements for BVH building.
  //! \param[in] index1 first index.
  //! \param[in] index2 second index.
  virtual void
    Swap(const int index1,
         const int index2) override;

public:

  //! Returns vertices for a facet with the given 0-based index.
  void
    GetVertices(const int  index,
                BVH_Vec3d& vertex1,
                BVH_Vec3d& vertex2,
                BVH_Vec3d& vertex3) const;

  //! \return characteristic diagonal of the full model.
  double
    GetBoundingDiag() const;

public:

  //! Sets the map of faces to use.
  //! \param[in] faces the map of faces to set.
  void SetMapOfFaces(const TopTools_IndexedMapOfShape& faces)
  {
    m_faces = faces;
  }

  //! \return the constructed map of faces.
  const TopTools_IndexedMapOfShape& GetMapOfFaces() const
  {
    return m_faces;
  }

  //! Returns a facet by its 0-based index.
  //! \param[in] index index of the facet of interest.
  //! \return requested facet.
  const t_facet& GetFacet(const int index)
  {
    return m_facets[index];
  }

  //! \return AABB of the entire set of objects.
  virtual BVH_Box<double, 3> Box() const
  {
    BVH_Box<double, 3> aabb;
    const int size = this->Size();

    for ( int i = 0; i < size; ++i )
    {
      aabb.Combine( this->Box(i) );
    }
    return aabb;
  }

protected:

  //! Initializes the accelerating structure with the given CAD model.
  //! \param[in] model       the CAD model to prepare the accelerating structure for.
  //! \param[in] builderType the type of the builder to use.
  //! \return true in case of success, false -- otherwise.
  bool
    init(const TopoDS_Shape&  model,
         const BVHBuilderType builderType);

  //! Initializes the accelerating structure with the given triangulation.
  //! \param[in] model       the triangulation to prepare the accelerating structure for.
  //! \param[in] builderType the type of the builder to use.
  //! \return true in case of success, false -- otherwise.
  bool
    init(const Handle(Poly_Triangulation)& mesh,
         const BVHBuilderType              builderType);

  //! Adds face to the accelerating structure.
  //! \param[in] face     face to add.
  //! \param[in] face_idx index of the face being added.
  //! \return true in case of success, false -- otherwise.
  bool
    addFace(const TopoDS_Face& face,
            const int          face_idx);

  //! Adds triangulation to the accelerating structure.
  //! \param[in] triangulation triangulation to add.
  //! \param[in] loc           location to apply.
  //! \param[in] face_idx      index of the corresponding face being.
  //! \param[in] isReversed    true if the original B-rep face is reversed.
  //! \return true in case of success, false -- otherwise.
  bool
    addTriangulation(const Handle(Poly_Triangulation)& triangulation,
                     const TopLoc_Location&            loc,
                     const int                         face_idx,
                     const bool                        isReversed);

protected:

  //! Map of faces constructed by the BVH builder.
  TopTools_IndexedMapOfShape m_faces;

  //! Array of facets.
  std::vector<t_facet> m_facets;

  //! Characteristic size of the model.
  double m_fBoundingDiag;

};

#endif
