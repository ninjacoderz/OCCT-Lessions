// Local includes
#include "Viewer.h"

#include <IGESControl_Reader.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Curve.hxx>

//-----------------------------------------------------------------------------

TopoDS_Shape BooleanGeneralFuse(const TopTools_ListOfShape& objects,
                                const double                fuzz,
                                BOPAlgo_Builder&            API)
{
  const bool bRunParallel = false;

  BOPAlgo_PaveFiller DSFiller;
  DSFiller.SetArguments(objects);
  DSFiller.SetRunParallel(bRunParallel);
  DSFiller.SetFuzzyValue(fuzz);
  DSFiller.Perform();
  bool hasErr = DSFiller.HasErrors();
  //
  if ( hasErr )
  {
    return TopoDS_Shape();
  }

  API.SetArguments(objects);
  API.SetRunParallel(bRunParallel);
  API.PerformWithFiller(DSFiller);
  hasErr = API.HasErrors();
  //
  if ( hasErr )
  {
    return TopoDS_Shape();
  }

  return API.Shape();
}

//-----------------------------------------------------------------------------

bool LoadIGES(const TCollection_AsciiString& filename,
              TopoDS_Shape&                  result)
{
  IGESControl_Reader reader;
  IFSelect_ReturnStatus outcome = reader.ReadFile( filename.ToCString() );
  //
  if ( outcome != IFSelect_RetDone )
    return false;

  reader.TransferRoots();

  result = reader.OneShape();
  return true;
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  TopoDS_Shape sh;
  if ( !LoadIGES("D:/Demos/lessons/Lesson21_split-curves/data/klp1.igs", sh) )
    return 1;

  TopTools_ListOfShape args;
  for ( TopExp_Explorer exp(sh, TopAbs_EDGE); exp.More(); exp.Next() )
    args.Append( exp.Current() );

  BOPAlgo_Builder bop;
  TopoDS_Shape res = BooleanGeneralFuse(args, 0.001, bop);
  //
  if ( res.IsNull() )
    return 1;

  vout << res;

  TopTools_IndexedMapOfShape allVertices;
  TopExp::MapShapes(res, TopAbs_VERTEX, allVertices);

  Handle(BRepTools_History) history = bop.History();

  BRep_Builder bbuilder;

  for ( TopTools_ListIteratorOfListOfShape argsIt(args); argsIt.More(); argsIt.Next() )
  {
    const TopoDS_Edge& currentEdge = TopoDS::Edge( argsIt.Value() );
    const TopTools_ListOfShape& images = history->Modified(currentEdge);

    TopoDS_Compound comp;
    bbuilder.MakeCompound(comp);
    //
    for ( TopTools_ListOfShape::Iterator imIt(images); imIt.More(); imIt.Next() )
    {
      const TopoDS_Edge& segment = TopoDS::Edge( imIt.Value() );

      TopoDS_Vertex vf = ShapeAnalysis_Edge().FirstVertex(segment);
      TopoDS_Vertex vl = ShapeAnalysis_Edge().LastVertex(segment);

      bbuilder.Add(comp, vf);
      bbuilder.Add(comp, vl);
    }

    vout << comp;

    double f, l;
    Handle(Geom_Curve) c3d = BRep_Tool::Curve(currentEdge, f, l);

    std::vector<double> params;

    ShapeAnalysis_Curve sac;
    //
    for ( TopoDS_Iterator compIt(comp); compIt.More(); compIt.Next() )
    {
      const TopoDS_Vertex& V = TopoDS::Vertex( compIt.Value() );
      gp_Pnt P = BRep_Tool::Pnt(V);

      gp_Pnt Pproj;
      double param;
      sac.Project(c3d, P, Precision::Confusion(), Pproj, param);

      params.push_back(param);
    }

    continue;
  }

  // D:/Demos/lessons/Lesson21_split-curves/data/klp1.igs

  vout.StartMessageLoop();
  return 0;
}
