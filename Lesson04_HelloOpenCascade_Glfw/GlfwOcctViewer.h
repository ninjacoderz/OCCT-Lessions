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

#pragma once

// Local includes
#include "ViewerInteractor.h"

// OpenCascade includes
#include <TopoDS_Shape.hxx>
#include <WNT_Window.hxx>

// Standard includes
#include <vector>
#include "GlfwOcctWindow.h"

class V3d_Viewer;
class V3d_View;
class AIS_InteractiveContext;
class AIS_ViewController;

//-----------------------------------------------------------------------------

//! Simple 3D viewer.
class GlfwOcctViewer
{
public:

  GlfwOcctViewer( int left, int top, int width, int height);

public:

  GlfwOcctViewer& operator<<(const TopoDS_Shape& shape)
  {
    this->AddShape(shape);
    return *this;
  }

  void AddShape(const TopoDS_Shape& shape);

  //! Main application entry point.
  void run();

private:
  //! Create GLFW window.
  void initWindow(int theWidth, int theHeight, const char *theTitle);  

  //! Create 3D GlfwOcctViewer.
  void initViewer();

  void initEvents();

  //! Application event loop.
  void mainloop();

  //! Draw Shapes
  void drawShapes();

private:
  std::vector<TopoDS_Shape> m_shapes; //!< Shapes to visualize.

private:
  //! Wrapper for glfwGetWindowUserPointer() returning this class instance.
  static GlfwOcctViewer* toView (GLFWwindow* theWin);
  //! Mouse scroll callback.
  static void onMouseScrollCallback (GLFWwindow* theWin, double theOffsetX, double theOffsetY)
  { 
    toView(theWin)->onMouseScroll (theOffsetX, theOffsetY); 
  }

private:
  //! Mouse scroll event.
  void onMouseScroll (double theOffsetX, double theOffsetY);

/* OpenCascade's things */
private:

  Handle(V3d_Viewer)             m_viewer;
  Handle(V3d_View)               m_view;
  Handle(AIS_InteractiveContext) m_context;
  
  Handle(ViewerInteractor)       m_evtMgr;
  Handle(GlfwOcctWindow) m_glfwOcctWindow;

/* Lower-level things */
private:
  bool      m_bQuit;     //!< Indicates whether user want to quit from window.
};

