//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#include "ViewerInteractor.h"
#include "Viewer.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_RubberBand.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <WNT_Window.hxx>

#include <QMouseEvent>

//-----------------------------------------------------------------------------

ViewerInteractor::ViewerInteractor(const Handle(AIS_InteractiveContext)& context)
: m_context(context),
  m_currentAction(CurrentAction_Nothing),
  m_movePointX(0),
  m_movePointY(0),
  m_pressPointX(0),
  m_pressPointY(0)
{
  m_rubberBand = new AIS_RubberBand (Quantity_NOC_LIGHTBLUE, Aspect_TOL_SOLID, Quantity_NOC_LIGHTBLUE4, 0.5, 1.0);
  m_rubberBand->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  m_rubberBand->SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_UPPER));
  m_rubberBand->SetDisplayMode (0);
  m_rubberBand->SetMutable (true);
}

//-----------------------------------------------------------------------------

void ViewerInteractor::fitAll()
{
  Handle(V3d_View) view = Viewer::activeView(m_context);
  if (view.IsNull())
    return;
  view->FitAll();
  view->Redraw();
}

//-----------------------------------------------------------------------------

void ViewerInteractor::zoom(const Standard_Real delta)
{
  Handle(V3d_View) view = Viewer::activeView(m_context);
  if (view.IsNull())
    return;
  view->SetZoom(delta);
  view->Redraw();
}

//-----------------------------------------------------------------------------

void ViewerInteractor::mousePressEvent(const Graphic3d_Vec2i& point,
                                       Aspect_VKeyMouse       keyMouse,
                                       Aspect_VKeyFlags       keyFlag)
{
  Handle(V3d_View) view = Viewer::activeView(m_context);
  if (view.IsNull())
    return;

  m_currentAction = CurrentAction_Nothing;
  m_pressPointX = point.x();
  m_pressPointY = point.y();
  m_movePointX = m_pressPointX;
  m_movePointY = m_pressPointY;
  if (keyMouse == Aspect_VKeyMouse_LeftButton)
  {
    if (m_context->HasDetected())
    {
      m_context->SelectDetected(keyFlag == Aspect_VKeyFlags_SHIFT ? AIS_SelectionScheme_Add : AIS_SelectionScheme_Replace);
      m_context->UpdateCurrentViewer();
    }
    else if (keyFlag == Aspect_VKeyFlags_SHIFT || keyFlag == Aspect_VKeyFlags_CTRL)
    {
      m_rubberBand->SetRectangle(m_pressPointX, -m_pressPointY, m_movePointX, -m_movePointY);
      m_context->Display(m_rubberBand, 0, -1, true, AIS_DS_Displayed);
      view->Invalidate();
    }
    else
    {
      view->StartRotation(m_pressPointX, m_pressPointY);
      m_currentAction = CurrentAction_Rotation;
    }
  }
  else if (keyMouse == Aspect_VKeyMouse_RightButton)
  {
    m_currentAction = CurrentAction_Zoom;
  }
  else if (keyMouse == Aspect_VKeyMouse_MiddleButton)
  {
    m_currentAction = CurrentAction_Pan;
  }
}

//-----------------------------------------------------------------------------

void ViewerInteractor::mouseMoveEvent(const Graphic3d_Vec2i& point,
                                      Aspect_VKeyMouse       /*keyMouse*/,
                                      Aspect_VKeyFlags       /*keyFlag*/)
{
  Handle(V3d_View) view = Viewer::activeView(m_context);
  if (view.IsNull())
    return;

  if (m_currentAction == CurrentAction_Nothing)
  {
    if (!m_rubberBand.IsNull() && m_rubberBand->HasInteractiveContext())
    {
      if (m_pressPointX != m_movePointX || m_pressPointY != m_movePointY)
      {
        m_rubberBand->SetToUpdate();
        m_rubberBand->SetRectangle(m_pressPointX, -m_pressPointY, m_movePointX, -m_movePointY);
        m_context->Display (m_rubberBand, 0, -1, Standard_True, AIS_DS_Displayed);
      }
    }
    else
    {
      // do highlight
      m_context->MoveTo(point.x(), point.y(), view, Standard_True);
    }
  }
  else if (m_currentAction == CurrentAction_Zoom)
  {
    view->Zoom(m_movePointX, m_movePointY, point.x(), point.y());
    m_movePointX = point.x();
    m_movePointY = point.y();
  }
  else if (m_currentAction == CurrentAction_Pan)
  {
    view->Pan(point.x() - m_movePointX, m_movePointY - point.y());
    m_movePointX = point.x();
    m_movePointY = point.y();
  }
  else if (m_currentAction == CurrentAction_Rotation)
  {
    view->Rotation(point.x(), point.y());
    view->Redraw();
  }
  m_movePointX = point.x();
  m_movePointY = point.y();
}

//-----------------------------------------------------------------------------

void ViewerInteractor::mouseReleaseEvent(const Graphic3d_Vec2i& /*point*/,
                                         Aspect_VKeyMouse       /*keyMouse*/,
                                         Aspect_VKeyFlags       keyFlag)
{
  Handle(V3d_View) view = Viewer::activeView(m_context);
  if (view.IsNull())
    return;

  if (m_currentAction == CurrentAction_Nothing || m_currentAction == CurrentAction_Rotation)
  {
    if (!m_rubberBand.IsNull() && m_rubberBand->HasInteractiveContext())
    {
      m_context->Remove (m_rubberBand, false);
      if (m_pressPointX != m_movePointX || m_pressPointY != m_movePointY)
      {
        m_context->SelectRectangle (Graphic3d_Vec2i (Min (m_pressPointX, m_movePointX), Min (m_pressPointY, m_movePointY)),
                                    Graphic3d_Vec2i (Max (m_pressPointX, m_movePointX), Max (m_pressPointY, m_movePointY)),
                                    view,
                                    AIS_SelectionScheme_Replace);
      }
      m_context->UpdateCurrentViewer();
    }
    else
    {
      // do deselect
      if (!m_context->HasDetected() && m_pressPointX == m_movePointX && m_pressPointY == m_movePointY)
      {
        m_context->SelectDetected(keyFlag == Aspect_VKeyFlags_SHIFT ? AIS_SelectionScheme_Add : AIS_SelectionScheme_Replace);
        m_context->UpdateCurrentViewer();
      }
    }
  }
  m_currentAction = CurrentAction_Nothing;
}
