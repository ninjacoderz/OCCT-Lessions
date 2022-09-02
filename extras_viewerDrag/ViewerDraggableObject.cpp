//-----------------------------------------------------------------------------
// Created on: 14 June 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#include "ViewerDraggableObject.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

//-----------------------------------------------------------------------------

ViewerDraggableObject::ViewerDraggableObject(const TopoDS_Shape& shape)
: AIS_Shape(shape)
{
}

//-----------------------------------------------------------------------------

gp_Vec GetTranslationVector(const Handle(V3d_View)& view,
                            const Standard_Integer startX,
                            const Standard_Integer startY,
                            const Standard_Integer endX,
                            const Standard_Integer endY)
{
  gp_Vec translateVec;

  // start point
  Standard_Real viewX1, viewY1, viewZ1;
  view->Convert(startX, startY, viewX1, viewY1, viewZ1);

  // end point
  Standard_Real viewX2, viewY2, viewZ2;
  view->Convert(endX, endY, viewX2, viewY2, viewZ2);

  return gp_Vec(viewX2 - viewX1, viewY2 - viewY1, viewZ2 - viewZ1);
}

//-----------------------------------------------------------------------------

Standard_Boolean ViewerDraggableObject::ProcessDragging (const Handle(AIS_InteractiveContext)& context,
                                                            const Handle(V3d_View)&              view,
                                                            const Handle(SelectMgr_EntityOwner)& owner,
                                                            const Graphic3d_Vec2i&               dragFrom,
                                                            const Graphic3d_Vec2i&               dragTo,
                                                            const AIS_DragAction                 action)
{
  if (action == AIS_DragAction_Start)
  {
    m_startPosition = dragFrom;
    m_startTrsf = Transformation();
    m_startAttributes = Attributes();

    // hide detected presentation in the viewer as it will be changed on drag
    InteractiveContext()->ClearDetected();
    // set visual parameters into presentation as it's highlighted and recompute presentation
    SetAttributes(this->InteractiveContext()->HighlightStyle());
    InteractiveContext()->RecomputePrsOnly (this, false);
  }
  if (action == AIS_DragAction_Update)
  {
      gp_Trsf DirTrans;
      const Handle(V3d_View) view = this->InteractiveContext()->LastActiveView();
      gp_Vec translationVec = GetTranslationVector(view, m_startPosition.x(), m_startPosition.y(), dragTo.x(), dragTo.y());
      DirTrans.SetTranslation(translationVec);

      // unites the previuos and the current transformations
      gp_Trsf trsf = DirTrans * m_startTrsf;

      SetLocalTransformation(trsf);
  }
  else if (action == AIS_DragAction_Stop)
  {
    // restore initial visual attributes and recompute presentation
    SetAttributes(m_startAttributes);
    InteractiveContext()->RecomputePrsOnly (this, false);

    // recompute selection on the presentation
    InteractiveContext()->SelectionManager()->Update(this);

    // restore highlight in the viewer
    const Handle(V3d_View) view = this->InteractiveContext()->LastActiveView();
    InteractiveContext()->MoveTo(dragTo.x(), dragTo.y(), view, true);
  }
  return true;
}
