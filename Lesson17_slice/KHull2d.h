//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Quaoar Studio
// All rights reserved.
//-----------------------------------------------------------------------------

#ifndef KHull2d_HeaderFile
#define KHull2d_HeaderFile

// OCCT includes
#include <TColStd_HSequenceOfInteger.hxx>

// Standard includes
#include <vector>

//-----------------------------------------------------------------------------
// Single point with additional information
//-----------------------------------------------------------------------------

//! Point enriched with additional information used in algorithms.
template <typename TCoord>
struct PointWithAttr
{
  //! Copy ctor.
  //! \param[in] P point to initialize this one from.
  PointWithAttr(const PointWithAttr& P) : Coord(P.Coord), Status(P.Status), Index(P.Index), pData(P.pData) {}

  //! Complete ctor.
  //! \param[in] _coord    spatial coordinates.
  //! \param[in] _status   status (deleted, not deleted).
  //! \param[in] _index    index of the point in external collection.
  //! \param[in] _data_ptr user data.
  PointWithAttr(const TCoord _coord    = TCoord(),
                const int    _status   = 0,
                const size_t _index    = 0,
                void*        _data_ptr = nullptr) : Coord(_coord), Status(_status), Index(_index), pData(_data_ptr) {}

  bool operator>  (const PointWithAttr&) const { return false; }
  bool operator== (const PointWithAttr&) const { return false; }

  //! Conversion operator to access coordinates only, without attributes.
  operator TCoord() const
  {
    return Coord;
  }

  TCoord Coord;  //!< Coordinates.
  int    Status; //!< General-purpose status.
  size_t Index;  //!< Index in the owning collection.
  void*  pData;  //!< Associated data.
};

//-----------------------------------------------------------------------------
// Point cloud
//-----------------------------------------------------------------------------

//! Point cloud where each element is enriched with metadata. This class is
//! to some extent similar to `asiAlgo_BaseCloud` (from Analysis Situs),
//! though, unlike the latter, it allows storing not only point coordinates
//! but also custom attributes. This class is best suited for moderate
//! amount of data because each point here is stored explicitly (plus
//! metadata it contains also significantly contributes to the memory
//! consumption).
//!
//! Note: interface similarity between this class and `asiAlgo_BaseCloud` is kept
//!       to allow for using common services like cloud purification, etc.
template <typename TCoord>
class PointWithAttrCloud : public Standard_Transient
{
public:

  //! Default ctor.
  PointWithAttrCloud() {}

public:

  //! Cleans up the point cloud.
  virtual void Clear()
  {
    m_pts.clear();
  }

public:

  //! \return number of contained elements (points).
  virtual int GetNumberOfElements() const
  {
    return int( m_pts.size() );
  }

  //! Returns the point cloud element by its zero-based index.
  //! \param[in] zeroBasedIndex index of the point to access.
  //! \return const reference to the contained point cloud element.
  virtual const PointWithAttr<TCoord>&
    GetElement(const int zeroBasedIndex) const
  {
    return m_pts[zeroBasedIndex];
  }

  //! Returns the point cloud element by its zero-based index.
  //! \param[in] zeroBasedIndex index of the point to access.
  //! \return non-const reference to the contained point cloud element.
  virtual PointWithAttr<TCoord>&
    ChangeElement(const int zeroBasedIndex)
  {
    return m_pts[zeroBasedIndex];
  }

  //! Adds another element to the point cloud.
  //! \param[in] coord element to add.
  virtual void AddElement(const PointWithAttr<TCoord>& coord)
  {
    m_pts.push_back(coord);
  }

protected:

  //! Internal collection of points with attributes.
  std::vector< PointWithAttr<TCoord> > m_pts;

};

//-----------------------------------------------------------------------------

//! Algorithm computing a non-convex hull of two-dimensional point cloud
//! using a k-nearest neighbors approach described in a paper of A. Moreira
//! and M.Y. Santos "Concave Hull: A K-nearest Neighbours Approach
//! for the Computation of the Region Occupied by a Set of Points".
//!
//! This algorithm generally works as a "blind rat" which follows
//! incognita path and knows nothing about its perspectives to reach
//! the end. Of course, such an approach cannot be truly effective as
//! the algorithm can stuck in a no-way situation (and will have to
//! re-select a "better" path from the very beginning). Such attempts are
//! hardly limited and, moreover, each new attempt makes the resulting
//! envelope coarser. However, we know for sure that this algorithm
//! demonstrates quite good results on many cases, especially, if the
//! cloud density is not very high.
//!
//! NOTE: there are infinite solutions for the problem of reconstruction of
//!       con-convex hulls. This one should not be suggested as the best one.
//!       It implements just one (quite simple) solution from a big variety
//!       of hypothetical choices.
template <typename TPoint>
class KHull2d
{
public:

  //! Initializes tool with the passed two-dimensional point cloud.
  //! \param[in] cloud      initial cloud.
  //! \param[in] k          number of neighbors to start calculation with.
  //! \param[in] limitIters limit for the number of iterations (0 is unlimited).
  KHull2d(const Handle(PointWithAttrCloud<TPoint>)& cloud,
          const int                                 k,
          const int                                 limitIters);

public:

  //! Constructs non-convex hull from the initial cloud.
  //! \return true in case of success, false -- otherwise.
  bool Perform();

public:

  //! Returns initial point cloud.
  //! \return initial point cloud.
  const Handle(PointWithAttrCloud<TPoint>)& GetInputPoints() const
  {
    return m_cloud;
  }

  //! Returns resulting non-convex hull.
  //! \return resulting non-convex hull.
  const Handle(PointWithAttrCloud<TPoint>)& GetHull() const
  {
    return m_hull;
  }

  //! Returns the current value of k.
  //! \return current value of k.
  int GetK() const
  {
    return m_iK;
  }

protected:

  //! Attempts to remove duplicated points from the cloud. Duplications
  //! are recognized with some user-specified precision.
  //!
  //! \param[in] cloud cloud to sparse (will be kept unchanged).
  //! \param[in] prec  precision to use for resolving coincident points.
  //! \return cloud after processing.
  Handle(PointWithAttrCloud<TPoint>)
    sparseCloud(const Handle(PointWithAttrCloud<TPoint>)& cloud,
                const double                              prec) const;

  //! Creates deep copy of the passed point cloud.
  //! \param[in] cloud point cloud to copy.
  //! \return deep copy of point cloud.
  Handle(PointWithAttrCloud<TPoint>)
    copyCloud(const Handle(PointWithAttrCloud<TPoint>)& cloud) const;

  //! Performs actual reconstruction of non-convex hull.
  //! \param[in,out] dataset working cloud.
  //! \return true in case of success, false -- otherwise.
  bool
    perform(Handle(PointWithAttrCloud<TPoint>)& dataset);

  //! Selects a point with minimal Y coordinate.
  //! \param[out] point     found point.
  //! \param[out] point_idx zero-based index of the found point.
  //! \param[in]  cloud     working point cloud.
  void
    findMinYPoint(PointWithAttr<TPoint>&                    point,
                  int&                                      point_idx,
                  const Handle(PointWithAttrCloud<TPoint>)& cloud) const;

  //! Adds the passed point to the target cloud.
  //! \param point [in]     point to add.
  //! \param cloud [in/out] target point cloud.
  //! \return index of the just added point in the target cloud.
  int
    addPoint(const PointWithAttr<TPoint>&        point,
             Handle(PointWithAttrCloud<TPoint>)& cloud);

  //! Removes point with the passed index from the target cloud.
  //! \param[in]     point_idx zero-based index of point to remove.
  //! \param[in,out] cloud     target point cloud.
  void
    removePoint(const int                           point_idx,
                Handle(PointWithAttrCloud<TPoint>)& cloud);

  //! Returns all points which are nearest to the given one. Only k nearest
  //! points are consulted.
  //! \param[in]     point_idx 0-based index of the point to find neighbors for.
  //! \param[in,out] k         number of neighbors to consult.
  //! \param[in,out] cloud     working point cloud.
  //! \return indices of the nearest points.
  std::vector<int>
    nearestPoints(const int                           point_idx,
                  int&                                k,
                  Handle(PointWithAttrCloud<TPoint>)& cloud);

  //! Checks whether the passed point belongs to the interior of the given
  //! polygon. This check is performed using simplest ray casting method.
  //! \param[in] point point to check.
  //! \param[in] poly  target polygon to check the point against.
  //! \return true if the point is inside, false -- otherwise.
  bool
    isPointInPolygon(const PointWithAttr<TPoint>&              point,
                     const Handle(PointWithAttrCloud<TPoint>)& poly) const;

  //! Checks whether the passed link intersects the given polygon.
  //! \param[in] P1   first point of the link.
  //! \param[in] P2   second point of the link.
  //! \param[in] poly polygon to check for intersections with.
  //! \return true/false.
  bool
    checkIntersections(const PointWithAttr<TPoint>&              P1,
                       const PointWithAttr<TPoint>&              P2,
                       const Handle(PointWithAttrCloud<TPoint>)& poly) const;

  //! Starting from the last link of the resulting polygon, checks the angles
  //! between this link and each point from the given list. The points in the
  //! list are then reordered, so those having greater angles come first.
  //!
  //! \param[in]     point_indices zero-based point indices to check.
  //! \param[in,out] dataset       initial dataset.
  //! \param[in]     poly          resulting polygon.
  //! \return indices of the nearest points in angle-descending order.
  std::vector<int>
    sortByAngle(const std::vector<int>&                   point_indices,
                Handle(PointWithAttrCloud<TPoint>)&       dataset,
                const Handle(PointWithAttrCloud<TPoint>)& poly) const;

private:

  int                                m_iK;          //!< Number of neighbors to consult.
  int                                m_iK_init;     //!< Initial value of k.
  int                                m_iK_limit;    //!< Max k number.
  int                                m_iLimitIters; //!< Limit for number of iterations.
  int                                m_iIterNum;    //!< Current iteration number.
  Handle(PointWithAttrCloud<TPoint>) m_cloud;       //!< Initial cloud.
  Handle(PointWithAttrCloud<TPoint>) m_hull;        //!< Resulting hull.

};

#endif
