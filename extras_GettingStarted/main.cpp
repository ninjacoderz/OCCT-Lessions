// Local includes
#include "Viewer.h"

class MyClass : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(MyClass, Standard_Transient)

public:

  MyClass() : Standard_Transient()
  {
    std::cout << "ctor" << std::endl;
  }

  virtual ~MyClass()
  {
    std::cout << "dtor" << std::endl;
  }

  void DoSmth()
  {
    std::cout << "DoSmth()" << std::endl;
  }
};

void Foo(const Handle(MyClass)& arg)
{
  arg->DoSmth();
}

//-----------------------------------------------------------------------------

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopoDS_Iterator.hxx>

class MakePrism
{
public:

  MakePrism(const double L, const double W, const double H) : m_fL(L), m_fW(W), m_fH(H)
  {
    this->build();
  }

public:

  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  operator TopoDS_Shape()
  {
    return m_result;
  }

private:

  void build()
  {
    gp_Pnt2d P2d[4] = { gp_Pnt2d(0, 0), gp_Pnt2d(m_fL, 0), gp_Pnt2d(m_fL, m_fW), gp_Pnt2d(0, m_fW) };

    // P = S(u,v)
    Handle(Geom_Plane) plane = new Geom_Plane( gp_Pln( gp::XOY() ) );

    gp_Pnt P[4];
    int i = 0;
    //
    for ( auto uv : P2d )
      P[i++] = plane->Value( uv.X(), uv.Y() );

    std::vector<TopoDS_Edge> edges;
    for ( int e = 0; e < 4; ++e )
      edges.push_back( BRepBuilderAPI_MakeEdge(P[e], P[(e + 1 == 4) ? 0 : e + 1]) );

    BRepBuilderAPI_MakeWire mkWire;
    //
    for ( auto edge : edges )
      mkWire.Add(edge);

    const TopoDS_Wire& W = mkWire.Wire();

    TopoDS_Shape F = BRepBuilderAPI_MakeFace(W);

    BRepPrimAPI_MakePrism mkPrism( F, gp_Vec( plane->Axis().Direction() )*m_fH );
    TopoDS_Shape Prism = mkPrism.Shape();

    TopTools_IndexedMapOfShape vertices;
    TopExp::MapShapes(F, TopAbs_VERTEX, vertices);

    TopoDS_Compound edges2Blend; // TShape (null)
    BRep_Builder bbuilder;
    bbuilder.MakeCompound(edges2Blend); // TShape (not null)

    for ( int i = 1; i <= vertices.Extent(); ++i )
    {
      const TopoDS_Shape& V = vertices(i);

      // V -> Prism -> V1 -> BOP -> V2
      //        H1            H2
      // H = H1.Merge(H2)...
      //
      // H(V) -> V2

      const TopTools_ListOfShape& images = mkPrism.Generated(V);
      //
      for ( TopTools_ListOfShape::Iterator it(images); it.More(); it.Next() )
      {
        const TopoDS_Shape& image = it.Value();

        if ( image.ShapeType() == TopAbs_EDGE )
        {
          const TopoDS_Edge& edge = TopoDS::Edge(image);
          bbuilder.Add(edges2Blend, edge);
        }
      }
    }

    BRepFilletAPI_MakeFillet mkFillet(Prism);
    //
    for ( TopoDS_Iterator it(edges2Blend); it.More(); it.Next() )
    {
      const TopoDS_Edge& edge = TopoDS::Edge( it.Value() );

      mkFillet.Add(1., edge);
    }
    //
    mkFillet.Build();

    m_result = mkFillet.Shape();
  }

private:

  double       m_fL, m_fW, m_fH;
  TopoDS_Shape m_result;

};

#include <STEPControl_Writer.hxx>

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  Viewer vout(50, 50, 500, 500);

  /* ================================
   *  myObj is destroyed in DoSmth()
   * ================================ */

  Handle(MyClass) myObj = new MyClass();

  Foo(myObj);

  myObj->DoSmth();

  /* ===================
   *  Modeling exercise.
   * =================== */

  const double L = 20;
  const double W = 10;
  const double H = 5;
  //
  TopoDS_Shape res = MakePrism(L, W, H);

  STEPControl_Writer writer;
  writer.Transfer(res, STEPControl_AsIs);
  writer.Write("C:/users/serge/Desktop/test-exercise.stp");

  vout << res;

  vout.StartMessageLoop();
  return 0;
}
