//-----------------------------------------------------------------------------
// Created on: 23 January 2024
// Copyright (c) 2024-present, Quaoar Studio (ask@quaoar.pro)
//----------------------------------------------------------------------------

// Local includes
#include "Timer.h"
#include "Viewer.h"

// OpenCascade includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <OSD_Thread.hxx>
#include <Standard_Mutex.hxx>
#include <TopExp_Explorer.hxx>

#define THREAD_TIMEOUT_MS 500

//----------------------------------------------------------------------------

/*
   It should be noted that OpenCascade threads are nothing but wrappers
   over the operating system's threads. Therefore, you might consider
   using them just for simplicity and OS-independent abstraction.
 */

//----------------------------------------------------------------------------

static Standard_Mutex Mutex;

//----------------------------------------------------------------------------

void* Test_ThreadFunction(void* /*theData*/)
{
  // Enter the critical section (in Windows terminology) by using the scoped object
  // which acquires the lock on construction and releases it on destruction (RAII).
  Standard_Mutex::Sentry sentry(Mutex);

  std::cout << "Running in worker thread id: " << OSD_Thread::Current() << std::endl;

  return NULL;
}

//----------------------------------------------------------------------------

int TestThreads()
{
  const int MAX_THREAD = 5;
  OSD_Thread threads[MAX_THREAD];

  std::cout << "Running in master thread id: " << OSD_Thread::Current() << std::endl;

  // Prepare threads.
  OSD_ThreadFunction pThreadFunc = Test_ThreadFunction;
  int i;
  for ( i = 0; i < MAX_THREAD; ++i )
  {
    // Detaches the currently iterated thread and sets a function pointer
    // in the corresponding OSD_Thread data structure.
    //
    // In posix, the pthread_detach() function marks the target thread as detached.
    // When a detached thread terminates, its resources are automatically released
    // back to the system without the need for another thread to join with the
    // terminated thread. Attempting to detach an already detached thread results in
    // unspecified behavior.
    threads[i].SetFunction(pThreadFunc);
  }

  // Run threads.
  for ( i = 0; i < MAX_THREAD; ++i )
  {
    if ( !threads[i].Run(NULL) )
      std::cerr << "Error: cannot start thread " << i << std::endl;
  }

  for ( i = 0; i < MAX_THREAD; ++i )
  {
    Standard_Address res;

    // In posix, The pthread_join() function waits for the thread to terminate.
    // If that thread has already terminated, then pthread_join() returns
    // immediately.
    if ( !threads[i].Wait(res) )
      std::cerr << "Error: cannot get result of thread " << i << std::endl;
  }

  return 0;
}

//----------------------------------------------------------------------------

//! Settings to control which types of edges to output
struct t_hlrEdges
{
  bool OutputVisibleSharpEdges;
  bool OutputVisibleSmoothEdges;
  bool OutputVisibleOutlineEdges;
  bool OutputVisibleSewnEdges;
  bool OutputVisibleIsoLines;
  bool OutputHiddenSharpEdges;
  bool OutputHiddenSmoothEdges;
  bool OutputHiddenOutlineEdges;
  bool OutputHiddenSewnEdges;
  bool OutputHiddenIsoLines;

  t_hlrEdges()
  : OutputVisibleSharpEdges   (true),
    OutputVisibleSmoothEdges  (true),
    OutputVisibleOutlineEdges (true),
    OutputVisibleSewnEdges    (true),
    OutputVisibleIsoLines     (true),
    OutputHiddenSharpEdges    (false),
    OutputHiddenSmoothEdges   (false),
    OutputHiddenOutlineEdges  (false),
    OutputHiddenSewnEdges     (false),
    OutputHiddenIsoLines      (false)
  {}
};

//----------------------------------------------------------------------------

const TopoDS_Shape& Build3dCurves(const TopoDS_Shape& shape)
{
  for ( TopExp_Explorer it(shape, TopAbs_EDGE); it.More(); it.Next() )
    BRepLib::BuildCurve3d( TopoDS::Edge( it.Current() ) );

  return shape;
}

//----------------------------------------------------------------------------

TopoDS_Shape HLR(const TopoDS_Shape& shape,
                 const gp_Dir&       direction,
                 const t_hlrEdges    visibility)
{
  Handle(HLRBRep_Algo) brep_hlr = new HLRBRep_Algo;
  brep_hlr->Add(shape);

  gp_Ax2 transform(gp::Origin(), direction);
  HLRAlgo_Projector projector(transform);
  brep_hlr->Projector(projector);
  brep_hlr->Update();
  brep_hlr->Hide();

  // Extract the result sets.
  HLRBRep_HLRToShape shapes(brep_hlr);

  // V -- visible
  // H -- hidden
  TopoDS_Shape V  = Build3dCurves(shapes.VCompound       ()); // hard edge visibly
  TopoDS_Shape V1 = Build3dCurves(shapes.Rg1LineVCompound()); // smooth edges visibly
  TopoDS_Shape VN = Build3dCurves(shapes.RgNLineVCompound()); // contour edges visibly
  TopoDS_Shape VO = Build3dCurves(shapes.OutLineVCompound()); // contours apparents visibly
  TopoDS_Shape VI = Build3dCurves(shapes.IsoLineVCompound()); // isoparamtriques visibly
  TopoDS_Shape H  = Build3dCurves(shapes.HCompound       ()); // hard edge invisibly
  TopoDS_Shape H1 = Build3dCurves(shapes.Rg1LineHCompound()); // smooth edges invisibly
  TopoDS_Shape HN = Build3dCurves(shapes.RgNLineHCompound()); // contour edges invisibly
  TopoDS_Shape HO = Build3dCurves(shapes.OutLineHCompound()); // contours apparents invisibly
  TopoDS_Shape HI = Build3dCurves(shapes.IsoLineHCompound()); // isoparamtriques invisibly

  TopoDS_Compound C;
  BRep_Builder().MakeCompound(C);
  //
  if ( !V.IsNull() && visibility.OutputVisibleSharpEdges)
    BRep_Builder().Add(C, V);
  //
  if ( !V1.IsNull() && visibility.OutputVisibleSmoothEdges)
    BRep_Builder().Add(C, V1);
  //
  if ( !VN.IsNull() && visibility.OutputVisibleOutlineEdges)
    BRep_Builder().Add(C, VN);
  //
  if ( !VO.IsNull() && visibility.OutputVisibleSewnEdges)
    BRep_Builder().Add(C, VO);
  //
  if ( !VI.IsNull() && visibility.OutputVisibleIsoLines)
    BRep_Builder().Add(C, VI);
  //
  if ( !H.IsNull() && visibility.OutputHiddenSharpEdges)
    BRep_Builder().Add(C, H);
  //
  if ( !H1.IsNull() && visibility.OutputHiddenSmoothEdges)
    BRep_Builder().Add(C, H1);
  //
  if ( !HN.IsNull() && visibility.OutputHiddenOutlineEdges)
    BRep_Builder().Add(C, HN);
  //
  if ( !HO.IsNull() && visibility.OutputHiddenSewnEdges)
    BRep_Builder().Add(C, HO);
  
  if ( !HI.IsNull() && visibility.OutputHiddenIsoLines)
    BRep_Builder().Add(C, HI);

  gp_Trsf T;
  T.SetTransformation( gp_Ax3(transform) );
  T.Invert();

  return C.Moved(T);
}

//----------------------------------------------------------------------------

TopoDS_Shape DHLR(const TopoDS_Shape& shape,
                  const gp_Dir&       direction,
                  const t_hlrEdges    visibility)
{
  gp_Ax2 transform(gp::Origin(), direction);

  // Prepare projector.
  HLRAlgo_Projector projector(transform);

  // Prepare polygonal HLR algorithm which is known to be more reliable than
  // the "curved" version of HLR.
  Handle(HLRBRep_PolyAlgo) polyAlgo = new HLRBRep_PolyAlgo;
  //
  polyAlgo->Projector(projector);
  polyAlgo->Load(shape);
  polyAlgo->Update();

  // Create topological entities.
  HLRBRep_PolyHLRToShape HLRToShape;
  HLRToShape.Update(polyAlgo);

  // Prepare one compound shape to store HLR results.
  TopoDS_Compound C;
  BRep_Builder().MakeCompound(C);

  // Add visible edges.
  TopoDS_Shape vcompound = HLRToShape.VCompound();
  if ( !vcompound.IsNull() )
    BRep_Builder().Add(C, vcompound);
  //
  vcompound = HLRToShape.OutLineVCompound();
  if ( !vcompound.IsNull() )
    BRep_Builder().Add(C, vcompound);

  gp_Trsf T;
  T.SetTransformation( gp_Ax3(transform) );
  T.Invert();

  return C.Moved(T);
}

//----------------------------------------------------------------------------

struct t_threadData
{
  TopoDS_Shape input;
  gp_Dir       dir;
  TopoDS_Shape output;

  t_threadData() = default;

  t_threadData(const TopoDS_Shape& shape,
               const gp_Dir&       projDir)
  {
    this->input = BRepBuilderAPI_Copy(shape, true, true);
    this->dir   = projDir;
  }
};

//----------------------------------------------------------------------------

void* ThreadHLR(void* pData)
{
  std::cout << "Running HLR in worker thread id: " << OSD_Thread::Current() << std::endl;

  TIMER_NEW
  TIMER_GO

  t_threadData* pThreadData = reinterpret_cast<t_threadData*>(pData);

  t_hlrEdges style;

  pThreadData->output = HLR(pThreadData->input,
                            pThreadData->dir,
                            style);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("HLR finished")

  return NULL;
}

//----------------------------------------------------------------------------

void* ThreadDHLR(void* pData)
{
  std::cout << "Running DHLR in worker thread id: " << OSD_Thread::Current() << std::endl;

  TIMER_NEW
  TIMER_GO

  t_threadData* pThreadData = reinterpret_cast<t_threadData*>(pData);

  t_hlrEdges style;

  pThreadData->output = DHLR(pThreadData->input,
                             pThreadData->dir,
                             style);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("DHLR finished")

  return NULL;
}

//----------------------------------------------------------------------------

void ProjectParallel(const TopoDS_Shape& shape,
                     const gp_Dir&       dir,
                     TopoDS_Shape&       phlr,
                     TopoDS_Shape&       dhlr)
{
  std::cout << "Running in master thread id: " << OSD_Thread::Current() << std::endl;

  // Prepare threads.
  OSD_Thread threads[2];
  //
  threads[0].SetFunction(ThreadHLR);
  threads[1].SetFunction(ThreadDHLR);

  /*
   * Prepare data. Shape is passed as a shallow pointer to be deep-copied
   * in the ctor of `t_threadData`. The copy of the `shape` should stay alive
   * as long as the assigned thread is running.
   */
  t_threadData data[2];
  //
  data[0] = t_threadData( shape, dir );
  data[1] = t_threadData( shape, dir );

  // Run threads.
  for ( int i = 0; i < 2; ++i )
  {
    if ( !threads[i].Run(&data[i]) )
      std::cerr << "Error: cannot start thread " << i << std::endl;
  }

  for ( int i = 0; i < 2; ++i )
  {
    Standard_Address res;

    // In posix, The pthread_join() function waits for the thread to terminate.
    // If that thread has already terminated, then pthread_join() returns
    // immediately.
    if ( !threads[i].Wait(THREAD_TIMEOUT_MS, res) )
      std::cerr << "Error: cannot get result of thread " << threads[i].GetId() << std::endl;
  }

  phlr = data[0].output;
  dhlr = data[1].output;
}

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  Viewer vout(50, 50, 500, 500);

  if ( argc != 2 )
  {
    std::cout << "Error: input filename is not provided." << std::endl;
    return 1;
  }

  // Read geometry.
  BRep_Builder bbuilder;
  TopoDS_Shape shape;
  //
  if ( !BRepTools::Read(shape, argv[1], bbuilder) )
  {
    std::cout << "Error: cannot read shape from file." << std::endl;
    return 1;
  }
  //
  vout << shape;

  // DHLR requires mesh on the shape. We cannot wait for visualizer to do it
  // for us here as the visualization mesh is constructed on rendering.
  BRepMesh_IncrementalMesh meshGen(shape, 1.0);

  // Prepare HLR projections with time limit.
  TopoDS_Shape phlr, dhlr;
  ProjectParallel( shape, gp::DX(), phlr, dhlr );

  // Precise HLR.
  if ( !phlr.IsNull() )
    vout << phlr;

  // Discrete HLR.
  if ( !dhlr.IsNull() )
  {
    gp_Trsf T;
    T.SetTranslation( gp_Vec(-100, 0, 0) );
    //
    vout << dhlr.Moved(T);
  }

  vout.StartMessageLoop();

  return 0;
}
