//-----------------------------------------------------------------------------
// Created on: 21 September 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Quaoar, https://analysissitus.org
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

// BVH
#include "BVHFacets.h"
#include "BVHIterator.h"

// Viewer
#include "Viewer.h"

// OpenCascade includes
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <STEPControl_Reader.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

// Standard includes
#include <iostream>
#include <map>

//! See discussion at
//!
//! https://analysissitus.org/forum/index.php?threads/this-is-a-forum-thread-about-building-and-using-bvh-bounding-volume-hierarchy-in-opencascade.250/
//!
//! Kudos to voyager (https://analysissitus.org/forum/index.php?members/voyager.1489/) for
//! providing the sample code.
int main(int argc, char** argv)
{
  if ( argc != 2 )
  {
    std::cerr << "Usage: model.step\n";
    return 1;
  }

  Viewer vout(50, 50, 500, 500);

  // Read STEP file.
  STEPControl_Reader reader;
  //
  if ( reader.ReadFile(argv[1]) != IFSelect_RetDone )
  {
    std::cerr << "STEP read failed\n";
    return 1;
  }
  //
  reader.TransferRoots();
  //
  TopoDS_Shape shape = reader.OneShape();

  vout << shape;

  // Generate facets.
  BRepMesh_IncrementalMesh(shape, 0.1);

  // Check summary.
  int totalTriangles = 0;
  int totalFaces = 0;
  int facesWithMesh = 0;
  //
  for ( TopExp_Explorer explorer(shape, TopAbs_FACE);
        explorer.More(); explorer.Next() )
  {
    totalFaces++;
    const TopoDS_Face& face = TopoDS::Face( explorer.Current() );

    TopLoc_Location loc;
    Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);
    //
    if ( !triangulation.IsNull() )
    {
      facesWithMesh++;
      totalTriangles += triangulation->NbTriangles();
    }
  }

  // Construct a BVH tree.
  Handle(BVHFacets) bvh = new BVHFacets(shape, BVHBuilder_Binned, &vout);
  //
  const opencascade::handle<BVH_Tree<double, 3>>& bvhTree = bvh->BVH();
  //
  if ( bvhTree.IsNull() )
  {
    std::cerr << "Failed to build BVH\n";
    return 1;
  }

  // Traverse the BVH tree.
  std::cout << "Traversing BVH tree...\n\n";
  std::map<int, std::string> nodeRelationship;
  nodeRelationship[0] = "Root";
  int totalBvhTriangles = 0;
  //
  for ( BVHIterator it(bvhTree); it.More(); it.Next() )
  {
    const BVH_Vec4i& nodeData = it.Current();
    int nodeId = it.CurrentIndex();

    if ( !it.IsLeaf() )
    {
      nodeRelationship[nodeData.y()] = "Left";
      nodeRelationship[nodeData.z()] = "Right";
    }

    std::cout << std::setw(6) << nodeId << " | "
              << std::setw(7) << nodeRelationship[nodeId] << " | ";

    if ( it.IsLeaf() )
    {
      int startIdx = nodeData.y();
      int endIdx = nodeData.z();
      int triangleCount = 1 + endIdx - startIdx;

      totalBvhTriangles += triangleCount;

      std::cout << "LEAF: Triangles [" << startIdx << " to " << (endIdx-1)
                << "] (" << triangleCount << " triangles)\n";
    }
    else
    {
      std::cout << "NODE: Left child: " << nodeData.y()
                << ", Right child: " << nodeData.z() << "\n";
    }
  }
  std::cout << "Mesh stats: " << totalTriangles << " triangles on "
            << facesWithMesh << "/" << totalFaces << " faces.\n"
            << "Total number of triangles in BVH: " << totalBvhTriangles << '\n';

  std::cout << "BVH size: " << bvh->Size() << std::endl;

  vout.StartMessageLoop();
}
