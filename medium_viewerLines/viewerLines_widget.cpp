#include "viewerLines_widget.h"
#include "viewerLines_scene.h"

#include "Graphic3d_Vec2.hxx"

#include <QMouseEvent>

viewerLines_widget::viewerLines_widget(QWidget* parent)
: QWidget(parent), m_firstPaint(true)
{
  m_scene = new viewerLines_scene();

  Handle(V3d_Viewer) v3dviewer = viewerLines_scene::createViewer();
  m_scene->setContext(viewerLines_scene::createContext(v3dviewer));

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  setMouseTracking(true);
}

void viewerLines_widget::paintEvent(QPaintEvent* /*theEvent*/)
{
  if (m_firstPaint)
  {
    viewerLines_scene::createView(m_scene->context(), (Aspect_Handle)winId());
    m_firstPaint = false;
  }
  m_scene->redrawView();
}

void viewerLines_widget::resizeEvent(QResizeEvent* /*theEvent*/)
{
  if (m_firstPaint)
  {
    m_scene->createView(m_scene->context(), (Aspect_Handle)winId());
    m_firstPaint = false;
  }
  m_scene->resizeView();
}

void viewerLines_widget::mousePressEvent(QMouseEvent* theEvent)
{
  m_scene->mousePressEvent(Graphic3d_Vec2i(theEvent->x(), theEvent->y()), keyMouse(theEvent->button()), keyFlag(theEvent->modifiers()));
}

void viewerLines_widget::mouseReleaseEvent(QMouseEvent* theEvent)
{
  m_scene->mouseReleaseEvent(Graphic3d_Vec2i(theEvent->x(), theEvent->y()), keyMouse(theEvent->button()), keyFlag(theEvent->modifiers()));
}

void viewerLines_widget::mouseMoveEvent(QMouseEvent* theEvent)
{
  m_scene->mouseMoveEvent(Graphic3d_Vec2i(theEvent->x(), theEvent->y()), keyMouse(theEvent->button()), keyFlag(theEvent->modifiers()));
}

Aspect_VKeyFlags viewerLines_widget::keyFlag(const int theModifierId)
{
  switch (theModifierId)
  {
  case Qt::NoModifier:      return Aspect_VKeyFlags_NONE;
  case Qt::ShiftModifier:   return Aspect_VKeyFlags_SHIFT;
  case Qt::ControlModifier: return Aspect_VKeyFlags_CTRL;
  default: break;
  }
  return Aspect_VKeyFlags_NONE;
}

Aspect_VKeyMouse viewerLines_widget::keyMouse(const int theButtonId)
{
  switch (theButtonId)
  {
  case Qt::NoButton:    return Aspect_VKeyMouse_NONE;
  case Qt::LeftButton:  return Aspect_VKeyMouse_LeftButton;
  case Qt::RightButton: return Aspect_VKeyMouse_RightButton;
  case Qt::MidButton:   return Aspect_VKeyMouse_MiddleButton;
  default: break;
  }
  return Aspect_VKeyMouse_NONE;
}
