// Exercise includes
#include "Viewer.h"

// OpenCascade includes
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Geom_Plane.hxx>
#include <Intf_InterferencePolygon2d.hxx>
#include <Intf_Polygon2d.hxx>

//-----------------------------------------------------------------------------

//! The derived polygon class.
class SimplePolygon : public Intf_Polygon2d
{
public:

  static Handle(Geom_Plane) PLANE;

public:

  //! Ctor with initializer list.
  SimplePolygon(const std::initializer_list<std::pair<double, double>>& poles)
  {
    for ( const auto& P : poles )
    {
      gp_Pnt2d P2d(P.first, P.second);

      // If performance is an issue, avoid using dynamically
      // growing collections like vectors, etc.
      m_poles.push_back( gp_Pnt2d(P.first, P.second) );

      // One thing which is pretty inconvenient is the necessity to
      // update the AABB of a polygon manually. If you forget doing that,
      // the intersection check will return nothing.
      myBox.Add(P2d);
    }
  }

public:

  //! Returns the tolerance of the polygon.
  virtual double DeflectionOverEstimation() const
  {
    return Precision::Confusion();
  }

  //! Returns the number of segments in the polyline.
  virtual int NbSegments() const
  {
    return int( m_poles.size() - 1 );
  }

  //! Returns the points of the segment <index> in the Polygon.
  virtual void Segment(const int index, gp_Pnt2d& beg, gp_Pnt2d& end) const
  {
    beg = m_poles[index - 1];
    end = m_poles[index];
  }

public:

  //! Conversion operator.
  operator TopoDS_Shape()
  {
    BRepBuilderAPI_MakePolygon mkShape;
    //
    for ( const auto& P : m_poles )
    {
      mkShape.Add( PLANE->Value( P.X(), P.Y() ) );
    }

    return mkShape.Shape();
  }

protected:
 
  std::vector<gp_Pnt2d> m_poles;

};

Handle(Geom_Plane) SimplePolygon::PLANE;

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  SimplePolygon::PLANE = new Geom_Plane( gp::XOY() );

  SimplePolygon poly1 = { {0.0, 0.0}, {1.0,  0.0}, {0.25, 0.75} };
  SimplePolygon poly2 = { {0.5, 1.2}, {0.5, -1.0}, {1.0, 3.0} };

  vout << poly1 << poly2;

  Intf_InterferencePolygon2d algo(poly1, poly2);
  const int numPts = algo.NbSectionPoints();

  std::cout << "Num. of intersections: " << numPts << std::endl;

  for ( int isol = 1; isol <= numPts; ++isol )
  {
    gp_Pnt2d sol = algo.Pnt2dValue(isol);
    gp_Pnt   P   = SimplePolygon::PLANE->Value( sol.X(), sol.Y() );

    vout << BRepBuilderAPI_MakeVertex(P);
  }

  vout.StartMessageLoop();

  return 0;
}
