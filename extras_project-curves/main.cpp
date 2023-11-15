// Local includes
#include "Viewer.h"

// OpenCascade includes
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <GeomProjLib.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <BRepLib.hxx>

//-----------------------------------------------------------------------------

Handle(Geom_Curve) ProjectEdge(Handle(Geom_Curve)& c3d,
                               const Handle(Geom_Surface)& surf)
{
  ShapeConstruct_ProjectCurveOnSurface projector;
  projector.Init( surf, Precision::Confusion() );

  // Curve on surface.
  Handle(Geom2d_Curve) cons;
  //
  if ( !projector.Perform(c3d, c3d->FirstParameter(), c3d->LastParameter(), cons) )
  {
    return nullptr;
  }

  TopoDS_Edge tmpEdge = BRepBuilderAPI_MakeEdge(cons, surf);

  if ( !BRepLib::BuildCurve3d(tmpEdge, Max(1.e-5, BRep_Tool::Tolerance(tmpEdge) ), GeomAbs_C1, 3, 10) )
  {
    return nullptr;
  }

  double f, l;
  return BRep_Tool::Curve(tmpEdge, f, l);
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  // Read face.
  BRep_Builder bbuilder;
  TopoDS_Shape face;
  if ( !BRepTools::Read(face, "D:/Demos/lessons/Lesson22_project-curves/data/surface.brep", bbuilder) )
  {
    return 1;
  }
  //
  vout << face;

  // Read curves (edges) to project.
  TopoDS_Shape edgesComp;
  if ( !BRepTools::Read(edgesComp, "D:/Demos/lessons/Lesson22_project-curves/data/curves.brep", bbuilder) )
  {
    return 1;
  }
  //
  //vout << edgesComp;

  Handle(Geom_Surface) surf = BRep_Tool::Surface( TopoDS::Face(face) );

  TopoDS_Compound projComp;
  bbuilder.MakeCompound(projComp);

  for ( TopExp_Explorer exp(edgesComp, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    double f, l;
    Handle(Geom_Curve) c3d = BRep_Tool::Curve( TopoDS::Edge( exp.Current() ), f, l );

    Handle(Geom_Curve) projC3d = ProjectEdge(c3d, surf);

    TopoDS_Edge projEdge = BRepBuilderAPI_MakeEdge(projC3d);

    bbuilder.Add(projComp, projEdge);

    vout << BRepBuilderAPI_MakeEdge(projC3d);
  }

  BRepTools::Write(projComp, "C:/users/user/desktop/projEdges.brep");

  // D:/Demos/lessons/Lesson22_project-curves/data/curves.brep
  // D:/Demos/lessons/Lesson22_project-curves/data/surface.brep

  vout.StartMessageLoop();
  return 0;
}
