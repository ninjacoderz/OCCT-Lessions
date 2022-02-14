#pragma once

#include <AIS_InteractiveObject.hxx>

class Graphic3d_ArrayOfPoints;

class viewerLines_linePresentation : public AIS_InteractiveObject
{
public:
  viewerLines_linePresentation();

  void setPoints(const Handle(Graphic3d_ArrayOfPoints)& points);

  void ComputeSelection(const Handle(SelectMgr_Selection)& /*theSelection*/, const Standard_Integer /*theMode*/) override {}

protected:
  void Compute(const Handle(PrsMgr_PresentationManager)& prsMgr, const Handle(Prs3d_Presentation)& prs,
	const Standard_Integer mode) override;

protected:
  Handle(Graphic3d_ArrayOfPoints) m_points;
};
