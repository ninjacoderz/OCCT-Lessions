// Below is illustrated how to intersect edges without BOP.

  //Handle(Geom_Circle) circle1 = new Geom_Circle(gp_Ax2(gp::Origin(), gp::DZ()), 5.0);
  //Handle(Geom_Circle) circle2 = new Geom_Circle(gp_Ax2(gp_Pnt(3.0, 0.0, 0.0), gp::DZ()), 5.0);
  //TopoDS_Edge shape1 = BRepBuilderAPI_MakeEdge(circle1);
  //TopoDS_Edge shape2 = BRepBuilderAPI_MakeEdge(circle2);

Handle(Geom_Line) line1 = new Geom_Line(gp::Origin(), gp::DZ());
  Handle(Geom_Line) line2 = new Geom_Line(gp::Origin(), gp::DZ());

  TopoDS_Edge shape1 = BRepBuilderAPI_MakeEdge(line1, gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, 10.0));
  TopoDS_Edge shape2 = BRepBuilderAPI_MakeEdge(line2, gp_Pnt(0.0, 0.0, 5.0), gp_Pnt(0.0, 0.0, 15.0));

  interp->GetPlotter().DRAW_SHAPE(shape1, Color_Red, "shape1");
  interp->GetPlotter().DRAW_SHAPE(shape2, Color_Red, "shape2");

  IntTools_EdgeEdge intersector(shape1, shape2);
  intersector.Perform();
  intersector.UseQuickCoincidenceCheck(false);
  if (!intersector.IsDone())
  {
    return TCL_ERROR;
  }

  const IntTools_SequenceOfCommonPrts& cprts = intersector.CommonParts();
  int nbResults = cprts.Length();

  TopoDS_Compound comp;
  BRep_Builder builder;
  builder.MakeCompound(comp);

  for (int index = 1; index <= nbResults; ++index)
  {
    const IntTools_CommonPrt& cpart = cprts(index);

    const TopoDS_Edge& aE1 = cpart.Edge1();
    const TopoDS_Edge& aE2 = cpart.Edge2();

    TopAbs_ShapeEnum type = cpart.Type();
    switch (type)
    {
      case TopAbs_EDGE:
      {
        const IntTools_Range& range = cpart.Range1();
        double t1 = range.First();
        double t2 = range.Last();

        Standard_Real f, l;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(aE1, f, l);

        Handle(Geom_TrimmedCurve) trimmedCurve =
          new Geom_TrimmedCurve(Handle(Geom_Curve)::DownCast(curve->Copy()), std::min(t1, t2), std::max(t1, t2));
        TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(trimmedCurve);
        builder.Add(comp, newEdge);

        break;
      }
      case TopAbs_VERTEX:
      {
        double t1 = 0.0;
        double t2 = 0.0;
        IntTools_Tools::VertexParameters(cpart, t1, t2);

        gp_Pnt newPnt;
        Standard_Real f, l;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(aE1, f, l);
        curve->D0(t1, newPnt);

        TopoDS_Vertex newV = BRepBuilderAPI_MakeVertex(newPnt);
        builder.Add(comp, newV);

        break;
      }
      default:
        return TCL_ERROR;
    }
  }

  interp->GetPlotter().REDRAW_SHAPE("res", comp, Color_Blue);

  return TCL_OK;