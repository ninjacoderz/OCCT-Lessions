// Local includes
#include "Viewer.h"

// OpenCascade includes
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Circ.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

namespace
{
  bool IsCircular(const TopoDS_Wire& wire,
                  gp_Circ&           circ)
  {
    TopoDS_Iterator it(wire);
    const TopoDS_Edge& edge = TopoDS::Edge( it.Value() );

    double f, l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, f, l);

    if ( curve->IsKind( STANDARD_TYPE(Geom_Circle) ) )
    {
      circ = Handle(Geom_Circle)::DownCast(curve)->Circ();
      return true;
    }

    if ( curve->IsInstance( STANDARD_TYPE(Geom_TrimmedCurve) ) )
    {
      Handle(Geom_TrimmedCurve) tcurve = Handle(Geom_TrimmedCurve)::DownCast(curve);
      //
      if ( tcurve->BasisCurve()->IsKind( STANDARD_TYPE(Geom_Circle) ) )
      {
        circ = Handle(Geom_Circle)::DownCast( tcurve->BasisCurve() )->Circ();
        return true;
      }
    }

    return false;
  }

}

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  // Read curves.
  BRep_Builder bbuilder;
  TopoDS_Shape shape;
  //
  if ( !BRepTools::Read(shape, "D:/Demos/lessons/Lesson22_modify-shape/data/shell.brep", bbuilder) )
  {
    std::cout << "Error: cannot read shape from file." << std::endl;
    return 1;
  }

  Handle(BRepTools_ReShape)
    ctx = new BRepTools_ReShape;

  for ( TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( exp.Current() );

    TopTools_IndexedMapOfShape allWires;
    TopExp::MapShapes(face, TopAbs_WIRE, allWires);
    //
    if ( allWires.Extent() == 1 )
      continue;

    TopoDS_Wire innerWire;
    TopoDS_Wire outerWire = BRepTools::OuterWire(face);

    for ( int w = 1; w <= allWires.Extent(); ++w )
    {
      if ( allWires(w).IsPartner(outerWire) )
        continue;

      innerWire = TopoDS::Wire( allWires(w) );
    }
    //
    if ( innerWire.IsNull() )
      continue;

    gp_Circ circProps;
    //
    if ( ::IsCircular(innerWire, circProps) )
    {
      circProps.SetRadius( circProps.Radius() * 4. );
      ctx->Replace(innerWire, BRepBuilderAPI_MakeWire( BRepBuilderAPI_MakeEdge( new Geom_Circle(circProps) ) ));
    }
  }

  shape = ctx->Apply(shape);

  vout << shape;

  vout.StartMessageLoop();
  return 0;
}
