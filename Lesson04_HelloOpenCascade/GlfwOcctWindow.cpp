#include "GlfwOcctWindow.h"

#if defined (__APPLE__)
  #undef Handle // avoid name collisions in macOS headers
  #define GLFW_EXPOSE_NATIVE_COCOA
  #define GLFW_EXPOSE_NATIVE_NSGL
#elif defined (_WIN32)
  #define GLFW_EXPOSE_NATIVE_WIN32
  #define GLFW_EXPOSE_NATIVE_WGL
#else
  #define GLFW_EXPOSE_NATIVE_X11
  #define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

GlfwOcctWindow::GlfwOcctWindow(int theWidth, int theHeight, const TCollection_AsciiString &theTitle)
  :mGlfwWindow (glfwCreateWindow (theWidth, theHeight, theTitle.ToCString(), nullptr, nullptr)),
  mXLeft(0), mYTop(0), mXRight(0), mYBottom(0)
{
    if(mGlfwWindow != nullptr)
    {
        int aWidth = 0, aHeight = 0;
        glfwGetWindowPos( mGlfwWindow, & mXLeft, &mYTop);
        glfwGetWindowSize(mGlfwWindow, &aWidth, &aHeight);
        mXRight = mXLeft + aWidth;
        mYBottom = mYTop + aHeight;

    #if !defined(_WIN32) && !defined(__APPLE__)
        myDisplay = new Aspect_DisplayConnection ((Aspect_XDisplay* )glfwGetX11Display());
    #endif
    }
}

void GlfwOcctWindow::Close()
{
    if (mGlfwWindow != nullptr)
    {
        glfwDestroyWindow(mGlfwWindow);
        mGlfwWindow = nullptr;
    }
}

Aspect_Drawable GlfwOcctWindow::NativeHandle() const
{
#if defined (__APPLE__)
  return (Aspect_Drawable) glfwGetCocoaWindow (mGlfwWindow);
#elif defined (_WIN32)
  return (Aspect_Drawable)glfwGetWin32Window (mGlfwWindow);
#else
  return (Aspect_Drawable)glfwGetX11Window (mGlfwWindow);
#endif
}

Aspect_RenderingContext GlfwOcctWindow::NativeGlContext() const
{
#if defined (__APPLE__)
  return (NSOpenGLContext*)glfwGetNSGLContext (mGlfwWindow);
#elif defined (_WIN32)
  return glfwGetWGLContext (mGlfwWindow);
#else
  return glfwGetGLXContext (mGlfwWindow);
#endif
}

Graphic3d_Vec2i GlfwOcctWindow::CursorPosition() const
{
  Graphic3d_Vec2d aPos;
  glfwGetCursorPos (mGlfwWindow, &aPos.x(), &aPos.y());
  return Graphic3d_Vec2i ((int )aPos.x(), (int )aPos.y());
}

Aspect_TypeOfResize GlfwOcctWindow::DoResize()
{
  if (glfwGetWindowAttrib (mGlfwWindow, GLFW_VISIBLE) == 1)
  {
    int anXPos = 0, anYPos = 0, aWidth = 0, aHeight = 0;
    glfwGetWindowPos (mGlfwWindow, &anXPos, &anYPos);
    glfwGetWindowSize(mGlfwWindow, &aWidth, &aHeight);
    mXLeft   = anXPos;
    mXRight  = anXPos + aWidth;
    mYTop    = anYPos;
    mYBottom = anYPos + aHeight;
  }
  return Aspect_TOR_UNKNOWN;
}

void GlfwOcctWindow::Map() const
{
  glfwShowWindow (mGlfwWindow);
}

Standard_Boolean GlfwOcctWindow::IsMapped() const
{
  return glfwGetWindowAttrib (mGlfwWindow, GLFW_VISIBLE) != 0;
}

void GlfwOcctWindow::Unmap() const
{
  glfwHideWindow (mGlfwWindow);
}