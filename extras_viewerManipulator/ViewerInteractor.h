//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#pragma once

#include <Aspect_VKeyFlags.hxx>
#include <Graphic3d_Vec2.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Handle.hxx>

class AIS_InteractiveContext;
class AIS_RubberBand;

//! Handler for key mouse events.
class ViewerInteractor
{
  //! Viewer actions.
  enum CurrentAction
  {
    CurrentAction_Nothing,  //!< no action
    CurrentAction_Zoom,     //!< zoom action
    CurrentAction_Pan,      //!< pan action
    CurrentAction_Rotation, //!< rotation action
    CurrentAction_Dragging  //!< drag action
  };

public:
  //! Contstructor.
  //! @param context viewer context
  ViewerInteractor(const Handle(AIS_InteractiveContext)& context);

  //! Set state whether the transformation of drag to be set into shape by mouse release.
  //! @param toApply flag to apply or not. The default value is false.
  void toApplyTransformation(const bool toApply) { m_applyTrsf = toApply; }

  //! Fit all scene.
  void fitAll();

  //! Zoom scene.
  //! @param delta the zoom step.
  void zoom(const Standard_Real delta);

  //! Processing mouse press event to activate viewer action.
  //! @param point    clicked point in pixels
  //! @param keyMouse mouse button (left, middle, right)
  //! @param keyFlag  key modifier like SHIFT, CTRL and so on
  virtual void mousePressEvent(const Graphic3d_Vec2i& point,
                               Aspect_VKeyMouse       keyMouse,
                               Aspect_VKeyFlags       keyFlag);

  //! Processing mouse move event to process viewer action.
  //! @param point    clicked point in pixels
  //! @param keyMouse mouse button (left, middle, right)
  //! @param keyFlag  key modifier like SHIFT, CTRL and so on
  virtual void mouseMoveEvent(const Graphic3d_Vec2i& point,
                              Aspect_VKeyMouse       keyMouse,
                              Aspect_VKeyFlags       keyFlag);

  //! Processing mouse press event to finalize viewer action.
  //! @param point    clicked point in pixels
  //! @param keyMouse mouse button (left, middle, right)
  //! @param keyFlag  key modifier like SHIFT, CTRL and so on
  virtual void mouseReleaseEvent(const Graphic3d_Vec2i& point,
                                 Aspect_VKeyMouse       keyMouse,
                                 Aspect_VKeyFlags       keyFlag);
private:
  Handle(AIS_InteractiveContext) m_context;       //!< viewer context
  Handle(AIS_RubberBand)         m_rubberBand;    //!< rubber-band presentation for rectangle selection
  Handle(SelectMgr_EntityOwner)  m_draggedOwner;  //!< dragged owner if current action is drag
  CurrentAction                  m_currentAction; //!< current viewer action
  Standard_Integer               m_pressPointX;   //!< point X position on mouse press
  Standard_Integer               m_pressPointY;   //!< point Y position on mouse press
  Standard_Integer               m_movePointX;    //!< previous point X position on mouse move
  Standard_Integer               m_movePointY;    //!< previous point Y position on mouse move
  bool                           m_applyTrsf;     //!< flag whether the local transformation is set to shape.
};
