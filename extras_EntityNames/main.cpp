#include <Interface_EntityIterator.hxx>
#include <STEPControl_Reader.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <ShapeFix_Shape.hxx>
#include <XSControl_WorkSession.hxx>

int main(int argc, char** argv)
{
  const bool doHealing = true;

  STEPControl_Reader reader;

  if ( argc < 2 )
  {
    std::cout << "Error: insufficient number of arguments." << std::endl;
    return 1;
  }

  /* ================
   *  Read STEP file
   * ================ */

  if ( reader.ReadFile(argv[1]) != IFSelect_RetDone )
  {
    std::cout << "Cannot read file '" << argv[1] << "'." << std::endl;
    return false;
  }

  /* ======================
   *  Iterate Entity Model
   * ====================== */

  // Get STEP model.
  Handle(StepData_StepModel) stepModel = reader.StepModel();
  Interface_EntityIterator entIt = stepModel->Entities();

  std::cout << "Got "
            << entIt.NbEntities()
            << " entities from STEP file." << std::endl;

  // Prepare a map for counting entities.
  NCollection_DataMap<Handle(Standard_Type), int> entCountMap;

  // Iterate all entities.
  for ( ; entIt.More(); entIt.Next() )
  {
    const Handle(Standard_Transient)& ent     = entIt.Value();
    const Handle(Standard_Type)&      entType = ent->DynamicType();

    if ( ent->IsKind( STANDARD_TYPE(StepShape_ManifoldSolidBrep) ) )
    {
      Handle(StepShape_ManifoldSolidBrep)
        msb = Handle(StepShape_ManifoldSolidBrep)::DownCast(ent);

      std::cout << "Found Manifold Solid BREP with name '"
                << msb->Name()->ToCString()
                << "'" << std::endl;
    }

    // Count in a map.
    if ( entCountMap.IsBound(entType) )
      entCountMap(entType)++;
    else
      entCountMap.Bind(entType, 1);
  }

  /* ==========================
   *  Translate STEP into BREP
   * ========================== */

  // Transfer all roots into one shape or into several shapes.
  try
  {
    reader.TransferRoots();
  }
  catch ( Standard_Failure& )
  {
    std::cout << "Warning: exception occurred during translation." << std::endl;
  }
  if ( reader.NbShapes() <= 0 )
  {
    std::cout << "Error: transferring STEP to BREP failed." << std::endl;
    return false;
  }

  TopoDS_Shape preResult = reader.OneShape();

  // Release memory after translation.
  reader.WS()->NewModel();
  Standard::Purge();

  /* =================
   *  Post-processing
   * ================= */

  TopoDS_Shape result = preResult;

  // Apply shape healing.
  if ( doHealing )
  {
    Handle(ShapeFix_Shape) shapeHealer = new ShapeFix_Shape(result);
    bool fixRes = false;
    try
    {
      fixRes = shapeHealer->Perform();
    }
    catch ( Standard_Failure& )
    {
      fixRes = false;
    }
    if ( fixRes )
    {
      result = shapeHealer->Shape();
    }
  }

  return 0;
}
