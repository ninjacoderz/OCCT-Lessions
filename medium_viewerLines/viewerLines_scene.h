#pragma once

#include <Aspect_Handle.hxx>
#include <Aspect_VKeyFlags.hxx>
#include <Graphic3d_Vec2.hxx>
#include <Standard_Handle.hxx>

class AIS_InteractiveContext;
class AIS_InteractiveObject;
class V3d_Viewer;
class V3d_View;

class viewerLines_scene
{
  enum CurrentAction
  {
    CurrentAction_Nothing,
    CurrentAction_Zoom,
    CurrentAction_Pan,
    CurrentAction_Rotation
  };
public:
  viewerLines_scene();

  static Handle(AIS_InteractiveContext) createContext(Handle(V3d_Viewer) viewer);
  static Handle(V3d_Viewer) createViewer();
  static Handle(V3d_View) createView(const Handle(AIS_InteractiveContext)& context, Aspect_Handle windowHandle);

  void setContext(const Handle(AIS_InteractiveContext)& ctx);
  Handle(AIS_InteractiveContext) context() const;

  void redrawView();
  void resizeView();

  void fitAll();
  void setTop();

  void displayPresentation(const Handle(AIS_InteractiveObject)& presentation, const int mode);
  void redisplayPresentation(const Handle(AIS_InteractiveObject)& presentation);
  void erasePresentation(const Handle(AIS_InteractiveObject)& presentation);

  void setDisplayMode(const Handle(AIS_InteractiveObject)& presentation, int displayMode);

  void activatePresentation(const Handle(AIS_InteractiveObject)& presentation, int selectionMode);
  void deactivatePresentation(const Handle(AIS_InteractiveObject)& presentation);

  virtual void mousePressEvent(const Graphic3d_Vec2i& thePoint, Aspect_VKeyMouse keyMouse, Aspect_VKeyFlags keyFlag);
  virtual void mouseReleaseEvent(const Graphic3d_Vec2i& thePoint, Aspect_VKeyMouse keyMouse, Aspect_VKeyFlags keyFlag);
  virtual void mouseMoveEvent(const Graphic3d_Vec2i& thePoint, Aspect_VKeyMouse keyMouse, Aspect_VKeyFlags keyFlag);

private:
  Handle(V3d_View) activeView() const;

private:
  Handle(AIS_InteractiveContext) m_context;
  CurrentAction m_currentAction;
  Standard_Integer m_movePointX, m_movePointY;
  Standard_Integer m_pressPointX, m_pressPointY;
};
