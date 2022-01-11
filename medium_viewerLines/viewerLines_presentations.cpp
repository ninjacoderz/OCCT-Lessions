#include "viewerLines_presentations.h"
#include "viewerLines_linePresentation.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Shape.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools.hxx>

void addChain(const gp_Pnt startPoint, const int pointsCount, const int deltaX, const int deltaY,
  NCollection_List<gp_Pnt>& points)
{
  if (pointsCount > 0)
    points.Append(startPoint.XYZ() + gp_Pnt(deltaX, 0., 0.).XYZ());
  if (pointsCount > 1)
    points.Append(startPoint.XYZ() + gp_Pnt(deltaX, -1. * deltaY, 0.).XYZ());
  if (pointsCount > 2)
    points.Append(startPoint.XYZ() + gp_Pnt(0., -1. * deltaY, 0.).XYZ());
  if (pointsCount > 3)
    points.Append(startPoint.XYZ() + gp_Pnt(0., -2. * deltaY, 0.).XYZ());
}

NCollection_List<gp_Pnt> viewerLines_presentations::generatePointsOfLines(const int nbLines)
{
  int deltaX = 10;
  int deltaY = 1;

  int spanX = 5;
  int maxYCoordinate = -500;

  NCollection_List<gp_Pnt> points;
  gp_Pnt startPoint;
  points.Append(gp_Pnt(0., 0., 0.));

  startPoint = points.Last();
  for (int i = 0; i < 100000; i++) {
    addChain(startPoint, 4, deltaX, deltaY, points);
    startPoint = points.Last();

    if (startPoint.Y() <= maxYCoordinate) {
      gp_Pnt movedPoint(startPoint.X() + deltaX + spanX, startPoint.Y(), 0.);
      points.Append(movedPoint);

      movedPoint.SetY(0.);
      points.Append(movedPoint);

      startPoint = gp_Pnt(movedPoint.X() + deltaY, movedPoint.Y(), 0.);
    }
    if (points.Size() - 1 >= nbLines)
      break;
  }
  return points;
}

TopoDS_Shape buildEdges(const NCollection_List<gp_Pnt>& points, const Standard_Integer timerIndex = -1)
{
  TopoDS_Shape shape;
  BRep_Builder builder;
  TopoDS_Compound compound;
  builder.MakeCompound(compound);

  NCollection_List<gp_Pnt>::Iterator it(points);
  gp_Pnt prevPoint = it.Value(), curPoint;
  if (!it.More())
    return compound;

  it.Next();
  int index = 1;
  for (; it.More(); it.Next()) {
    if (timerIndex != -1 && timerIndex <= index++)
      break;
    curPoint = it.Value();
    builder.Add(compound, BRepBuilderAPI_MakeEdge(prevPoint, curPoint));
    prevPoint = curPoint;
  }
  return compound;
}


Handle(AIS_InteractiveObject) viewerLines_presentations::presentation(const Handle(AIS_InteractiveContext)& context,
  const Standard_CString typeName)
{
  AIS_ListOfInteractive displayedObjects;
  context->DisplayedObjects(displayedObjects);
  for (AIS_ListOfInteractive::Iterator it(displayedObjects); it.More(); it.Next())
  {
    if (it.Value()->DynamicType()->Name() == typeName)
      return it.Value();
  }
  return Handle(AIS_InteractiveObject)();
}

Handle(AIS_InteractiveObject) viewerLines_presentations::createTrihedron()
{
  Handle(Geom_Axis2Placement) placement = new Geom_Axis2Placement(gp_Pnt(0.0, 0.0, 0.0), gp::DZ(), gp::DX());
  Handle(AIS_Trihedron) trihedron = new AIS_Trihedron(placement);
  trihedron->SetTextColor(Quantity_NOC_BLACK);

  return trihedron;
}

Handle(AIS_InteractiveObject) viewerLines_presentations::createShapePresentation()
{
  return new AIS_Shape(TopoDS_Shape());
}

Handle(AIS_InteractiveObject) viewerLines_presentations::createCustomPresentation()
{
  return new viewerLines_linePresentation();
}

void viewerLines_presentations::updateCustomPresentation(const Handle(AIS_InteractiveObject)& prs, const NCollection_List<gp_Pnt>& points,
  const Standard_Integer timerIndex)
{
  Handle(viewerLines_linePresentation) customPrs = Handle(viewerLines_linePresentation)::DownCast(prs);
  if (!customPrs.IsNull()) {
    Standard_Integer nbPoints = points.Extent();
    Handle(Graphic3d_ArrayOfPoints) arrayPoints = new Graphic3d_ArrayOfPoints(nbPoints, Graphic3d_ArrayFlags_None);
    int index = 0;
    for (NCollection_List<gp_Pnt>::Iterator it(points); it.More(); it.Next(), index++) {
      if (timerIndex != -1 && timerIndex <= index)
        break;
      arrayPoints->AddVertex(it.Value());
    }
    customPrs->setPoints(arrayPoints);
  }
  else if (!Handle(AIS_Shape)::DownCast(prs).IsNull()) {
    Handle(AIS_Shape)::DownCast(prs)->Set(buildEdges(points, timerIndex));
  }
 }
