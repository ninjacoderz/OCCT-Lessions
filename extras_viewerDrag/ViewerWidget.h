//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#pragma once

#include <Aspect_VKeyFlags.hxx>

#include <QWidget>

class QMouseEvent;
class Viewer;
class ViewerInteractor;

//! Widget with OCCT viewer content inside.
class ViewerWidget : public QWidget
{
  Q_OBJECT
public:
  //! Constructor.
  ViewerWidget(QWidget* parent);

  //! Returns viewer.
  Viewer* viewer() const { return m_viewer; }

  //! Returns viewer interactor.
  ViewerInteractor* interactor() const { return m_interactor; }

  //! Qt paint engine. Empty to paint by OCCT viewer.
  virtual QPaintEngine* paintEngine() const override { return 0; }

protected:
  //! Inits and redraws view.
  //! @param event paint event
  virtual void paintEvent(QPaintEvent* event) override;

  //! Inits and resizes view.
  //! @param event resize event
  virtual void resizeEvent(QResizeEvent* event) override;

  //! Processes mouse event by viewer interactor.
  //! @param event the mouse event.
  virtual void mousePressEvent(QMouseEvent* event) override;

  //! Processes mouse event by viewer interactor.
  //! @param event the mouse event.
  virtual void mouseReleaseEvent(QMouseEvent* event) override;

  //! Processes mouse event by viewer interactor.
  //! @param event the mouse event.
  virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
  //! Converts Qt enum to OCCT enum value.
  //! @param modifierId key modifier.
  static Aspect_VKeyFlags keyFlag(const int modifierId);

  //! Converts Qt enum to OCCT enum value.
  //! @param buttonId key button.
  static Aspect_VKeyMouse keyMouse(const int buttonId);

private:
  Viewer*           m_viewer;     //!< current viewer.
  ViewerInteractor* m_interactor; //!< viewer interactor.
  bool              m_viewEmpty;  //!< flag whether the viewer is initialized or not.
};
