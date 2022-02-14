#include "viewerLines_scene.h"
#include "viewerLines_presentations.h"

#include <AIS_InteractiveContext.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <WNT_Window.hxx>

#include <QMouseEvent>

viewerLines_scene::viewerLines_scene()
: m_currentAction(CurrentAction_Nothing), m_movePointX(0), m_movePointY(0), m_pressPointX(0), m_pressPointY(0)
{
}

Handle(AIS_InteractiveContext) viewerLines_scene::createContext(Handle(V3d_Viewer) viewer)
{
  Handle(AIS_InteractiveContext) context = new AIS_InteractiveContext(viewer);
  const Handle(Prs3d_Drawer)& hiStyle = context->HighlightStyle();
  hiStyle->SetColor(Quantity_NOC_CYAN1);
  context->UpdateCurrentViewer();

  return context;
}

Handle(V3d_Viewer) viewerLines_scene::createViewer()
{
  Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
  Handle(OpenGl_GraphicDriver) graphicDriver = new OpenGl_GraphicDriver(displayConnection);

  Handle(V3d_Viewer) viewer = new V3d_Viewer(graphicDriver);
  viewer->SetDefaultLights();
  viewer->SetLightOn();

  return viewer;
}

Handle(V3d_View) viewerLines_scene::createView(const Handle(AIS_InteractiveContext)& m_context, Aspect_Handle windowHandle)
{
  Handle(V3d_View) view = m_context->CurrentViewer()->CreateView();

  Handle(Aspect_Window) wnd = new WNT_Window(windowHandle);
  view->SetWindow(wnd);
  if (!wnd->IsMapped())
    wnd->Map();

  view->SetBackgroundColor(Quantity_Color(Quantity_NOC_WHITE));// Quantity_NOC_BLACK));
  view->MustBeResized();

  return view;
}

Handle(AIS_InteractiveContext) viewerLines_scene::context() const
{
  return m_context;
}

void viewerLines_scene::setContext(const Handle(AIS_InteractiveContext)& ctx)
{
  m_context = ctx;
}

void viewerLines_scene::redrawView()
{
  Handle(V3d_View) view = activeView();
  if (view.IsNull())
    return;
  view->Redraw();
}

void viewerLines_scene::resizeView()
{
  Handle(V3d_View) view = activeView();
  if (view.IsNull())
    return;
  view->MustBeResized();
}

void viewerLines_scene::fitAll()
{
  Handle(V3d_View) view = activeView();
  if (view.IsNull())
    return;
  view->FitAll();
  view->Redraw();
}

void viewerLines_scene::setTop()
{
  Handle(V3d_View) view = activeView();
  if (view.IsNull())
    return;
  view->SetProj(V3d_TypeOfOrientation_Zup_Top);
  view->Redraw();
}

void viewerLines_scene::displayPresentation(const Handle(AIS_InteractiveObject)& presentation, const int mode)
{
  if (m_context.IsNull())
    return;

  m_context->Display(presentation, mode, -1 /*do not activate selection*/, Standard_True);
}

void viewerLines_scene::redisplayPresentation(const Handle(AIS_InteractiveObject)& presentation)
{
  if (m_context.IsNull())
    return;

  m_context->Redisplay(presentation, Standard_True);
}

void viewerLines_scene::setDisplayMode(const Handle(AIS_InteractiveObject)& presentation, int displayMode)
{
  if (m_context.IsNull() || presentation.IsNull())
    return;

  m_context->SetDisplayMode(presentation, displayMode, Standard_True);
}

void viewerLines_scene::erasePresentation(const Handle(AIS_InteractiveObject)& presentation)
{
  if (m_context.IsNull() || presentation.IsNull())
    return;

  m_context->Erase(presentation, Standard_True);
}

void viewerLines_scene::activatePresentation(const Handle(AIS_InteractiveObject)& presentation, int selectionMode)
{
  if (m_context.IsNull() || presentation.IsNull())
    return;

  m_context->Activate(presentation, selectionMode);
}

void viewerLines_scene::deactivatePresentation(const Handle(AIS_InteractiveObject)& presentation)
{
  if (m_context.IsNull() || presentation.IsNull())
    return;

  m_context->ClearSelected(Standard_True);
  m_context->Deactivate(presentation);
}

void viewerLines_scene::mousePressEvent(const Graphic3d_Vec2i& point, Aspect_VKeyMouse keyMouse, Aspect_VKeyFlags keyFlag)
{
  m_currentAction = CurrentAction_Nothing;
  m_pressPointX = point.x();
  m_pressPointY = point.y();
  m_movePointX = m_pressPointX;
  m_movePointY = m_pressPointY;
  if (keyMouse == Aspect_VKeyMouse_LeftButton) {
    if (m_context->HasDetected()) {
      m_context->SelectDetected(keyFlag == Aspect_VKeyFlags_SHIFT ? AIS_SelectionScheme_Add : AIS_SelectionScheme_Replace);
      m_context->UpdateCurrentViewer();
    }
    else {
      activeView()->StartRotation(m_pressPointX, m_pressPointY);
      m_currentAction = CurrentAction_Rotation;
    }
  }
  else if (keyMouse == Aspect_VKeyMouse_RightButton) {
    m_currentAction = CurrentAction_Zoom;
  }
  else if (keyMouse == Aspect_VKeyMouse_MiddleButton) {
    m_currentAction = CurrentAction_Pan;
  }
}

void viewerLines_scene::mouseMoveEvent(const Graphic3d_Vec2i& point, Aspect_VKeyMouse /*keyMouse*/, Aspect_VKeyFlags /*keyFlag*/)
{
  Handle(V3d_View) view = activeView();
  if (view.IsNull())
    return;

  if (m_currentAction == CurrentAction_Nothing) {
    // do highlight
    m_context->MoveTo(point.x(), point.y(), view, Standard_True);
  }
  else if (m_currentAction == CurrentAction_Zoom) {
    view->Zoom(m_movePointX, m_movePointY, point.x(), point.y());
    m_movePointX = point.x();
    m_movePointY = point.y();
  }
  else if (m_currentAction == CurrentAction_Pan) {
    view->Pan(point.x() - m_movePointX, m_movePointY - point.y());
    m_movePointX = point.x();
    m_movePointY = point.y();
  }
  else if (m_currentAction == CurrentAction_Rotation) {
    view->Rotation(point.x(), point.y());
    view->Redraw();
  }
  m_movePointX = point.x();
  m_movePointY = point.y();
}


void viewerLines_scene::mouseReleaseEvent(const Graphic3d_Vec2i& /*point*/, Aspect_VKeyMouse /*keyMouse*/, Aspect_VKeyFlags keyFlag)
{
  if (m_currentAction == CurrentAction_Rotation) {
    // do deselect
    if (!m_context->HasDetected() && m_pressPointX == m_movePointX && m_pressPointY == m_movePointY) {
      m_context->SelectDetected(keyFlag == Aspect_VKeyFlags_SHIFT ? AIS_SelectionScheme_Add : AIS_SelectionScheme_Replace);
      m_context->UpdateCurrentViewer();
    }
  }
  m_currentAction = CurrentAction_Nothing;
}

Handle(V3d_View) viewerLines_scene::activeView() const
{
  Handle(V3d_View) view;
  if (m_context.IsNull())
    return view;

  const Handle(V3d_Viewer)& viewer = m_context->CurrentViewer();
  if (!viewer.IsNull())
  {
    if (!viewer->ActiveViews().IsEmpty())
    {
      view = viewer->ActiveViews().First();
    }
  }
  return view;
}

