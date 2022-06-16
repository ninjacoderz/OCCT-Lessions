//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Quaoar Studio
// All rights reserved.
//-----------------------------------------------------------------------------

// Local includes
#include "ClassifyPt.h"
#include "Viewer.h"

#define TIMER_NEW \
  OSD_Timer __aux_debug_Timer; \
  Standard_Real __aux_debug_Seconds, __aux_debug_CPUTime; \
  Standard_Integer __aux_debug_Minutes, __aux_debug_Hours;

#define TIMER_RESET \
  __aux_debug_Seconds = __aux_debug_CPUTime = 0.0; \
  __aux_debug_Minutes = __aux_debug_Hours = 0; \
  __aux_debug_Timer.Reset();

#define TIMER_GO \
  __aux_debug_Timer.Start();

#define TIMER_FINISH \
  __aux_debug_Timer.Stop(); \
  __aux_debug_Timer.Show(__aux_debug_Seconds, __aux_debug_Minutes, __aux_debug_Hours, __aux_debug_CPUTime);

#define TIMER_COUT_RESULT_MSG(Msg) \
  { \
    std::cout << "\n=============================================" << std::endl; \
    TCollection_AsciiString ascii_msg(Msg); \
    if ( !ascii_msg.IsEmpty() ) \
    { \
      std::cout << Msg                                             << std::endl; \
      std::cout << "---------------------------------------------" << std::endl; \
    } \
    std::cout << "Seconds:  " << __aux_debug_Seconds               << std::endl; \
    std::cout << "Minutes:  " << __aux_debug_Minutes               << std::endl; \
    std::cout << "Hours:    " << __aux_debug_Hours                 << std::endl; \
    std::cout << "CPU time: " << __aux_debug_CPUTime               << std::endl; \
    std::cout << "=============================================\n" << std::endl; \
  }

#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_CoherentTriangulation.hxx>

#include <unordered_map>

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  TopoDS_Shape shape;
  BRep_Builder bb;

  if ( argc > 1 )
  {
    if ( !BRepTools::Read(shape, argv[1], bb) )
    {
      std::cout << "Cannot read the input file" << std::endl;
      return 1;
    }
  }

  //vout << shape;

  Bnd_Box aabb;
  BRepBndLib::Add(shape, aabb, true);

  int density = 20;
  gp_XYZ Pmin = aabb.CornerMin().XYZ();
  gp_XYZ Pmax = aabb.CornerMax().XYZ();
  gp_XYZ D    = Pmax - Pmin;
  double dim[3] = {D.X(), D.Y(), D.Z()};
  double mind = Min(dim[0], Min(dim[1], dim[2]));
  const double d = mind/density;
  int nslice[3] = { int(Round(dim[0]/d)) + 1,
                    int(Round(dim[1]/d)) + 1,
                    int(Round(dim[2]/d)) + 1 };

  std::vector<gp_XYZ> gridPts;
  //
  for ( int i = 0; i < nslice[0]; ++i )
    for ( int j = 0; j < nslice[1]; ++j )
      for ( int k = 0; k < nslice[2]; ++k )
      {
        gp_XYZ P = Pmin
                 + gp_XYZ(d*i, 0,   0)
                 + gp_XYZ(0,   d*j, 0)
                 + gp_XYZ(0,   d*j, d*k);
        //
        gridPts.push_back(P);

        //vout << BRepBuilderAPI_MakeVertex(P);
      }

  // PMC by OpenCascade

  TIMER_NEW
  TIMER_GO

  BRepClass3d_SolidClassifier classOCC(shape);
  //
  for ( const auto& pt : gridPts )
  {
    classOCC.Perform(pt, Precision::Confusion());
    //
    if ( classOCC.State() == TopAbs_IN )
    {
      //vout << BRepBuilderAPI_MakeVertex(pt);
    }
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("PMC by OpenCascade")

  Handle(Poly_CoherentTriangulation)
    cohTris = new Poly_CoherentTriangulation;

  BRepMesh_IncrementalMesh meshGen(shape, 0.1);

  for ( TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( exp.Current() );

    TopLoc_Location loc;
    Handle(Poly_Triangulation) faceTris = BRep_Tool::Triangulation(face, loc);

    // Local node ID -> Global node ID
    std::unordered_map<int, int> nodesMap;
    for ( int iNode = 1; iNode <= faceTris->NbNodes(); ++iNode )
    {
      gp_Pnt P = faceTris->Node(iNode).Transformed(loc);
      const int cohNodeIndex = cohTris->SetNode( P.XYZ() );

      nodesMap.insert({iNode, cohNodeIndex});
    }

    for ( int iTri = 1; iTri <= faceTris->NbTriangles(); ++iTri )
    {
      const Poly_Triangle& tri = faceTris->Triangle(iTri);

      int iNodes[3];
      tri.Get(iNodes[0], iNodes[1], iNodes[2]);

      if ( face.Orientation() == TopAbs_REVERSED )
        std::swap(iNodes[1], iNodes[2]); // 1, 2, 3 -> 1, 3, 2

      int iCohNodes[3] = { nodesMap[iNodes[0]], nodesMap[iNodes[1]], nodesMap[iNodes[2]] };

      cohTris->AddTriangle(iCohNodes[0], iCohNodes[1], iCohNodes[2]);
    }
  }

  Handle(Poly_Triangulation) tris = cohTris->GetTriangulation();

  //vout << tris;

  TIMER_RESET
  TIMER_GO

  ClassifyPt classMesh(tris);
  //
  for ( const auto& pt : gridPts )
  {
    if ( classMesh.IsIn(pt, Precision::Confusion() ) )
    {
      vout << BRepBuilderAPI_MakeVertex(pt);
    }
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("PMC by meshes")


  vout.StartMessageLoop();
  return 0;
}
