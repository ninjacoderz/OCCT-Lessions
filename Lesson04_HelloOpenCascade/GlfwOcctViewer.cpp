//-----------------------------------------------------------------------------
// Created on: 24 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev (sergey.slyadnev@gmail.com)
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
#include "GlfwOcctViewer.h"

// OpenCascade includes
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

//-----------------------------------------------------------------------------

GlfwOcctViewer::GlfwOcctViewer(const int left,
               const int top,
               const int width,
               const int height)
    : m_bQuit(false)
{
  
}

//-----------------------------------------------------------------------------

void GlfwOcctViewer::AddShape(const TopoDS_Shape &shape)
{
  m_shapes.push_back(shape);
}

void GlfwOcctViewer::initViewer()
{

  if (m_glfwOcctWindow.IsNull()
  || m_glfwOcctWindow->getGlfwWindow() == nullptr)
  {
    return;
  }

  // Create OCCT viewer.
  Handle(OpenGl_GraphicDriver)
      graphicDriver = new OpenGl_GraphicDriver(m_glfwOcctWindow->GetDisplay(), false);

  m_viewer = new V3d_Viewer(graphicDriver);

  // Lightning.
  Handle(V3d_DirectionalLight) LightDir = new V3d_DirectionalLight(V3d_Zneg, Quantity_Color(Quantity_NOC_GRAY97), 1);
  Handle(V3d_AmbientLight) LightAmb = new V3d_AmbientLight();
  //
  LightDir->SetDirection(1.0, -2.0, -10.0);
  //
  m_viewer->AddLight(LightDir);
  m_viewer->AddLight(LightAmb);
  m_viewer->SetLightOn(LightDir);
  m_viewer->SetLightOn(LightAmb);

  // AIS context.
  m_context = new AIS_InteractiveContext(m_viewer);


  // Main view creation.
  m_view = m_viewer->CreateView();
  m_view->SetImmediateUpdate(false);

  // Event manager is constructed when both contex and view become available.
  m_evtMgr = new ViewerInteractor(m_view, m_context);

  // Aspect window creation
  m_view->SetWindow(m_glfwOcctWindow, m_glfwOcctWindow->NativeGlContext());
  
  if (!m_glfwOcctWindow->IsMapped())
  {
    m_glfwOcctWindow->Map();
  }

  m_view->MustBeResized();

  // View settings.
  m_view->SetShadingModel(V3d_PHONG);

}

void GlfwOcctViewer::initEvents()
{
  // mouse callback
  glfwSetScrollCallback (m_glfwOcctWindow->getGlfwWindow(),GlfwOcctViewer::onMouseScrollCallback);
}

void GlfwOcctViewer::mainloop()
{
  while (!glfwWindowShouldClose (m_glfwOcctWindow->getGlfwWindow()))
  {
    // glfwPollEvents() for continuous rendering (immediate return if there are no new events)
    // and glfwWaitEvents() for rendering on demand (something actually happened in the viewer)
    //glfwPollEvents();
    glfwWaitEvents();
    if (!m_view.IsNull())
    {
      m_evtMgr->FlushViewEvents (m_context, m_view, true);
    }
  }
}

void GlfwOcctViewer::drawShapes()
{
  for ( auto sh : m_shapes )
  {
    Handle(AIS_Shape) shape = new AIS_Shape(sh);
    m_context->Display(shape, true);
    m_context->SetDisplayMode(shape, AIS_Shaded, true);
  }
}

GlfwOcctViewer *GlfwOcctViewer::toView(GLFWwindow *theWin)
{
  return static_cast<GlfwOcctViewer*>(glfwGetWindowUserPointer (theWin));
}

void GlfwOcctViewer::onMouseScroll (double theOffsetX, double theOffsetY)
{
  if (!m_view.IsNull())
  {
    const Graphic3d_Vec2i aPos = m_glfwOcctWindow->CursorPosition();
    m_evtMgr->UpdateMouseScroll(Aspect_ScrollDelta(aPos, int(theOffsetY * 8.0)));
  }
}

void GlfwOcctViewer::initWindow(int theWidth, int theHeight, const char *theTitle)
{

  glfwInit();
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
#if defined (__APPLE__)
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_glfwOcctWindow = new GlfwOcctWindow (theWidth, theHeight, theTitle);
  glfwSetWindowUserPointer       (m_glfwOcctWindow->getGlfwWindow(), this);
}


void GlfwOcctViewer::run()
{
  initWindow(800, 600, "The First OCCT Window");
  initViewer();
  initEvents();
  drawShapes();
  mainloop();
}