//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Quaoar Studio
// All rights reserved.
//-----------------------------------------------------------------------------

// Own include
#include "Viewer.h"

// OpenCascade includes
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <BRep_Builder.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

namespace {
  //! Adjust the style of local selection.
  //! \param[in] context the AIS context.
  void AdjustSelectionStyle(const Handle(AIS_InteractiveContext)& context)
  {
    // Initialize style for sub-shape selection.
    Handle(Prs3d_Drawer) selDrawer = new Prs3d_Drawer;
    //
    selDrawer->SetLink                ( context->DefaultDrawer() );
    selDrawer->SetFaceBoundaryDraw    ( true );
    selDrawer->SetDisplayMode         ( 1 ); // Shaded
    selDrawer->SetTransparency        ( 0.5f );
    selDrawer->SetZLayer              ( Graphic3d_ZLayerId_Topmost );
    selDrawer->SetColor               ( Quantity_NOC_GOLD );
    selDrawer->SetBasicFillAreaAspect ( new Graphic3d_AspectFillArea3d() );

    // Adjust fill area aspect.
    const Handle(Graphic3d_AspectFillArea3d)&
      fillArea = selDrawer->BasicFillAreaAspect();
    //
    fillArea->SetInteriorColor     (Quantity_NOC_GOLD);
    fillArea->SetBackInteriorColor (Quantity_NOC_GOLD);
    //
    fillArea->ChangeFrontMaterial() .SetMaterialName(Graphic3d_NOM_NEON_GNC);
    fillArea->ChangeFrontMaterial() .SetTransparency(0.4f);
    fillArea->ChangeBackMaterial()  .SetMaterialName(Graphic3d_NOM_NEON_GNC);
    fillArea->ChangeBackMaterial()  .SetTransparency(0.4f);

    selDrawer->UnFreeBoundaryAspect()->SetWidth(1.0);

    // Update AIS context.
    context->SetHighlightStyle(Prs3d_TypeOfHighlight_LocalSelected, selDrawer);
  }
}

//-----------------------------------------------------------------------------

//! Polished copy of XSDRAWSTLVRML_DataSource.
class TriangulationDataSource : public MeshVS_DataSource
{
  // RTTI
  DEFINE_STANDARD_RTTI_INLINE(TriangulationDataSource, MeshVS_DataSource)

public:

  //! Ctor.
  TriangulationDataSource(const Handle(Poly_Triangulation)& tris)
  {
    m_mesh = tris;

    if ( !m_mesh.IsNull() )
    {
      const int nbNodes = m_mesh->NbNodes();
      m_nodeCoords = new TColStd_HArray2OfReal(1, nbNodes, 1, 3);

      for ( int i = 1; i <= nbNodes; ++i )
      {
        m_nodes.Add(i);
        gp_Pnt xyz = m_mesh->Node(i);

        m_nodeCoords->SetValue( i, 1, xyz.X() );
        m_nodeCoords->SetValue( i, 2, xyz.Y() );
        m_nodeCoords->SetValue( i, 3, xyz.Z() );
      }

      const int nbTris = m_mesh->NbTriangles();
      //
      m_elemNormals = new TColStd_HArray2OfReal    (1, nbTris, 1, 3);
      m_elemNodes   = new TColStd_HArray2OfInteger (1, nbTris, 1, 3);

      for ( int i = 1; i <= nbTris; ++i )
      {
        m_elements.Add(i);

        const Poly_Triangle& tri = m_mesh->Triangle(i);

        int V[3];
        tri.Get(V[0], V[1], V[2]);

        const gp_Pnt P1 = m_mesh->Node(V[0]);
        const gp_Pnt P2 = m_mesh->Node(V[1]);
        const gp_Pnt P3 = m_mesh->Node(V[2]);

        gp_Vec V1(P1, P2);
        gp_Vec V2(P2, P3);

        gp_Vec N = V1.Crossed(V2);
        //
        if ( N.SquareMagnitude() > Precision::SquareConfusion() )
          N.Normalize();
        else
          N.SetCoord(0.0, 0.0, 0.0);

        for ( int j = 0; j < 3; ++j )
        {
          m_elemNodes->SetValue(i, j+1, V[j]);
        }

        m_elemNormals->SetValue( i, 1, N.X() );
        m_elemNormals->SetValue( i, 2, N.Y() );
        m_elemNormals->SetValue( i, 3, N.Z() );
      }
    }
  }

  //! Returns the geometric props of a node or an element.
  bool GetGeom(const int             ID,
               const bool            IsElement,
               TColStd_Array1OfReal& Coords,
               int&                  NbNodes,
               MeshVS_EntityType&    Type) const override
  {
    if ( m_mesh.IsNull() )
      return false;

    if ( IsElement ) // Element
    {
      if ( ID >= 1 && ID <= m_elements.Extent() )
      {
        Type = MeshVS_ET_Face;
        NbNodes = 3;

        for ( int i = 1, k = 1; i <= 3; i++ )
        {
          int IdxNode = m_elemNodes->Value(ID, i);
          for ( int j = 1; j <= 3; j++, k++ )
            Coords(k) = m_nodeCoords->Value(IdxNode, j);
        }

        return true;
      }

      return false;
    }
    else // Node
    {
      if ( ID >= 1 && ID <= m_nodes.Extent() )
      {
        Type    = MeshVS_ET_Node;
        NbNodes = 1;

        Coords( 1 ) = m_nodeCoords->Value(ID, 1);
        Coords( 2 ) = m_nodeCoords->Value(ID, 2);
        Coords( 3 ) = m_nodeCoords->Value(ID, 3);
        return true;
      }
    }

    return false;
  }

  //! This method is similar to `GetGeom()`, but returns only element or node type.
  //! This method is provided to achieve better performance.
  bool GetGeomType(const int          ID,
                   const bool         IsElement,
                   MeshVS_EntityType& Type) const override
  {
    if ( IsElement )
    {
      Type = MeshVS_ET_Face;
      return true;
    }
    else
    {
      Type = MeshVS_ET_Node;
      return true;
    }
  }

  //! Not used.
  void* GetAddr(const int ID, const bool IsElement) const override { return nullptr; }

  //! \return element's nodes.
  virtual bool GetNodesByElement(const int                ID,
                                 TColStd_Array1OfInteger& NodeIDs,
                                 int&                     NbNodes) const override
  {
    if ( m_mesh.IsNull() )
      return false;

    if ( ID >= 1 && ID <= m_elements.Extent() && NodeIDs.Length() >= 3 )
    {
      int low = NodeIDs.Lower();
      NodeIDs(low)     = m_elemNodes->Value(ID, 1);
      NodeIDs(low + 1) = m_elemNodes->Value(ID, 2);
      NodeIDs(low + 2) = m_elemNodes->Value(ID, 3);
      return true;
    }
    return false;
  }

  //! \return indices of nodes.
  const TColStd_PackedMapOfInteger& GetAllNodes() const override { return m_nodes; }

  //! \return indices of elements.
  const TColStd_PackedMapOfInteger& GetAllElements() const override { return m_elements; }

  //! This method calculates normal of face, which is using for correct reflection presentation.
  //! There is default method, for advance reflection this method can be redefined.
  virtual bool GetNormal(const int Id,
                         const int Max,
                         double&   nx,
                         double&   ny,
                         double&   nz) const override
  {
    if ( m_mesh.IsNull() )
      return false;

    if ( Id >= 1 && Id <= m_elements.Extent() && Max >= 3 )
    {
      nx = m_elemNormals->Value(Id, 1);
      ny = m_elemNormals->Value(Id, 2);
      nz = m_elemNormals->Value(Id, 3);
      return true;
    }

    return false;
  }

private:

  Handle(Poly_Triangulation)       m_mesh;
  TColStd_PackedMapOfInteger       m_nodes;
  TColStd_PackedMapOfInteger       m_elements;
  Handle(TColStd_HArray2OfInteger) m_elemNodes;
  Handle(TColStd_HArray2OfReal)    m_nodeCoords;
  Handle(TColStd_HArray2OfReal)    m_elemNormals;

};

//-----------------------------------------------------------------------------

Viewer::Viewer(const int left,
               const int top,
               const int width,
               const int height)
: m_hWnd  (NULL),
  m_bQuit (false)
{
  // Register the window class once
  static HINSTANCE APP_INSTANCE = NULL;
  if ( APP_INSTANCE == NULL )
  {
    APP_INSTANCE = GetModuleHandleW(NULL);
    m_hInstance = APP_INSTANCE;

    WNDCLASSW WC;
    WC.cbClsExtra    = 0;
    WC.cbWndExtra    = 0;
    WC.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    WC.hCursor       = LoadCursor(NULL, IDC_ARROW);
    WC.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    WC.hInstance     = APP_INSTANCE;
    WC.lpfnWndProc   = (WNDPROC) wndProcProxy;
    WC.lpszClassName = L"OpenGLClass";
    WC.lpszMenuName  = 0;
    WC.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    if ( !RegisterClassW(&WC) )
    {
      return;
    }
  }

  // Set coordinates for window's area rectangle.
  RECT Rect;
  SetRect(&Rect,
          left, top,
          left + width, top + height);

  // Adjust window rectangle.
  AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false);

  // Create window.
  m_hWnd = CreateWindow(L"OpenGLClass",
                        L"Quaoar >>> 3D",
                        WS_OVERLAPPEDWINDOW,
                        Rect.left, Rect.top, // Adjusted x, y positions
                        Rect.right - Rect.left, Rect.bottom - Rect.top, // Adjusted width and height
                        NULL, NULL,
                        m_hInstance,
                        this);

  // Check if window has been created successfully.
  if ( m_hWnd == NULL )
  {
    return;
  }

  // Show window finally.
  ShowWindow(m_hWnd, TRUE);

  HANDLE windowHandle = (HANDLE) m_hWnd;

  this->init(windowHandle);
}

//-----------------------------------------------------------------------------

void Viewer::AddShape(const TopoDS_Shape& shape)
{
  if ( shape.ShapeType() == TopAbs_VERTEX )
  {
    // Trick for vertices: just for better performance.
    if ( m_vertices.IsNull() )
      BRep_Builder().MakeCompound(m_vertices);

    BRep_Builder().Add(m_vertices, shape);
  }
  else
  {
    m_shapes.push_back(shape);
  }
}

//-----------------------------------------------------------------------------

void Viewer::AddMesh(const Handle(Poly_Triangulation)& mesh)
{
  m_meshes.push_back(mesh);
}

//-----------------------------------------------------------------------------

//! Starts message loop.
void Viewer::StartMessageLoop()
{
  // Add shapes.
  for ( auto sh : m_shapes )
  {
    Handle(AIS_Shape) shape = new AIS_Shape(sh);
    m_context->Display(shape, true);
    m_context->SetDisplayMode(shape, AIS_Shaded, true);

    // Adjust selection style.
    ::AdjustSelectionStyle(m_context);

    // Activate selection modes.
    m_context->Activate(4, true); // faces
    m_context->Activate(2, true); // edges
  }

  // Add vertices.
  {
    Handle(AIS_Shape) shape = new AIS_Shape(m_vertices);
    m_context->Display(shape, true);
  }

  // Add meshes.
  for ( auto mesh : m_meshes )
  {
    Handle(MeshVS_Mesh) meshVs = new MeshVS_Mesh;
    //
    meshVs->SetDataSource  ( new TriangulationDataSource(mesh) );
    meshVs->AddBuilder     ( new MeshVS_MeshPrsBuilder(meshVs), true );
    meshVs->SetDisplayMode ( MeshVS_DMF_Shading ); // Different from what we have for shapes!
    //
    Quantity_Color intColor     = Quantity_NOC_LIGHTSALMON3;
    Quantity_Color backIntColor = Quantity_NOC_CADETBLUE;
    Quantity_Color edgeColor    = Quantity_NOC_GREEN3;
    //
    meshVs->GetDrawer()->SetInteger (MeshVS_DA_MaxFaceNodes,      4);
    meshVs->GetDrawer()->SetColor   (MeshVS_DA_InteriorColor,     intColor);
    meshVs->GetDrawer()->SetColor   (MeshVS_DA_BackInteriorColor, backIntColor);
    meshVs->GetDrawer()->SetBoolean (MeshVS_DA_ShowEdges,         false);
    meshVs->GetDrawer()->SetColor   (MeshVS_DA_EdgeColor,         edgeColor);
    meshVs->GetDrawer()->SetDouble  (MeshVS_DA_EdgeWidth,         0.5);
    //
    m_context->Display(meshVs, true);
  }

  MSG Msg;
  while ( !m_bQuit )
  {
    switch ( ::MsgWaitForMultipleObjectsEx(0, NULL, 12, QS_ALLINPUT, 0) )
    {
      case WAIT_OBJECT_0:
      {
        while ( ::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) )
        {
          if ( Msg.message == WM_QUIT )
            m_bQuit = true;// return;

          ::TranslateMessage(&Msg);
          ::DispatchMessage(&Msg);
        }
      }
    }
  }
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

  // Event manager is constructed when both contex and view become available.
  m_evtMgr = new ViewerInteractor(m_view, m_context);

  // Aspect window creation
  m_wntWindow = new WNT_Window(winHandle);
  m_view->SetWindow(m_wntWindow, nullptr);
  //
  if ( !m_wntWindow->IsMapped() )
  {
    m_wntWindow->Map();
  }
  m_view->MustBeResized();

  // View settings.
  m_view->SetShadingModel(V3d_PHONG);

  // Configure rendering parameters
  Graphic3d_RenderingParams& RenderParams = m_view->ChangeRenderingParams();
  RenderParams.IsAntialiasingEnabled = true;
  RenderParams.NbMsaaSamples = 8; // Anti-aliasing by multi-sampling
  RenderParams.IsShadowEnabled = false;
  RenderParams.CollectedStats = Graphic3d_RenderingParams::PerfCounters_NONE;
}

//-----------------------------------------------------------------------------

LRESULT WINAPI Viewer::wndProcProxy(HWND   hwnd,
                                    UINT   message,
                                    WPARAM wparam,
                                    LPARAM lparam)
{
  if ( message == WM_CREATE )
  {
    // Save pointer to our class instance (sent on window create) to window storage.
    CREATESTRUCTW* pCreateStruct = (CREATESTRUCTW*) lparam;
    SetWindowLongPtr(hwnd, int (GWLP_USERDATA), (LONG_PTR) pCreateStruct->lpCreateParams);
  }

  // Get pointer to our class instance.
  Viewer* pThis = (Viewer*) GetWindowLongPtr( hwnd, int (GWLP_USERDATA) );
  return (pThis != NULL) ? pThis->wndProc(hwnd, message, wparam, lparam)
                         : DefWindowProcW(hwnd, message, wparam, lparam);
}

//-----------------------------------------------------------------------------

//! Window procedure.
LRESULT Viewer::wndProc(HWND   hwnd,
                        UINT   message,
                        WPARAM wparam,
                        LPARAM lparam)
{
  if ( m_view.IsNull() )
    return DefWindowProc(hwnd, message, wparam, lparam);

  switch ( message )
  {
    case WM_PAINT:
    {
      PAINTSTRUCT aPaint;
      BeginPaint(m_hWnd, &aPaint);
      EndPaint  (m_hWnd, &aPaint);
      m_evtMgr->ProcessExpose();
      break;
    }
    case WM_SIZE:
    {
      m_evtMgr->ProcessConfigure();
      break;
    }
    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZING:
    {
      switch ( m_view->RenderingParams().StereoMode )
      {
        case Graphic3d_StereoMode_RowInterlaced:
        case Graphic3d_StereoMode_ColumnInterlaced:
        case Graphic3d_StereoMode_ChessBoard:
        {
          // track window moves to reverse stereo pair
          m_view->MustBeResized();
          m_view->Update();
          break;
        }
        default:
          break;
      }
      break;
    }
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
      const Aspect_VKey vkey = WNT_Window::VirtualKeyFromNative( (int) wparam );
      //
      if ( vkey != Aspect_VKey_UNKNOWN )
      {
        const double timeStamp = m_evtMgr->EventTime();
        if ( message == WM_KEYDOWN )
        {
          m_evtMgr->KeyDown(vkey, timeStamp);
        }
        else
        {
          m_evtMgr->KeyUp(vkey, timeStamp);
        }
      }
      break;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      const Graphic3d_Vec2i pos( LOWORD(lparam), HIWORD(lparam) );
      const Aspect_VKeyFlags flags = WNT_Window::MouseKeyFlagsFromEvent(wparam);
      Aspect_VKeyMouse button = Aspect_VKeyMouse_NONE;
      //
      switch ( message )
      {
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
          button = Aspect_VKeyMouse_LeftButton;
          break;
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
          button = Aspect_VKeyMouse_MiddleButton;
          break;
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
          button = Aspect_VKeyMouse_RightButton;
          break;
      }
      if ( message == WM_LBUTTONDOWN
        || message == WM_MBUTTONDOWN
        || message == WM_RBUTTONDOWN )
      {
        SetFocus  (hwnd);
        SetCapture(hwnd);

        if ( !m_evtMgr.IsNull() )
          m_evtMgr->PressMouseButton(pos, button, flags, false);
      }
      else
      {
        ReleaseCapture();

        if ( !m_evtMgr.IsNull() )
          m_evtMgr->ReleaseMouseButton(pos, button, flags, false);
      }

      m_evtMgr->FlushViewEvents(m_context, m_view, true);
      break;
    }
    case WM_MOUSEWHEEL:
    {
      const int    delta  = GET_WHEEL_DELTA_WPARAM(wparam);
      const double deltaF = double(delta) / double(WHEEL_DELTA);
      //
      const Aspect_VKeyFlags flags = WNT_Window::MouseKeyFlagsFromEvent(wparam);
      //
      Graphic3d_Vec2i pos( int(short(LOWORD(lparam))), int(short(HIWORD(lparam))) );
      POINT cursorPnt = { pos.x(), pos.y() };
      if ( ScreenToClient(hwnd, &cursorPnt) )
      {
        pos.SetValues(cursorPnt.x, cursorPnt.y);
      }

      if ( !m_evtMgr.IsNull() )
      {
        m_evtMgr->UpdateMouseScroll( Aspect_ScrollDelta(pos, deltaF, flags) );
        m_evtMgr->FlushViewEvents(m_context, m_view, true);
      }
      break;
    }
    case WM_MOUSEMOVE:
    {
      Graphic3d_Vec2i pos( LOWORD(lparam), HIWORD(lparam) );
      Aspect_VKeyMouse buttons = WNT_Window::MouseButtonsFromEvent (wparam);
      Aspect_VKeyFlags flags   = WNT_Window::MouseKeyFlagsFromEvent(wparam);

      // don't make a slide-show from input events - fetch the actual mouse cursor position
      CURSORINFO cursor;
      cursor.cbSize = sizeof(cursor);
      if ( ::GetCursorInfo(&cursor) != FALSE )
      {
        POINT cursorPnt = { cursor.ptScreenPos.x, cursor.ptScreenPos.y };
        if ( ScreenToClient(hwnd, &cursorPnt) )
        {
          // as we override mouse position, we need overriding also mouse state
          pos.SetValues(cursorPnt.x, cursorPnt.y);
          buttons = WNT_Window::MouseButtonsAsync();
          flags   = WNT_Window::MouseKeyFlagsAsync();
        }
      }

      if ( m_wntWindow.IsNull() || (HWND) m_wntWindow->HWindow() != hwnd )
      {
        // mouse move events come also for inactive windows
        break;
      }

      if ( !m_evtMgr.IsNull() )
      {
        m_evtMgr->UpdateMousePosition(pos, buttons, flags, false);
        m_evtMgr->FlushViewEvents(m_context, m_view, true);
      }
      break;
    }
    default:
    {
      break;
    }

    case WM_DESTROY:
      m_bQuit = true;
  }
  return DefWindowProc(hwnd, message, wparam, lparam);
}
