// The code below illustrates how to run Booleans on circles.
// Notice that intersections points are not returned as they are 1 dimension
// less than the Boolean operands.

  Handle(Geom_Circle) circle1 = new Geom_Circle(gp_Ax2(gp::Origin(), gp::DZ()), 5.0);
  Handle(Geom_Circle) circle2 = new Geom_Circle(gp_Ax2(gp_Pnt(3.0, 0.0, 0.0), gp::DZ()), 5.0);

  TopoDS_Shape shape1 = BRepBuilderAPI_MakeEdge(circle1);
  TopoDS_Shape shape2 = BRepBuilderAPI_MakeEdge(circle2);

  interp->GetPlotter().DRAW_SHAPE(shape1, Color_Red, "shape1");
  interp->GetPlotter().DRAW_SHAPE(shape2, Color_Red, "shape2");

  TopTools_ListOfShape args;
  TopTools_ListOfShape tools;
  args.Append(shape1);
  tools.Append(shape2);

  BRepAlgoAPI_Fuse bop;
  bop.SetArguments(args);
  bop.SetTools(tools);
  bop.Build();

  if (!bop.IsDone())
    return TCL_ERROR;

  TopoDS_Shape res = bop.Shape();
  interp->GetPlotter().DRAW_SHAPE(res, Color_Blue, "res");

  return TCL_OK;1