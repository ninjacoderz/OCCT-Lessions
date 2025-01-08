#include <Draw_Main.hxx>

#include <GCE2d_MakeCircle.hxx>
#include <DrawTrSurf.hxx>

int TestSketch(Draw_Interpretor& di,
               int               argc,
               const char**      argv)
{
  // 1-st circle
  const double c1x = Atof (argv[2]);
  const double c1y = Atof (argv[3]);
  const double r1  = Atof (argv[4]);

  // 2-nd circle
  const double c2x = Atof (argv[5]);
  const double c2y = Atof (argv[6]);
  const double r2  = Atof (argv[7]);

  Handle(Geom2d_Circle) C1 = GCE2d_MakeCircle( gp_Pnt2d(c1x, c1y), r1 );
  Handle(Geom2d_Circle) C2 = GCE2d_MakeCircle( gp_Pnt2d(c2x, c2y), r2 );

  DrawTrSurf::Set("C1", C1);
  DrawTrSurf::Set("C2", C2);

  di.Eval("av2d; 2dfit");

  return 0;
}

void Draw_InitAppli(Draw_Interpretor& di)
{
  Draw::Commands(di);

  di.Add("test-sketch", "test-sketch <name> <c1x> <c1y> <r1> <c2x> <c2y> <r2>", TestSketch);
}

DRAW_MAIN
