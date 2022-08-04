// The code below illustrates how to run planar offsets for wires. Here's how it can be presented:
//
// 1. Interpolate curved spline and connect it to a straight edge segment: offsets work.
// 2. Keep one straight edge: offsets won't work.
// 3. Show the bug in offset implementation for one edge.
// 4. Show how to workaround the issue by constructing a fictive face.
// 5. Show that GeomAbs_Intersection works nicely on non-closed hexagonal contours.

Handle(Geom_Plane) pln = new Geom_Plane(gp::XOY());
  std::vector<gp_XYZ> pts = { gp_XYZ(0,0,0), gp_XYZ(1,0,0), gp_XYZ(1,1,0), gp_XYZ(0,1,0) };
  /*Handle(Geom_BSplineCurve) bspl = asiAlgo_Utils::PolylineAsSpline(pts);
   bspl->IncreaseDegree(3);*/


  gp_XY poles[6] = {gp_XY(10, 0),
          gp_XY(5.000000000000001, 8.660254037844386),
 gp_XY(-4.999999999999998, 8.660254037844387),
gp_XY(-10, 1.2246467991473533e-15),
gp_XY(-5.000000000000004, -8.660254037844386),
gp_XY(5.000000000000001, -8.660254037844386)};


  //gp_XY poles[6];
  //asiAlgo_Utils::HexagonPoles( gp::Origin2d().XY(), 10, poles[0], poles[1], poles[2], poles[3], poles[4], poles[5] );

  //Handle(TColgp_HArray1OfPnt) hpts = new TColgp_HArray1OfPnt(1, (int) pts.size());
  ////
  //int idx = 0;
  //for ( const auto& p : pts )
  //  hpts->ChangeValue(++idx) = p;

  //GeomAPI_Interpolate mkCurve(hpts, false, Precision::Confusion());
  //mkCurve.Perform();

  //const Handle(Geom_BSplineCurve)& bspl = mkCurve.Curve();

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(gp_Pnt(-1,0,0), pts[0]);
  //TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(bspl);
  //TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(pts[0], pts[0] /*+ gp_XYZ(0.01,0.01,0)*/);

  BRepBuilderAPI_MakePolygon mkPolygon;
  for ( const auto& p : poles )
    mkPolygon.Add( pln->Value(p.X(), p.Y() ) );
  //
  //mkPolygon.Add( pln->Value(poles[0].X(), poles[0].Y() ) );

  TopoDS_Wire W = mkPolygon.Wire();

 /* BRepBuilderAPI_MakeWire mkWire;
  mkWire.Add(e1);*/
  //mkWire.Add(e2);
  //mkWire.Add(e3);
  //TopoDS_Wire W = mkWire.Wire();

  TopoDS_Face F = BRepBuilderAPI_MakeFace(pln->Pln(), W, false);


  // Divide by C1.
  //asiAlgo_DivideByContinuity divider(nullptr, nullptr);
  //divider.Perform(W, GeomAbs_C1, 0.1);

  //interp->GetPlotter().REDRAW_SHAPE("F", F, Color_White);
  interp->GetPlotter().REDRAW_SHAPE("W", W, Color_Red, 1., true);

  BRepOffsetAPI_MakeOffset mkOffset(W, GeomAbs_Intersection, false);
  //mkOffset.Build();
  mkOffset.Perform(Atof(argv[1]));
  const TopoDS_Shape& res = mkOffset.Shape();

  interp->GetPlotter().REDRAW_SHAPE("res", res, Color_Green, 1., true);

  return TCL_OK;
