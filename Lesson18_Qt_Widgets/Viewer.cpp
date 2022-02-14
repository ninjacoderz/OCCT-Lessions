//-----------------------------------------------------------------------------
// Created on: 15 October 2021
//-----------------------------------------------------------------------------
// Copyright (c) 2021, Sergey Slyadnev (sergey.slyadnev@gmail.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include "Viewer.h"

// OpenCascade includes
#include <AIS_InteractiveContext.hxx>
#include <AIS_Point.hxx>
#include <AIS_Line.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>

// Qt includes
#include <QPoint>

//-----------------------------------------------------------------------------

Viewer::Viewer(Aspect_Handle windowHandle)
{
  this->init(windowHandle);
}

//-----------------------------------------------------------------------------

void Viewer::init(const HANDLE& windowHandle)
{
  static Handle(Aspect_DisplayConnection) displayConnection;
  //
  if ( displayConnection.IsNull() )
    displayConnection = new Aspect_DisplayConnection();

  HWND winHandle = (HWND) windowHandle;
  //
  if ( winHandle == NULL )
    return;

  // Create OCCT viewer.
  Handle(OpenGl_GraphicDriver)
    graphicDriver = new OpenGl_GraphicDriver(displayConnection, false);

  m_viewer = new V3d_Viewer(graphicDriver);

  // Lightning.
  Handle(V3d_DirectionalLight) LightDir = new V3d_DirectionalLight(V3d_Zneg, Quantity_Color (Quantity_NOC_GRAY97), 1);
  Handle(V3d_AmbientLight)     LightAmb = new V3d_AmbientLight();
  //
  LightDir->SetDirection(1.0, -2.0, -10.0);
  //
  m_viewer->AddLight(LightDir);
  m_viewer->AddLight(LightAmb);
  m_viewer->SetLightOn(LightDir);
  m_viewer->SetLightOn(LightAmb);

  // AIS context.
  m_context = new AIS_InteractiveContext(m_viewer);

  // Configure some global props.
  const Handle(Prs3d_Drawer)& contextDrawer = m_context->DefaultDrawer();
  //
  if ( !contextDrawer.IsNull() )
  {
    const Handle(Prs3d_ShadingAspect)&        SA = contextDrawer->ShadingAspect();
    const Handle(Graphic3d_AspectFillArea3d)& FA = SA->Aspect();
    contextDrawer->SetFaceBoundaryDraw(true); // Draw edges.
    FA->SetEdgeOff();

    // Fix for inifinite lines has been reduced to 1000 from its default value 500000.
    contextDrawer->SetMaximalParameterValue(1000);
  }

  // Main view creation.
  m_view = m_viewer->CreateView();
  m_view->SetImmediateUpdate(false);
  m_view->SetBackgroundColor(Quantity_NOC_BLACK);

  // Aspect window creation
  Handle(Aspect_Window) wnd = new WNT_Window(windowHandle);
  m_view->SetWindow(wnd, nullptr);
  //
  if ( !wnd->IsMapped() )
  {
    wnd->Map();
  }
  m_view->MustBeResized();

  // View settings.
  m_view->SetShadingModel(V3d_PHONG);

  // Configure rendering parameters
  Graphic3d_RenderingParams& RenderParams = m_view->ChangeRenderingParams();
  RenderParams.IsAntialiasingEnabled      = true;
  RenderParams.NbMsaaSamples              = 8; // Anti-aliasing by multi-sampling
  RenderParams.IsShadowEnabled            = false;
  RenderParams.CollectedStats             = Graphic3d_RenderingParams::PerfCounters_NONE;
}

//-----------------------------------------------------------------------------

void Viewer::redrawView()
{
  m_view->Redraw();
}

//-----------------------------------------------------------------------------

void Viewer::resizeView()
{
  m_view->MustBeResized();
}

//-----------------------------------------------------------------------------

void Viewer::drawPoint(const QPoint& p)
{
  Standard_Real x, y, z;
  m_view->Convert(static_cast<int>( p.x() ), static_cast<int>( p.y() ),
                  x, y, z);

  Handle(Geom_Point) gP = new Geom_CartesianPoint(gp_Pnt(x, y, z));
  Handle(AIS_Point) pntPrs = new AIS_Point(gP);

  m_context->Display(pntPrs, true);
}

//-----------------------------------------------------------------------------

void Viewer::drawLine(const QPoint& p1, const QPoint& p2)
{
  Standard_Real x, y, z;
  m_view->Convert(static_cast<int>( p1.x() ), static_cast<int>( p1.y() ),
                  x, y, z);
  Handle(Geom_Point) gP1 = new Geom_CartesianPoint(gp_Pnt(x, y, z));

  m_view->Convert(static_cast<int>( p2.x() ), static_cast<int>( p2.y() ),
                  x, y, z);
  Handle(Geom_Point) gP2 = new Geom_CartesianPoint(gp_Pnt(x, y, z));
  //
  Handle(AIS_Line) linePrs = new AIS_Line(gP1, gP2);

  m_context->Display(linePrs, true);
}
