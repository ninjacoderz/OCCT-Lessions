#include "viewerLines_linePresentation.h"

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>

#include <Prs3d_LineAspect.hxx>

viewerLines_linePresentation::viewerLines_linePresentation()
{
  Attributes()->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1));
}

void viewerLines_linePresentation::setPoints(const Handle(Graphic3d_ArrayOfPoints)& points)
{
  m_points = points;
}

void viewerLines_linePresentation::Compute(const Handle(PrsMgr_PresentationManager)& prsMgr,
  const Handle(Prs3d_Presentation)& prs, const Standard_Integer mode)
{
  if (m_points.IsNull())
	return;

  const Standard_Integer nbVertices = m_points->VertexNumber();
  Handle(Graphic3d_ArrayOfPolylines) prims = new Graphic3d_ArrayOfPolylines(nbVertices);
  for (Standard_Integer pointsIt = 1; pointsIt <= nbVertices; ++pointsIt)
  {
	prims->AddVertex(m_points->Vertice(pointsIt));
  }

  Handle(Graphic3d_Group) group = prs->NewGroup();
  group->SetGroupPrimitivesAspect(Attributes()->LineAspect()->Aspect());
  group->AddPrimitiveArray(prims);
}
