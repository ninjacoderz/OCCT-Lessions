#include <Draw_Main.hxx>

#include <Geom_Circle.hxx>
#include <DrawTrSurf.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <DBRep.hxx>

#include <tcl.h>

int MyTestCommand(Draw_Interpretor& di,
                  int               argc,
                  const char**      argv)
{
  Handle(Geom_Circle) c = new Geom_Circle( gp_Ax2( gp::Origin(), gp::DZ() ), 10 );

  DrawTrSurf::Set("c", c);
  Standard_CString varName = "c";
  Handle(Geom_Circle) g = Handle(Geom_Circle)::DownCast( DrawTrSurf::Get(varName) );
  //
  std::cout << "Radius is " << g->Radius() << std::endl;

  TopoDS_Shape box = BRepPrimAPI_MakeBox(10, 10, 10);
  //
  DBRep::Set("b", box);

  DBRep::Get()

  return 0;
}

void Draw_InitAppli(Draw_Interpretor& di)
{
  Draw::Commands(di);

  di.Add("mytest", "myhelp string", MyTestCommand);
}

DRAW_MAIN
