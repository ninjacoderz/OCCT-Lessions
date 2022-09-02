//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#pragma once

#include <Aspect_Handle.hxx>
#include <Standard_Handle.hxx>

class AIS_InteractiveContext;
class AIS_InteractiveObject;
class V3d_View;

//! Creates main visu elements.
class Viewer
{
public:
  //! Constructor.
  Viewer();

  //! Creates context, viewer and view. View here is not fully filled.
  //! It requires posponed initView with window id.
  void init();

  //! Fill view with UI window identifier.
  //! @param windowHandle
  void initView(Aspect_Handle windowHandle);

  //! Returns current context.
  const Handle(AIS_InteractiveContext)& context() const;

  //! Redraw active view in viewer.
  void redrawView();

  //! Force the active view to react on window resize.
  void resizeView();

  //! Visualizes presentation in context.
  //! @param presentation  object to display
  //! @param displayMode   presentation display mode
  //! @param selectionMode selection mode, if -1 do not activate in selection
  void displayPresentation(const Handle(AIS_InteractiveObject)& presentation,
                           const int                            displayMode,
                           const int                            selectionMode = -1);

  //! Returns an active/defined view of the context. Gets the context viewer, find the first active view.
  //! If no active view, returns the first defined viewer.
  //! @param context visu context
  static Handle(V3d_View) activeView(const Handle(AIS_InteractiveContext)& context);

private:
  Handle(AIS_InteractiveContext) m_context; //!< visu context
};
