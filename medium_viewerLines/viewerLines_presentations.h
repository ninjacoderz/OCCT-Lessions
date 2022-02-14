#pragma once

#include <AIS_InteractiveObject.hxx>
#include <TopoDS_Shape.hxx>

class viewerLines_presentations
{
public:
  viewerLines_presentations() {}

  static NCollection_List<gp_Pnt> generatePointsOfLines(const int nbLines);

  static Handle(AIS_InteractiveObject) presentation(const Handle(AIS_InteractiveContext)& context, const Standard_CString typeName);

  static Handle(AIS_InteractiveObject) createTrihedron();
  static Handle(AIS_InteractiveObject) createShapePresentation();
  static Handle(AIS_InteractiveObject) createCustomPresentation();

  static void updateCustomPresentation(const Handle(AIS_InteractiveObject)& prs, const NCollection_List<gp_Pnt>& points, const Standard_Integer timerIndex);
};
