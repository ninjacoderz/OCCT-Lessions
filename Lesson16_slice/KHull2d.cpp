//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Quaoar Studio
// All rights reserved.
//-----------------------------------------------------------------------------

// Own include
#include "KHull2d.h"

// OCCT includes
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_CellFilter.hxx>
#include <NCollection_Vector.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

// STL includes
#include <algorithm>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------
// Two-dimensional inspector for cloud purification
//-----------------------------------------------------------------------------

//! Inspector tool (see OCCT Cell Filter) for purification of
//! points which are coincident with a certain tolerance.
template <typename TPoint>
class PointWithAttrInspector2d
{
public:

  //! Cloud dimension.
  enum { Dimension = 2 };

  //! Point index is used as a cell ID.
  typedef int Target;

  //! Point type.
  typedef TPoint Point;

  //! Constructor accepting the tolerance.
  //! \param[in] tol    tolerance to set.
  //! \param[in] points working collection of points.
  PointWithAttrInspector2d(const double                              tol,
                           const Handle(PointWithAttrCloud<TPoint>)& points)
  : m_fTol       (tol),
    m_pointCloud (points)
  {}

  //! Sets current point to search for coincidence.
  //! \param[in] target current target.
  //! \param[in] P      point coordinates to set as current.
  void SetCurrent(const int target, const TPoint& P)
  {
    m_iCurrentTarget = target;
    m_current        = P;
  }

  //! Cleans up the list of resulting indices.
  void ClearIndices2Purge()
  {
    m_indices2Purge.Clear();
  }

  //! Returns the list of indices falling to the current cell.
  //! \return list of indices.
  const TColStd_PackedMapOfInteger& GetIndices2Purge() const
  {
    return m_indices2Purge;
  }

  //! Implementation of inspection method.
  //! \param[in] target target.
  //! \return verdict (remove or keep).
  NCollection_CellFilter_Action Inspect(const int target)
  {
    if ( m_iCurrentTarget != target )
    {
      TPoint       P     = m_pointCloud->GetElement(target);
      const double delta = (P - m_current).Modulus();

      if ( delta < m_fTol )
      {
        m_indices2Purge.Add(target);
        return CellFilter_Purge;
      }
    }

    return CellFilter_Keep;
  }

  //! Auxiliary method to shift point coordinates by the given value.
  //! Useful for preparing a points range for Inspect() with tolerance.
  TPoint Shift(const TPoint& P, double tol) const
  {
    TPoint pt(P);
    pt.SetX(P.X() + tol);
    pt.SetY(P.Y() + tol);
    //
    return pt;
  }

  //! Returns coordinates.
  //! \param[in] i 0-based index of coordinate.
  //! \param[in] P point to access coordinates for.
  //! \return requested coordinate.
  static double Coord(int i, const TPoint& P)
  {
    return P.Coord(i + 1);
  }

private:

  double                             m_fTol;           //!< Linear tolerance to use.
  TColStd_PackedMapOfInteger         m_indices2Purge;  //!< Indices to exclude.
  Handle(PointWithAttrCloud<TPoint>) m_pointCloud;     //!< Source point cloud.
  TPoint                             m_current;        //!< Current point.
  int                                m_iCurrentTarget; //!< Current target.

};

//-----------------------------------------------------------------------------
// Purification algorithm
//-----------------------------------------------------------------------------

//! Auxiliary functions for purification of point clouds. These tools
//! allow reducing the number of points by getting rid of those which
//! fall into the given tolerant sphere.
class PurifyCloud
{
public:

  //! Default ctor.
  PurifyCloud() = default;

public:

  template<typename HCloudType,
           typename InspectorType>
  void PerformCommon(const double              tol,
                     const Handle(HCloudType)& source,
                     Handle(HCloudType)&       result)
  {
    // Check if there are any points to purify.
    const int source_size = source->GetNumberOfElements();
    if ( !source_size )
      return;

    // Populate cell filter with the source points.
    NCollection_CellFilter<InspectorType> CellFilter(tol);
    InspectorType Inspector(tol, source);
    TColStd_PackedMapOfInteger allIndices;
    //
    for ( int p_idx = 0; p_idx < source_size; ++p_idx )
    {
      auto P = source->GetElement(p_idx);
      CellFilter.Add(p_idx, P);
      //
      allIndices.Add(p_idx);
    }

    // Now classify each point to know which cell it belongs to.
    TColStd_PackedMapOfInteger traversed;
    int                        next_idx     = 0;
    bool                       allTraversed = false;
    //
    do
    {
      if ( traversed.Contains(next_idx) )
        continue;
      else
        traversed.Add(next_idx);

      // Classify next point of the source cloud. Notice that Shift() is always
      // done at Precision::Confusion() value in order for the cell filter
      // to work properly.
      auto P = source->GetElement(next_idx);
      //
      Inspector.SetCurrent(next_idx, P);
      auto P_min = Inspector.Shift( P, -Precision::Confusion() );
      auto P_max = Inspector.Shift( P,  Precision::Confusion() );
      //
      CellFilter.Inspect(P_min, P_max, Inspector);

      // Get indices of unvisited points.
      TColStd_PackedMapOfInteger nonTraversedIndices;
      //
      nonTraversedIndices.Subtraction( allIndices, traversed );
      nonTraversedIndices.Subtract( Inspector.GetIndices2Purge() );
      //
      if ( nonTraversedIndices.IsEmpty() )
        allTraversed = 1;
      else
        next_idx = nonTraversedIndices.GetMinimalMapped();
    }
    while ( !allTraversed );

    //----------------------------------------------------
    // Copy remaining points to another (resulting) cloud
    //----------------------------------------------------

    const TColStd_PackedMapOfInteger& indices2Purge = Inspector.GetIndices2Purge();
    const int indices_size = indices2Purge.Extent();
    //
    if ( !indices_size )
      result = source;
    else
    {
      // Prepare result.
      if ( result.IsNull() )
        result = new HCloudType;

      // Populate.
      for ( int i = 0; i < source_size; ++i )
      {
        if ( indices2Purge.Contains(i) )
          continue;

        result->AddElement( source->GetElement(i) );
      }
    }
  }

};

//-----------------------------------------------------------------------------

// Instantiate for allowed types
template class PointWithAttrCloud<gp_XY>;
template class KHull2d<gp_XY>;

//-----------------------------------------------------------------------------
// Custom data associated with each point in a cloud
//-----------------------------------------------------------------------------

//! Custom data associated with each point.
struct t_point_data
{
  t_point_data() : Dist(0.0), Idx(0) {}
  t_point_data(const double _d, const int _i) : Dist(_d), Idx(_i) {}

  double Dist; //!< Distance.
  double Ang;  //!< Angle.
  int    Idx;  //!< Zero-based index of point in the initial cloud.
};

//-----------------------------------------------------------------------------

//! Comparator for points by their associated distances.
template <typename TPoint>
class DistComparator
{
public:

  DistComparator() = delete;
  DistComparator& operator=(const DistComparator&) = delete;
  DistComparator(const std::vector< PointWithAttr<TPoint> >& points) : m_points(points) {}

  bool operator()(const PointWithAttr<TPoint>& left, const PointWithAttr<TPoint>& right)
  {
    const double l = (*((t_point_data*)(left.pData))).Dist;
    const double r = (*((t_point_data*)(right.pData))).Dist;

    return (l < r);
  }

public:

  const std::vector< PointWithAttr<TPoint> >& m_points; //!< Collection of points.

};

//-----------------------------------------------------------------------------

//! Comparator for points by their associated angles.
template <typename TPoint>
class AngleComparator
{
public:

  AngleComparator() = delete;
  AngleComparator& operator=(const AngleComparator&) = delete;
  AngleComparator(const std::vector< PointWithAttr<TPoint> >& points) : m_points(points) {}

  bool operator()(const PointWithAttr<TPoint>& left, const PointWithAttr<TPoint>& right)
  {
    const double l = (*((t_point_data*)(left.pData))).Ang;
    const double r = (*((t_point_data*)(right.pData))).Ang;

    return (l < r);
  }

public:

  const std::vector< PointWithAttr<TPoint> >& m_points; //!< Collection of points.

};

//-----------------------------------------------------------------------------

template <typename TPoint>
KHull2d<TPoint>::KHull2d(const Handle(PointWithAttrCloud<TPoint>)& cloud,
                         const int                                 k,
                         const int                                 limitIters)
{
  m_cloud       = cloud;
  m_iK          = k;
  m_iK_init     = k;
  m_iK_limit    = 25; // We do not want to fall into stack overflow.
  m_iLimitIters = limitIters;
  m_iIterNum    = 0;
}

//-----------------------------------------------------------------------------
// Interface methods
//-----------------------------------------------------------------------------

template <typename TPoint>
bool KHull2d<TPoint>::Perform()
{
  // ...
  // Input: list of points to process (m_cloud);
  //        number of neighbors (m_iK).
  //
  // Output: an ordered list of points representing the computed
  //         polygon (m_hull).
  // ...

  // Check initial cloud.
  if ( !m_cloud->GetNumberOfElements() )
  {
    std::cout << "Error: Empty point cloud." << std::endl;
    return false;
  }

  // Check if k (number of consulted neighbors) is sufficient.
  if ( m_iK < 2 )
  {
    std::cout << "Warning: Too small value of K." << std::endl;
    m_iK = 2;
  }

  // Check if k does not exceed the limit.
  if ( m_iK > m_iK_limit )
  {
    std::cout << "Warning: Value of K exceeds the limit." << std::endl;
    m_iK = m_iK_limit;
  }

  // Remove coincident points from the cloud.
  m_cloud = this->sparseCloud( m_cloud, Precision::Confusion() );
  //
  const int nPoints = m_cloud->GetNumberOfElements();

  // Nullify number of iterations.
  m_iIterNum = 0;

  // Check if cloud is degenerated.
  if ( nPoints <= 3 )
  {
    m_hull = m_cloud;
    return true;
  }

  // Build hull.
  Handle(PointWithAttrCloud<TPoint>) dataset = this->copyCloud(m_cloud);
  //
  return this->perform(dataset);
}

//-----------------------------------------------------------------------------

template <typename TPoint>
Handle(PointWithAttrCloud<TPoint>)
  KHull2d<TPoint>::sparseCloud(const Handle(PointWithAttrCloud<TPoint>)& cloud,
                               const double                              prec) const
{
  Handle(PointWithAttrCloud<TPoint>) res;

  // Sparse cloud using the appropriate type of inspector.
  PurifyCloud sparser;
  //
  sparser.PerformCommon< PointWithAttrCloud<TPoint>,
                         PointWithAttrInspector2d<TPoint> >(prec, cloud, res);

  return res;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
Handle(PointWithAttrCloud<TPoint>)
  KHull2d<TPoint>::copyCloud(const Handle(PointWithAttrCloud<TPoint>)& cloud) const
{
  Handle(PointWithAttrCloud<TPoint>) res = new PointWithAttrCloud<TPoint>;

  for ( int idx = 0; idx < cloud->GetNumberOfElements(); ++idx )
  {
    PointWithAttr<TPoint> P( cloud->GetElement(idx) );
    P.Status = 0;
    P.pData  = nullptr;

    // Add to result.
    res->AddElement(P);
  }

  return res;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
bool KHull2d<TPoint>::perform(Handle(PointWithAttrCloud<TPoint>)& dataset)
{
  // Make sure that k neighbors are available.
  m_iK = Min(m_iK, dataset->GetNumberOfElements() - 1);

  // Check if we reached the limit in the number of consulted neighbors.
  if ( m_iK > m_iK_limit )
  {
    std::cout << "Value of K reached the limit." << std::endl;
    return true; // Let's return at least anything (result is normally not empty by this stage).
  }

  // Start from beginning (with the new or initial K).
  if ( m_hull.IsNull() )
    m_hull = new PointWithAttrCloud<TPoint>;
  else
    m_hull->Clear();

  /* ==================================
   *  Perform reconstruction of a hull
   * ================================== */

  // Point with minimal Y coordinate.
  PointWithAttr<TPoint> firstPoint;
  int firstPointIdx;
  //
  this->findMinYPoint(firstPoint, firstPointIdx, dataset);

#if defined DRAW_DEBUG
  m_plotter.DRAW_POINT(firstPoint.Coord, Color_Green, "seed_pt");
#endif

  // Add the first point to the resulting cloud.
  this->addPoint(firstPoint, m_hull);

  // Remove the traversed point from the working cloud.
  this->removePoint(firstPointIdx, dataset);

  // Working variables.
  PointWithAttr<TPoint> currentPoint(firstPoint);
  int currentPointIdx = firstPointIdx;
  int step            = 2;

  // Main loop.
  while ( ( step == 2 || (currentPoint.Coord - firstPoint.Coord).SquareModulus() > gp::Resolution() ) && dataset->GetNumberOfElements() )
  {
    m_iIterNum++;
    if ( m_iIterNum == m_iLimitIters )
    {
      std::cout << "Algorithm reached the limit of iterations (max "
                << m_iLimitIters << " iterations allowed)." << std::endl;
      return true; // Just halt with a positive result.
    }

    // Check if all points are inside.
    bool allInside = true;
    int  j         = m_cloud->GetNumberOfElements();
    //
    while ( allInside && j )
    {
      --j;
      allInside = this->isPointInPolygon(m_cloud->GetElement(j), m_hull);
    }
    if ( allInside )
      return true;

    if ( step == 5 )
    {
      // Now we can restore the first point as we got far enough from it.
      dataset->ChangeElement(firstPointIdx).Status = 0;
    }

    // Find k neighbors.
    std::vector<int>
      kNearestPoints_indices = this->nearestPoints(currentPointIdx, m_iK, dataset);

    // Sort the candidates in descending order by right-hand rule.
    std::vector<int>
      cPoints_indices = this->sortByAngle(kNearestPoints_indices, dataset, m_hull);

    if ( !cPoints_indices.size() )
    {
      std::cout << "Error: Cannot find nearest points." << std::endl;
      return false;
    }

    /* ==============================
     *  Check for self-intersections
     * ============================== */

    bool its          = true;
    int  i            = 0;
    int  cPoint_index = 0;

    while ( its && ( i < int( cPoints_indices.size() ) ) )
    {
      cPoint_index = cPoints_indices.at(i++);
      const PointWithAttr<TPoint>& cPoint = dataset->GetElement(cPoint_index);

      // Check against the currently traversed path.
      its = this->checkIntersections(currentPoint, cPoint, m_hull);
    }

    if ( its ) // If there are some intersections, then we are in a trouble...
    {
      // Increase k and try again.
      const int prevK = m_iK;
      //
      m_iK = m_iK_init++ + 1;

      // Info.
      std::cout << "Incrementing K because of intersections (" << prevK
                << " increased to " << m_iK << ")." << std::endl;

      Handle(PointWithAttrCloud<TPoint>) copyDS = this->copyCloud(m_cloud);
      return this->perform(copyDS);
    }

    if ( cPoint_index == firstPointIdx )
      break;

    /* ==================================
     *  Choose the next point to proceed
     * ================================== */

    // Set working variables.
    currentPoint    = dataset->GetElement(cPoint_index);
    currentPointIdx = cPoint_index;
    ++step;

    // Adjust working clouds.
    this->addPoint(currentPoint, m_hull);
    this->removePoint(cPoint_index, dataset);
  }

  /* =========================================
   *  Check that all points have been covered
   * ========================================= */

  bool allInside = true;
  int  i         = m_cloud->GetNumberOfElements();

  while ( allInside && i )
  {
    --i;
    allInside = this->isPointInPolygon(m_cloud->GetElement(i), m_hull);
  }

  if ( !allInside ) // If there are some points still, then we are in a trouble...
  {
    // Increase k and try again.
    const int prevK = m_iK;
    //
    m_iK = m_iK_init++ + 1;

    // Info.
    std::cout << "Incrementing K because of incomplete coverage (" << prevK
              << " increased to " << m_iK << ")." << std::endl;

    Handle(PointWithAttrCloud<TPoint>) copyDS = this->copyCloud(m_cloud);
    return this->perform(copyDS);
  }

  return true; // Success.
}

//-----------------------------------------------------------------------------

template <typename TPoint>
void KHull2d<TPoint>::findMinYPoint(PointWithAttr<TPoint>&                    point,
                                    int&                                      point_idx,
                                    const Handle(PointWithAttrCloud<TPoint>)& cloud) const
{
  // Working variables.
  double minY = RealLast();

  // Let's do simple loop.
  for ( int idx = 0; idx < cloud->GetNumberOfElements(); ++idx )
  {
    const PointWithAttr<TPoint>& nextPoint = cloud->GetElement(idx);
    //
    if ( nextPoint.Coord.Y() < minY )
    {
      minY      = nextPoint.Coord.Y();
      point     = nextPoint;
      point_idx = idx;
    }
  }
}

//-----------------------------------------------------------------------------

template <typename TPoint>
int KHull2d<TPoint>::addPoint(const PointWithAttr<TPoint>&        point,
                              Handle(PointWithAttrCloud<TPoint>)& cloud)
{
  cloud->AddElement(point);

#if defined DRAW_DEBUG
  m_plotter.DRAW_POINT(point.Coord, Color_Blue, "next_pt");
#endif

  return cloud->GetNumberOfElements();
}

//-----------------------------------------------------------------------------

template <typename TPoint>
void KHull2d<TPoint>::removePoint(const int                           point_idx,
                                  Handle(PointWithAttrCloud<TPoint>)& cloud)
{
  cloud->ChangeElement(point_idx).Status = 1; // Signaled.
}

//-----------------------------------------------------------------------------

template <typename TPoint>
std::vector<int>
  KHull2d<TPoint>::nearestPoints(const int                           point_idx,
                                 int&                                k,
                                 Handle(PointWithAttrCloud<TPoint>)& cloud)
{
  // WARNING: currently implemented approach is suboptimal

  // Access seed point.
  const PointWithAttr<TPoint>& S = cloud->GetElement(point_idx);

  /* =====================================
   *  Build distance field for each point
   * ===================================== */

  // Fill collections to use sorting.
  std::vector< PointWithAttr<TPoint> > PointRefs;

  // Calculate Euclidian distances for all points.
  const int nPoints = cloud->GetNumberOfElements();
  for ( int idx = 0; idx < nPoints; ++idx )
  {
    if ( idx == point_idx )
      continue;

    PointWithAttr<TPoint>& P = cloud->ChangeElement(idx);

    if ( P.Status > 0 ) // Signaled state (i.e., removed point) -> skip.
      continue;

    // Calculate Euclidian distance.
    const double dist2 = (P.Coord - S.Coord).SquareModulus();

    if ( !P.pData )
      P.pData = new t_point_data;

    (*( (t_point_data*) (P.pData) )).Dist = dist2;
    (*( (t_point_data*) (P.pData) )).Idx  = idx;

    PointRefs.push_back(P);
  }

  /* ===========================================
   *  Sort points by their associated distances
   * =========================================== */

  // Sort points by distance.
  std::sort( PointRefs.begin(), PointRefs.end(), DistComparator<TPoint>(PointRefs) );

  k = Min( k, int( PointRefs.size() ) );

  std::vector<int> res;
  for ( int i = 0; i < k; ++i )
  {
    const int idx = (*( (t_point_data*) (PointRefs[i].pData) )).Idx;
    res.push_back(idx);
  }

  /* ======================
   *  Finalize calculation
   * ====================== */

  // Release allocated heap memory.
  for ( int idx = 0; idx < cloud->GetNumberOfElements(); ++idx )
  {
    PointWithAttr<TPoint>& P = cloud->ChangeElement(idx);
    delete P.pData; P.pData = nullptr;
  }

  return res;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
bool KHull2d<TPoint>::isPointInPolygon(const PointWithAttr<TPoint>&              point,
                                       const Handle(PointWithAttrCloud<TPoint>)& poly) const
{
  // Working variables.
  const double x      = point.Coord.X();
  const double y      = point.Coord.Y();
  const int    nVerts = poly->GetNumberOfElements();
  bool         sign   = false;
  const double eps    = gp::Resolution();

  // Loop over the polygon vertices.
  for ( int i = 0; i < nVerts; ++i )
  {
    // Access next segment of the polygon.
    const int     j  = ((i == nVerts - 1) ? 0 : i + 1);
    const TPoint& Pi = poly->GetElement(i).Coord;
    const TPoint& Pj = poly->GetElement(j).Coord;

    // Check if sample point is coincident with pole.
    if ( (point.Coord - Pi).SquareModulus() < eps )
      return true;

    // Coordinates of poles.
    const double xi = Pi.X();
    const double xj = Pj.X();
    const double yi = Pi.Y();
    const double yj = Pj.Y();

    // Check if sample point belongs to a horizontal segment. If so,
    // we are Ok to proceed.
    if ( Abs(y - yi) > eps || Abs(y - yj) > eps )
    {
      // Check if current link has a chance to be intersected by a horizontal
      // ray passing from the sample point.
      if ( (y < yi) == (y < yj) )
        continue;

      // If link is a candidate, count only those which are located on the right.
      const double f = (xj - xi)*(y - yi)/(yj - yi) + xi;
      if ( x < f )
        sign = !sign;
    }
    else // Point is on horizontal segment.
    {
      // Let's count links which the sample point does not belong to.
      if ( (x < xi) && (x < xj) )
        sign = !sign;
    }
  }

  return sign;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
bool KHull2d<TPoint>::checkIntersections(const PointWithAttr<TPoint>&              P1,
                                         const PointWithAttr<TPoint>&              P2,
                                         const Handle(PointWithAttrCloud<TPoint>)& poly) const
{
  const double eps = gp::Resolution();

  // Number of vertices.
  const int nVerts = poly->GetNumberOfElements();
  //
  if ( nVerts <= 1 )
    return false; // There cannot be intersection with just one point.

  // Create OCCT segment for (P1,P2).
  Handle(Geom2d_TrimmedCurve) L = GCE2d_MakeSegment(P1.Coord, P2.Coord);

  // Loop over the polygon vertices.
  for ( int i = 0; i < nVerts; ++i )
  {
    // Access next segment of the polygon.
    const int     j  = ((i == nVerts - 1) ? 0 : i + 1);
    const TPoint& Pi = poly->GetElement(i).Coord;
    const TPoint& Pj = poly->GetElement(j).Coord;

    if ( (P1.Coord - Pi).SquareModulus() < eps ||
         (P1.Coord - Pj).SquareModulus() < eps ||
         (P2.Coord - Pi).SquareModulus() < eps ||
         (P2.Coord - Pj).SquareModulus() < eps )
      continue; // No intersection can happen with adjacent segments.

    // Create OCCT segment for link.
    Handle(Geom2d_TrimmedCurve) LL = GCE2d_MakeSegment(Pi, Pj);

    // Check for intersections.
    Geom2dAPI_InterCurveCurve IntCC(L, LL);
    if ( IntCC.NbPoints() )
    {
#if defined DRAW_DEBUG
      m_plotter.DRAW_LINK(P1.Coord, P2.Coord, Color_Red, "ilink_a");
      m_plotter.DRAW_LINK(Pi,       Pj,       Color_Red, "ilink_b");
#endif
      return true;
    }
  }

  return false; // No intersections found.
}

//-----------------------------------------------------------------------------

template <typename TPoint>
std::vector<int>
  KHull2d<TPoint>::sortByAngle(const std::vector<int>&                   point_indices,
                               Handle(PointWithAttrCloud<TPoint>)&       dataset,
                               const Handle(PointWithAttrCloud<TPoint>)& poly) const
{
  const int     nPoints = poly->GetNumberOfElements();
  const TPoint& PLast   = poly->GetElement(nPoints - 1).Coord;

  // Last link of the polygon.
  gp_Dir2d seedDir;
  if ( nPoints == 1 )
    seedDir = -gp::DX2d();
  else
    seedDir = poly->GetElement(nPoints - 2).Coord - PLast;

  /* ================================
   *  Calculate angles between links
   * ================================ */

  // Prepare collections for sorting.
  std::vector< PointWithAttr<TPoint> > PointRefs;

  // Loop over the candidate links.
  for ( int i = 0; i < int( point_indices.size() ); ++i )
  {
    // Next candidate link.
    const int              next_idx = point_indices.at(i);
    PointWithAttr<TPoint>& P        = dataset->ChangeElement(next_idx);
    gp_Dir2d               PNext    = P.Coord - PLast;

    // Check angle.
    double ang = gp_Vec2d(PNext).Angle(seedDir);
    //
    if ( Abs(ang) < 1.e-6 )
      ang = 0;
    else if ( ang < 0 )
      ang += 2*M_PI;

    if ( ang > 0 )
    {
      if ( !P.pData )
        P.pData = new t_point_data;

      (*( (t_point_data*) (P.pData) )).Ang = ang;
      (*( (t_point_data*) (P.pData) )).Idx = next_idx;

      PointRefs.push_back(P);
    }
  }

  /* ========================================
   *  Sort points by their associated angles
   * ======================================== */

  // Sort points by angles.
  std::sort( PointRefs.begin(), PointRefs.end(), AngleComparator<TPoint>(PointRefs) );

  std::vector<int> res;
  for ( int i = 0; i < int( PointRefs.size() ); ++i ) // TODO: the order depends on orientation (!)
  {
    const int idx = (*( (t_point_data*) (PointRefs[i].pData) )).Idx;
    res.push_back( (int) idx );
  }

  /* ======================
   *  Finalize calculation
   * ====================== */

  // Release allocated heap memory.
  for ( int idx = 0; idx < int( point_indices.size() ); ++idx )
  {
    PointWithAttr<TPoint>& P = dataset->ChangeElement(idx);
    delete P.pData; P.pData = nullptr;
  }

  return res;
}
