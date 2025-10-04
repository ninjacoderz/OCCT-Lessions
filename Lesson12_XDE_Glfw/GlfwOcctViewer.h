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
 
namespace
{
  //! Convert GLFW mouse button into Aspect_VKeyMouse.
  static Aspect_VKeyMouse mouseButtonFromGlfw (int theButton)
  {
    switch (theButton)
    {
      case GLFW_MOUSE_BUTTON_LEFT:   return Aspect_VKeyMouse_LeftButton;
      case GLFW_MOUSE_BUTTON_RIGHT:  return Aspect_VKeyMouse_RightButton;
      case GLFW_MOUSE_BUTTON_MIDDLE: return Aspect_VKeyMouse_MiddleButton;
    }
    return Aspect_VKeyMouse_NONE;
  }
 
  //! Convert GLFW key modifiers into Aspect_VKeyFlags.
  static Aspect_VKeyFlags keyFlagsFromGlfw (int theFlags)
  {
    Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
    if ((theFlags & GLFW_MOD_SHIFT) != 0)
    {
      aFlags |= Aspect_VKeyFlags_SHIFT;
    }
    if ((theFlags & GLFW_MOD_CONTROL) != 0)
    {
      aFlags |= Aspect_VKeyFlags_CTRL;
    }
    if ((theFlags & GLFW_MOD_ALT) != 0)
    {
      aFlags |= Aspect_VKeyFlags_ALT;
    }
    if ((theFlags & GLFW_MOD_SUPER) != 0)
    {
      aFlags |= Aspect_VKeyFlags_META;
    }
    return aFlags;
  }
  static Aspect_VKey keyFromGlfw(int glfwKey)
  {
    switch (glfwKey)
    {
      case GLFW_KEY_F: return Aspect_VKey_F;
      case GLFW_KEY_S: return Aspect_VKey_S;
      case GLFW_KEY_W: return Aspect_VKey_W;
      case GLFW_KEY_T: return Aspect_VKey_T;
      case GLFW_KEY_B: return Aspect_VKey_B;
      case GLFW_KEY_L: return Aspect_VKey_L;
      case GLFW_KEY_R: return Aspect_VKey_R;
      case GLFW_KEY_BACKSPACE: return Aspect_VKey_Backspace;
      default: return Aspect_VKey_UNKNOWN;
    }
  }
}



 
//! Simple 3D viewer.
class GlfwOcctViewer
{
public:

  GlfwOcctViewer( int left, int top, int width, int height);
  //! Application event loop.
  void mainloop();
public:

  GlfwOcctViewer& operator<<(const TopoDS_Shape& shape)
  {
    this->AddShape(shape);
    return *this;
  }

  void AddShape(const TopoDS_Shape& shape);

  const Handle(AIS_InteractiveContext)& GetContext() const
  {
    return m_context;
  }
  //! Main application entry point.
  void run();

private:
  //! Create GLFW window.
  void initWindow(int theWidth, int theHeight, const char *theTitle);  

  //! Create 3D GlfwOcctViewer.
  void initViewer();

  void initEvents();

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
 
  //! Mouse click callback.
  static void onMouseButtonCallback (GLFWwindow* theWin, int theButton, int theAction, int theMods)
  {
    toView(theWin)->onMouseButton (theButton, theAction, theMods);
  }
 
  //! Mouse move callback.
  static void onMouseMoveCallback (GLFWwindow* theWin, double thePosX, double thePosY)
  {
    toView(theWin)->onMouseMove ((int )thePosX, (int )thePosY);
  }
  //! Window resize callback.
  static void onResizeCallback (GLFWwindow* theWin, int theWidth, int theHeight)
  { 
    toView(theWin)->onResize (theWidth, theHeight); 
  }

  static void onKeyCallback (GLFWwindow* theWin, int key, int scancode, int action, int mods)
  { 
    toView(theWin)->onKey ( key, scancode, action, mods);
  }
  
private:
  //! Mouse scroll event.
  void onMouseScroll (double theOffsetX, double theOffsetY);
 
  void onMouseButton(int theButton, int theAction, int theMods);
 
  void onMouseMove(int thePosX, int thePosY);

  void onResize(int theWidth, int theHeight);

  void onKey(int key, int scancode, int action, int mods);

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