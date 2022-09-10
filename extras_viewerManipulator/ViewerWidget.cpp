//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#include "ViewerWidget.h"
#include "Viewer.h"
#include "ViewerInteractor.h"

#include <Graphic3d_Vec2.hxx>

#include <QMouseEvent>

//-----------------------------------------------------------------------------

ViewerWidget::ViewerWidget(QWidget* parent)
: QWidget(parent), m_viewEmpty(true)
{
  m_viewer = new Viewer();
  m_viewer->init();
  m_interactor = new ViewerInteractor(m_viewer->context());
  m_interactor->toApplyTransformation(true);

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  setMouseTracking(true);
}

//-----------------------------------------------------------------------------

void ViewerWidget::paintEvent(QPaintEvent* /*event*/)
{
  if (m_viewEmpty)
  {
    m_viewer->initView((Aspect_Handle)winId());
    m_viewEmpty = false;
  }
  m_viewer->redrawView();
}

//-----------------------------------------------------------------------------

void ViewerWidget::resizeEvent(QResizeEvent* /*event*/)
{
  if (m_viewEmpty)
  {
    m_viewer->initView((Aspect_Handle)winId());
    m_viewEmpty = false;
  }
  m_viewer->resizeView();
}

//-----------------------------------------------------------------------------

void ViewerWidget::mousePressEvent(QMouseEvent* event)
{
  m_interactor->mousePressEvent(Graphic3d_Vec2i(event->x(), event->y()),
                                keyMouse(event->button()),
                                keyFlag(event->modifiers()));
}

//-----------------------------------------------------------------------------

void ViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
  m_interactor->mouseReleaseEvent(Graphic3d_Vec2i(event->x(), event->y()),
                                  keyMouse(event->button()),
                                  keyFlag(event->modifiers()));
}

//-----------------------------------------------------------------------------

void ViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
  m_interactor->mouseMoveEvent(Graphic3d_Vec2i(event->x(), event->y()),
                               keyMouse(event->button()),
                               keyFlag(event->modifiers()));
}

//-----------------------------------------------------------------------------

Aspect_VKeyFlags ViewerWidget::keyFlag(const int modifierId)
{
  switch (modifierId)
  {
    case Qt::NoModifier:      return Aspect_VKeyFlags_NONE;
    case Qt::ShiftModifier:   return Aspect_VKeyFlags_SHIFT;
    case Qt::ControlModifier: return Aspect_VKeyFlags_CTRL;
    default: break;
  }
  return Aspect_VKeyFlags_NONE;
}

//-----------------------------------------------------------------------------

Aspect_VKeyMouse ViewerWidget::keyMouse(const int buttonId)
{
  switch (buttonId)
  {
    case Qt::NoButton:     return Aspect_VKeyMouse_NONE;
    case Qt::LeftButton:   return Aspect_VKeyMouse_LeftButton;
    case Qt::RightButton:  return Aspect_VKeyMouse_RightButton;
    case Qt::MiddleButton: return Aspect_VKeyMouse_MiddleButton;
    default: break;
  }
  return Aspect_VKeyMouse_NONE;
}
