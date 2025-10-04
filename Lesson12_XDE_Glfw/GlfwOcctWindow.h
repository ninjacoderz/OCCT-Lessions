#ifndef GlfwOcctWindow_Header
#define GlfwOcctWindow_Header

#include <Aspect_Window.hxx>
#include <Aspect_RenderingContext.hxx>
#include <GLFW/glfw3.h>

struct GLFWwindow;

class GlfwOcctWindow final : public Aspect_Window
{

    public:
        //! Main constructor.
        GlfwOcctWindow (int theWidth, int theHeight, const TCollection_AsciiString& theTitle);

        //! Close the window.
        ~GlfwOcctWindow() override { Close(); }
        
        //! Close the window.
        void Close();

        //! Return native OpenGL context.
        Aspect_RenderingContext NativeGlContext() const;
        
        //! Return X Display connection.
        const Handle(Aspect_DisplayConnection)& GetDisplay() const { return myDisplay; }

        //! Return cursor position.
        Graphic3d_Vec2i CursorPosition() const;

        /* 
         ** Implement the pure virtual functions **
        */

        //! Returns native Window handle
        Aspect_Drawable NativeHandle() const Standard_OVERRIDE;

         //! Returns parent of native Window handle.
        Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE { return 0; }
        
        //! Applies the resizing to the window <me>
        Aspect_TypeOfResize DoResize() Standard_OVERRIDE;

        //! Returns True if the window <me> is opened and False if the window is closed.
        Standard_Boolean IsMapped() const Standard_OVERRIDE;

        //! Apply the mapping change to the window <me> and returns TRUE if the window is mapped at screen.
        Standard_Boolean DoMapping() const Standard_OVERRIDE { return Standard_True; }

        //! Opens the window <me>.
        void Map() const Standard_OVERRIDE;

        //! Closes the window <me>.
        void Unmap() const Standard_OVERRIDE;

        void Position (Standard_Integer& theX1, Standard_Integer& theY1,
                         Standard_Integer& theX2, Standard_Integer& theY2) const Standard_OVERRIDE
        {
            theX1 = mXLeft;
            theX2 = mXRight;
            theY1 = mYTop;
            theY2 = mYBottom;
        }

        //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions.
        Standard_Real Ratio() const Standard_OVERRIDE
        {
            return static_cast<Standard_Real>(mXRight - mXLeft) / static_cast<Standard_Real>(mYBottom - mYTop);
        }

        //! Return window size.
        void Size (Standard_Integer& theWidth, Standard_Integer& theHeight) const Standard_OVERRIDE
        {
            theWidth  = mXRight - mXLeft;
            theHeight = mYBottom - mYTop;
        }

        //! Return GLFW window.
        GLFWwindow* getGlfwWindow() const { return mGlfwWindow; }
        
        Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return nullptr; }

    protected:
        Handle(Aspect_DisplayConnection) mDisplay;
        GLFWwindow* mGlfwWindow;
        Standard_Integer mXLeft;
        Standard_Integer mYTop;
        Standard_Integer mXRight;
        Standard_Integer mYBottom;
};

#endif