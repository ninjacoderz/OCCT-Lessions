// Local includes
#include "Viewer.h"

// OpenCascade includes
#include <Geom_Plane.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <GeomAbs_JoinType.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

Handle(Geom2d_BSplineCurve)
  PolylineAsSpline(const std::vector<gp_XY>& trace)
{
  // Initialize properties for spline trajectories
  TColgp_Array1OfPnt2d poles( 1, (int) trace.size() );
  //
  for ( int k = 0; k < int( trace.size() ); ++k )
  {
    poles(k + 1) = gp_Pnt2d( trace[k] );
  }

  const int n = poles.Upper() - 1;
  const int p = 1;
  const int m = n + p + 1;
  const int k = m + 1 - (p + 1)*2;

  // Knots
  TColStd_Array1OfReal knots(1, k + 2);
  knots(1) = 0;
  //
  for ( int j = 2; j <= k + 1; ++j )
    knots(j) = knots(j-1) + 1.0 / (k + 1);
  //
  knots(k + 2) = 1;

  // Multiplicities
  TColStd_Array1OfInteger mults(1, k + 2);
  mults(1) = 2;
  //
  for ( int j = 2; j <= k + 1; ++j )
    mults(j) = 1;
  //
  mults(k + 2) = 2;

  return new Geom2d_BSplineCurve(poles, knots, mults, 1);
}

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  Handle(Geom_Plane) pln = new Geom_Plane( gp::XOY() );

  std::vector<gp_XY> poles = {
    gp_XY(0, 0),
    gp_XY(10, 0),
    gp_XY(10, 10),
    gp_XY(0, 10),
    gp_XY(0, 0)
  };

  BRepBuilderAPI_MakeWire mkWire;
  mkWire.Add( BRepBuilderAPI_MakeEdge( PolylineAsSpline( {poles[0], poles[1]} ), pln ) );
  mkWire.Add( BRepBuilderAPI_MakeEdge( PolylineAsSpline( {poles[1], poles[2]} ), pln ) );
  mkWire.Add( BRepBuilderAPI_MakeEdge( PolylineAsSpline( {poles[2], poles[3]} ), pln ) );
  mkWire.Add( BRepBuilderAPI_MakeEdge( PolylineAsSpline( {poles[3], poles[0]} ), pln ) );
  TopoDS_Wire W = mkWire.Wire();

  vout << W;

  BRepOffsetAPI_MakeOffset mkOffset(W, GeomAbs_Arc, false);
  mkOffset.Perform(3);

  if ( mkOffset.IsDone() )
    vout << mkOffset.Shape();

  vout.StartMessageLoop();
  return 0;
}
