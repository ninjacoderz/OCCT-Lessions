//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#include "Viewer.h"

#include <AIS_InteractiveContext.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <WNT_Window.hxx>

#include <QMouseEvent>

//-----------------------------------------------------------------------------

Viewer::Viewer()
{
}

//-----------------------------------------------------------------------------

void Viewer::init()
{
  // create the graphical driver
  Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
  Handle(OpenGl_GraphicDriver) graphicDriver = new OpenGl_GraphicDriver(displayConnection);

  // create the main viewer
  Handle(V3d_Viewer) viewer = new V3d_Viewer(graphicDriver);
  viewer->SetDefaultLights();
  viewer->SetLightOn();

  // create context
  Handle(AIS_InteractiveContext) context = new AIS_InteractiveContext(viewer);
  context->HighlightStyle()->SetDisplayMode(AIS_Shaded);
  context->SelectionStyle()->SetDisplayMode(AIS_Shaded);

  // create view
  Handle(V3d_View) view = context->CurrentViewer()->CreateView();
  view->SetBackgroundColor(Quantity_Color(Quantity_NOC_BLACK));
  view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1);

  m_context = context;
}

//-----------------------------------------------------------------------------

void Viewer::initView(Aspect_Handle windowHandle)
{
  Handle(V3d_View) view = activeView(m_context);

  // create platform specific window
  Handle(Aspect_Window) wnd = new WNT_Window(windowHandle);
  view->SetWindow(wnd);

  // ensure that view content occupies whole window
  view->MustBeResized();
}

//-----------------------------------------------------------------------------

const Handle(AIS_InteractiveContext)& Viewer::context() const
{
  return m_context;
}

//-----------------------------------------------------------------------------

void Viewer::redrawView()
{
  Handle(V3d_View) view = activeView(m_context);
  if (view.IsNull())
    return;
  view->Redraw();
}

//-----------------------------------------------------------------------------

void Viewer::resizeView()
{
  Handle(V3d_View) view = activeView(m_context);
  if (view.IsNull())
    return;
  view->MustBeResized();
}

//-----------------------------------------------------------------------------

void Viewer::displayPresentation(const Handle(AIS_InteractiveObject)& presentation,
                                 const int                            displayMode,
                                 const int                            selectionMode)
{
  if (m_context.IsNull())
    return;

  m_context->Display(presentation, displayMode, selectionMode, Standard_True);
}

//-----------------------------------------------------------------------------

Handle(V3d_View) Viewer::activeView(const Handle(AIS_InteractiveContext)& context)
{
  Handle(V3d_View) view;
  if (context.IsNull())
    return view;

  const Handle(V3d_Viewer)& viewer = context->CurrentViewer();
  if (!viewer.IsNull())
  {
    if (!viewer->ActiveViews().IsEmpty())
    {
      view = viewer->ActiveViews().First();
    }
    else if (!viewer->DefinedViews().IsEmpty())
    {
      view = viewer->DefinedViews().First();
    }
  }
  return view;
}
