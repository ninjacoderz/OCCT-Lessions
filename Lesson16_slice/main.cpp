//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Quaoar Studio
// All rights reserved.
//-----------------------------------------------------------------------------

// Local includes
#include "KHull2d.h"
#include "Viewer.h"

// OpenCascade includes
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepGProp.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <Poly_CoherentTriangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// Standard includes
#include <unordered_map>

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  /* =======================
   *  Get the working model.
   * ======================= */

  // Read from file.
  TopoDS_Shape shape;
  //
  if ( argc > 1 )
  {
    BRep_Builder bb;
    //
    if ( !BRepTools::Read(shape, argv[1], bb) )
    {
      std::cout << "Failed to read BREP shape from file '" << argv[1] << "'." << std::endl;
      return 1;
    }

    //vout << shape;
  }
  else
  {
    std::cout << "Please, pass filename (BREP) as an argument." << std::endl;
    return 1;
  }

  /* ==============================
   *  Prepare visualization meshes.
   * ============================== */

  // Prepare data structure for all triangles with back refs.
  Handle(Poly_CoherentTriangulation)
    tris = new Poly_CoherentTriangulation;

  BRepMesh_IncrementalMesh meshGen(shape, 1.0);

  // Add all triangulations from faces to the common collection.
  for ( TopExp_Explorer fexp(shape, TopAbs_FACE); fexp.More(); fexp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( fexp.Current() );

    // Poly_MeshPurpose was introduced in OpenCascade 7.6 and remained
    // undocumented.
    TopLoc_Location L;
    const Handle(Poly_Triangulation)&
      poly = BRep_Tool::Triangulation(face, L);

    // Add nodes.
    std::unordered_map<int, int> nodesMap;
    for ( int iNode = 1; iNode <= poly->NbNodes(); ++iNode )
    {
      // Make sure to apply location, e.g., see the effect in /cad/ANC101.brep
      const int n = tris->SetNode( poly->Node(iNode).Transformed(L).XYZ() );

      // Local to global node index mapping.
      nodesMap.insert({iNode, n});
    }

    // Add triangles.
    for ( int iTri = 1; iTri <= poly->NbTriangles(); ++iTri )
    {
      const Poly_Triangle& tri = poly->Triangle(iTri);

      int iNodes[3];
      tri.Get(iNodes[0], iNodes[1], iNodes[2]);

      tris->AddTriangle(nodesMap[iNodes[0]], nodesMap[iNodes[1]], nodesMap[iNodes[2]]);
    }
  }

  // Display the triangulation to be sure it's consistent.
  vout << tris->GetTriangulation();

  /* ===============================
   *  Compute the longest dimension.
   * =============================== */

  // Get the bounding box.
  Bnd_Box aabb;
  BRepBndLib::Add(shape, aabb, true); // Use triangulation.

  // Compute the dimensions.
  gp_XYZ Pmin = aabb.CornerMin().XYZ();
  gp_XYZ Pmax = aabb.CornerMax().XYZ();
  gp_XYZ D    = Pmax - Pmin;
  //
  double dims[3] = { Abs(D.X()), Abs(D.Y()), Abs(D.Z()) };

  // Construct the axis.
  gp_Ax1 axis;
  double tMin = 0.;
  double tMax = 0.;
  //
  if ( (dims[0] > dims[1]) && (dims[0] > dims[2]) ) // X
  {
    axis = gp_Ax1( Pmin, gp::DX() );
    tMin = 0;
    tMax = dims[0];
  }
  if ( (dims[1] > dims[0]) && (dims[1] > dims[2]) ) // Y
  {
    axis = gp_Ax1( Pmin, gp::DY() );
    tMin = 0;
    tMax = dims[1];
  }
  if ( (dims[2] > dims[0]) && (dims[2] > dims[1]) ) // Z
  {
    axis = gp_Ax1( Pmin, gp::DZ() );
    tMin = 0;
    tMax = dims[2];
  }
  //
  gp_Lin axisLin(axis);

  //vout << BRepBuilderAPI_MakeVertex(Pmin);
  //vout << BRepBuilderAPI_MakeVertex(Pmax);

  /* =================================
   *  Build a stack of slicing planes.
   * ================================= */

  const int    numPlanes = 10;
  const double step      = (tMax - tMin) / (numPlanes + 1);

  std::vector<gp_Pln> planes;
  //
  for ( int i = 0; i < numPlanes; ++i )
  {
    const double ti = tMin + step*(i + 1);
    const gp_Pnt Pi = ElCLib::Value(ti, axisLin);

    gp_Pln pln( Pi, axis.Direction() );
    //
    planes.push_back(pln);

    // Diagnostic dump.
    const double d = Abs(tMax - tMin)/2;
    vout << BRepBuilderAPI_MakeFace(pln, -d, d, -d, d);
  }

  /* ===================================
   *  Build mesh links and iterate them.
   * =================================== */

  tris->ComputeLinks();

  // Intersection points by edge IDs.
  //
  // plane 0: {P0, P1, P2 ...}
  // plane 1: {P3, P4, P5 ...}
  // ...
  std::vector<Handle(PointWithAttrCloud<gp_XY>)> slicePts;
  //
  for ( int i = 0; i < numPlanes; ++i )
  {
    slicePts.push_back( new PointWithAttrCloud<gp_XY> );
  }

  // Intersect each mesh link.
  for ( Poly_CoherentTriangulation::IteratorOfLink lit(tris); lit.More(); lit.Next() )
  {
    const Poly_CoherentLink& link = lit.Value();
    //
    const int n[2] = { link.Node(0), link.Node(1) };
    gp_XYZ    V[2] = { tris->Node(n[0]), tris->Node(n[1]) };

    //vout << BRepBuilderAPI_MakeVertex(V[0]) << BRepBuilderAPI_MakeVertex(V[1]);

    double Vt[2] = { (V[0] - Pmin)*axis.Direction().XYZ(),
                     (V[1] - Pmin)*axis.Direction().XYZ() };
    bool   rev   = false;
    //
    if ( Vt[1] < Vt[0] )
    {
      std::swap(Vt[0], Vt[1]);
      rev = true;
    }

    const int start = int( (Vt[0] - tMin)*numPlanes/(tMax - tMin) );
    const int end   = int( (Vt[1] - tMin)*numPlanes/(tMax - tMin) );

    for ( int i = start; i <= end; ++i )
    {
      // Position of the slicing plane along the axis.
      const double t = tMin + step*(i + 1);

      // The edge should have intersection point.
      if ( t < Vt[0] || t > Vt[1] )
        continue;

      //vout << BRepBuilderAPI_MakeVertex( ElCLib::Value(t, axisLin) );

      // Intersection point on the edge.
      const gp_XYZ edir = V[1] - V[0];
      const double tl   = rev ? Abs(t - Vt[1])/(Vt[1] - Vt[0]) : Abs(t - Vt[0])/(Vt[1] - Vt[0]); // Along link.
      const gp_XYZ p    = V[0] + tl*edir;
      double       pt   = (p - Pmin)*axis.Direction().XYZ();

      //vout << BRepBuilderAPI_MakeVertex(p);

      if ( (pt > Vt[1]) || (pt < Vt[0]) )
        continue; // Skip out of range intersection points.

      if ( std::isnan(pt) )
        continue;

      // Evaluate on plane.
      double u, v;
      ElSLib::Parameters(planes[i], p, u, v);

      vout << BRepBuilderAPI_MakeVertex(p);

      // Store the slice point.
      static int pidx = 0;
      slicePts[i]->AddElement( PointWithAttr<gp_XY>(gp_XY(u, v), 0, ++pidx) );
    }
  }

  /* =================
   *  Construct hulls.
   * ================= */

  for ( int i = 0; i < numPlanes; ++i )
  {
    //if ( i == 0 )
    //for ( int j = 0; j < slicePts[i]->GetNumberOfElements(); ++j )
    //{
    //  const gp_XY& uv = slicePts[i]->GetElement(j).Coord;

    //  vout << BRepBuilderAPI_MakeVertex( ElSLib::Value(uv.X(), uv.Y(), planes[i]) );
    //}

    // Prepare algorithm.
    KHull2d<gp_XY> kHull(slicePts[i], 5, 0);

    // Build K-neighbors hull.
    if ( !kHull.Perform() )
    {
      std::cout << "K-hull failed." << std::endl;
      continue;
    }

    // Override cloud with its hull.
    slicePts[i] = kHull.GetHull();

    // Build polygon for hull.
    BRepBuilderAPI_MakePolygon mkPolygon;
    //
    for ( int j = 0; j < slicePts[i]->GetNumberOfElements(); ++j )
    {
      const gp_XY& uv = slicePts[i]->GetElement(j).Coord;
      const gp_Pnt P  = ElSLib::Value(uv.X(), uv.Y(), planes[i]);

      mkPolygon.Add(P);

      //vout << BRepBuilderAPI_MakeVertex(P);
    }

    vout << mkPolygon.Wire();
  }

  vout.StartMessageLoop();
  return 0;
}
