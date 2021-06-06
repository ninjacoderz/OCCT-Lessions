//-----------------------------------------------------------------------------
// Creation date: 26 February 2019
// Author:        Sergey Slyadnev
//-----------------------------------------------------------------------------
// Copyright (c) 2021, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

// OcafExLib includes
#include <OcafEx_Application.h>
#include <OcafEx_BuildOBB.h>
#include <OcafEx_ILimb.h>
#include <OcafEx_ISocket.h>

// OCCT includes
#include <OSD_Environment.hxx>
#include <RWStl.hxx>
#include <STEPControl_Writer.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>

// Active Data includes
#include <ActData_ParameterFactory.h>

// Standard includes
#include <iostream>

//-----------------------------------------------------------------------------

std::string GetExeFileName()
{
  char buffer[MAX_PATH];
  GetModuleFileName( NULL, buffer, MAX_PATH );
  return std::string(buffer);
}

std::string GetExePath()
{
  std::string f = GetExeFileName();
  return f.substr(0, f.find_last_of( "\\/" ));
}

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  OcafEx_NotUsed(argc);
  OcafEx_NotUsed(argv);

  // This variable should be initialized with a directory which contains
  // the application executable.
  TCollection_AsciiString appDir = GetExePath().c_str();
  TCollection_AsciiString appDirRes = appDir + "\\resources";

  // Set CSF_PluginDefaults, CSF_ResourcesDefaults variables.
  OSD_Environment( "CSF_PluginDefaults",    appDirRes.ToCString() ).Build();
  OSD_Environment( "CSF_ResourcesDefaults", appDirRes.ToCString() ).Build();

  // Create OCAF Application.
  Handle(OcafEx_Application) app = OcafEx_Application::Instance();

  /* =======================================================================
   *  Practice #2: create new document
   * ======================================================================= */

  // Create OCAF document.
  Handle(TDocStd_Document) doc;
  app->NewDocument(OcafExBinFormat, doc);
  //
  if ( doc.IsNull() )
  {
    std::cout << "Error: OCAF document is null after NewDocument()." << std::endl;
    return 1;
  }

  /* =======================================================================
   *  Practice #5: enable transactions
   * ======================================================================= */

  doc->SetModificationMode(true);

  // Set undo limit.
  doc->SetUndoLimit(100);

  /* =======================================================================
   *  Practice #3: populate document with temporary attribute
   *  Practice #5: wrap with transaction
   *  Practice #6: add yet another attribute and undo/redo this change
   * ======================================================================= */

  // 1-st transaction: set integer attribute.
  doc->OpenCommand();
  {
    TDataStd_Integer::Set(doc->Main(), 1000);
  }
  doc->CommitCommand();

  // 2-nd transaction: set real attribute.
  doc->OpenCommand();
  {
    TDataStd_Real::Set(doc->Main(), M_PI);
  }
  doc->CommitCommand();

  // Undo once.
  doc->Undo();

  // Redo once.
  doc->Redo();

  /* =======================================================================
   *  Practice #7:  add child labels
   *  Practice #10: use tag source for implicit auto-tagging
   * ======================================================================= */

  TDF_Label mainLab = doc->Main();

  doc->OpenCommand();
  {
    TDF_Label childLab1 = TDF_TagSource::NewChild(mainLab); // mainLab.FindChild(1);
    TDF_Label childLab2 = TDF_TagSource::NewChild(mainLab); // mainLab.FindChild(2);

    // Add attributes.
    TDataStd_Integer::Set(childLab1, 111);
    TDataStd_Integer::Set(childLab2, 222);

    // Add more children to have a deeper hierarchy.
    TDF_Label childLab11 = childLab1.FindChild(1);
    TDF_Label childLab12 = childLab1.FindChild(2);
    TDF_Label childLab21 = childLab2.FindChild(1);
    TDF_Label childLab22 = childLab2.FindChild(2);

    // Add attributes.
    TDataStd_Integer::Set(childLab11, 11);
    TDataStd_Integer::Set(childLab12, 12);
    TDataStd_Integer::Set(childLab21, 21);
    TDataStd_Integer::Set(childLab22, 22);
  }
  doc->CommitCommand();

  /* =======================================================================
   *  Practice #8: use child iterator
   * ======================================================================= */

  for ( TDF_ChildIterator cit(mainLab); cit.More(); cit.Next() )
  {
    TDF_Label currentLab = cit.Value();

    // Get entry.
    TCollection_AsciiString currentEntry;
    TDF_Tool::Entry(currentLab, currentEntry);

    // Print entry.
    std::cout << "Next label: " << currentEntry.ToCString() << std::endl;
  }

  /* =======================================================================
   *  Practice #9: find attribute
   * ======================================================================= */

  Handle(TDataStd_Real) attr;
  if ( !mainLab.FindAttribute(TDataStd_Real::GetID(), attr) )
  {
    std::cout << "Error: no TDataStd_Real attribute on main label." << std::endl;
    return 1;
  }

  /* =======================================================================
   *  Practice #11: forget attributes
   * ======================================================================= */

  TDF_Label targetLab;
  TDF_Tool::Label(doc->GetData(), "0:1:2:1", targetLab);
  //
  if ( targetLab.IsNull() )
  {
    std::cout << "Error: target label is null." << std::endl;
    return 1;
  }

  // Clear data.
  doc->OpenCommand();
  {
    targetLab.ForgetAllAttributes();
  }
  doc->CommitCommand();

  /* =======================================================================
   *  Practice #13: create limb object
   * ======================================================================= */

  TDF_Label limbLab = mainLab.Father().FindChild(2);

  // Create limb interface.
  Handle(OcafEx_ILimb) ILimb = new OcafEx_ILimb(limbLab);

  // Set data.
  doc->OpenCommand();
  {
    ILimb->SetName("Limb");
    ILimb->SetTransform(0., 0., 0., 0., 0., 0.);
  }
  doc->CommitCommand();

  /* =======================================================================
   *  Practice #14: create socket object
   * ======================================================================= */

  TDF_Label socketLab = mainLab.Father().FindChild(3);

  // Create socket interface.
  Handle(OcafEx_ISocket) ISocket = new OcafEx_ISocket(socketLab);

  // Set data.
  doc->OpenCommand();
  {
    ISocket->SetName("Socket");
    ISocket->SetLimb(ILimb);
  }
  doc->CommitCommand();

  /* =======================================================================
   *  Practice #15, #16, #19: load STL, build OBB and save OBB to STEP
   * ======================================================================= */

  Handle(Poly_Triangulation) limbTris;
  TopoDS_Shape               limbObbShape;
  OcafEx_OBB                 limbObb;

  // Load data.
  doc->OpenCommand();
  {
    // Load triangulation from STL.
    limbTris = RWStl::ReadFile("C:/Work/ottobock/ocafex/data/polynurbs 4 mm full lattice_wrapping_1.stl");
    //
    if ( limbTris.IsNull() )
    {
      std::cout << "Error: cannot load STL file." << std::endl;

      // Abort transaction.
      doc->AbortCommand();
      return 1;
    }

    // Store.
    ILimb->SetMesh(limbTris);

    // Build OBB.
    OcafEx_BuildOBB buildOBB(limbTris);
    //
    if ( !buildOBB.Perform() )
    {
      std::cout << "Error: cannot build OBB." << std::endl;

      // Abort transaction.
      doc->AbortCommand();
      return 1;
    }

    limbObb      = buildOBB.GetResult();
    limbObbShape = buildOBB.GetResultBox();

    // Store OBB.
    ILimb->SetOBB(limbObb);
    ILimb->GetOBB().Dump();
  }
  doc->CommitCommand();

  // Save STEP file.
  STEPControl_Writer writer;
  //
  if ( writer.Transfer(limbObbShape, STEPControl_AsIs) != IFSelect_RetDone )
  {
    std::cout << "Error: STEP transferring failed." << std::endl;
    return 1;
  }
  //
  if ( writer.Write("C:/users/ssv/desktop/obb.step") != IFSelect_RetDone )
  {
    std::cout << "Error: STEP writing failed." << std::endl;
    return 1;
  }

  /* =======================================================================
   *  Practice #20: Change OBB and undo, fix undo
   * ======================================================================= */

  limbObb.LocalCornerMin = limbObb.LocalCornerMin.XYZ() - gp_XYZ(10, 10, 10);
  limbObb.LocalCornerMax = limbObb.LocalCornerMax.XYZ() + gp_XYZ(10, 10, 10);

  // Change OBB.
  doc->OpenCommand();
  {
    ILimb->SetOBB(limbObb);
    ILimb->GetOBB().Dump();
  }
  doc->CommitCommand();

  // Undo and check OBB after undo.
  doc->Undo();
  ILimb->GetOBB().Dump();

  /* =======================================================================
   *  Save the document
   * ======================================================================= */

  PCDM_StoreStatus writerStatus = app->SaveAs(doc, "C:/users/ssv/desktop/test-msvc2017.ottobock");

  // Check status.
  if ( writerStatus != PCDM_SS_OK )
  {
    TCollection_AsciiString statusStr;

         if ( writerStatus == PCDM_SS_DriverFailure )      statusStr = "PCDM_SS_DriverFailure";
    else if ( writerStatus == PCDM_SS_WriteFailure )       statusStr = "PCDM_SS_WriteFailure";
    else if ( writerStatus == PCDM_SS_Failure )            statusStr = "PCDM_SS_Failure";
    else if ( writerStatus == PCDM_SS_Doc_IsNull )         statusStr = "PCDM_SS_Doc_IsNull";
    else if ( writerStatus == PCDM_SS_No_Obj )             statusStr = "PCDM_SS_No_Obj";
    else if ( writerStatus == PCDM_SS_Info_Section_Error ) statusStr = "PCDM_SS_Info_Section_Error";

    std::cout << "Status of writer is not OK. Error code: " << statusStr << std::endl;

    return 1;
  }

  /* =======================================================================
   *  Last step: close the document
   * ======================================================================= */

  app->Close(doc);

  std::cout << "OCAF sample is done." << std::endl;
  return 0;
}
