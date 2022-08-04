// The code below illustrates how to run planar offsets for wires. Here's how it can be presented:
//
// 1. We construct C0 spline curve for a rectangle.
// 2. We see that offsets do not work, it's because of C0 (actually, G0) that should always be avoided.
// 3. We divide the curve by continuity to make 4 edges out of a single wire.
// 4. Now offsets work but only for the Arc mode because splines cannot be extended.
// 5. We make a polyline with BRepBuilderAPI_MakePolygon to get rid of splines.
// 5. Now offsets work in both Arc and Intersection modes because lines can easily be untrimmed and reintersected.
// 6. We can raise the degree of splines to see that offsets would work in the Arc mode for non-trivial geometries.

  std::vector<gp_XYZ> pts = { gp_XYZ(0,0,0), gp_XYZ(1,0,0), gp_XYZ(1,1,0), gp_XYZ(0,1,0), gp_XYZ(0,0,0) };
  Handle(Geom_BSplineCurve) bspl = asiAlgo_Utils::PolylineAsSpline(pts);

  bspl->IncreaseDegree(3);

  /*BRepBuilderAPI_MakePolygon mkPolygon;
  for ( const auto& p : pts )
    mkPolygon.Add(p);

  mkPolygon.Add(pts[0]);
  TopoDS_Wire W = mkPolygon.Wire();*/

  TopoDS_Wire W = BRepBuilderAPI_MakeWire( BRepBuilderAPI_MakeEdge(bspl) );

  // Divide by C1.
  asiAlgo_DivideByContinuity divider(nullptr, nullptr);
  divider.Perform(W, GeomAbs_C1, 0.1);

  interp->GetPlotter().REDRAW_SHAPE("W", W, Color_Red, 1., true);

  BRepOffsetAPI_MakeOffset mkOffset(W, GeomAbs_Arc, false);
  mkOffset.Build();
  mkOffset.Perform(Atof(argv[1]));
  const TopoDS_Shape& res = mkOffset.Shape();

  interp->GetPlotter().REDRAW_SHAPE("res", res, Color_Green, 1., true);

  return TCL_OK;