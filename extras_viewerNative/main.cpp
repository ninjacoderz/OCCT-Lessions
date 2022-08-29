//-----------------------------------------------------------------------------
// Created on: 15 February 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#include <QApplication>

#include "Viewer.h"
#include "ViewerInteractor.h"
#include "ViewerWidget.h"

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepTools.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Ellipse.hxx>
#include <OSD_Environment.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <QHBoxLayout>
#include <QMainWindow>

//-----------------------------------------------------------------------------

TopoDS_Shape createDefaultShape()
{
  gp_Ax2 axes(gp::Origin(), gp::DX());
  Handle(Geom_Ellipse) ellCurve = new Geom_Ellipse(gp_Ax2(gp_Pnt(1200., 400., 350.),
    gp_Dir(0.17, 0.98, 0.0),
    gp_Dir(-0.93, 0.16, -0.32)),
    150., 100.);

  BRepPrimAPI_MakeRevol revol(BRepBuilderAPI_MakeEdge(ellCurve), axes.Axis());
  return revol.Shape();
}

//-----------------------------------------------------------------------------

void visualizePresentations(ViewerWidget*  widget,
                            const QString& fileName,
                            const int      selectionMode)
{
  Handle(AIS_InteractiveContext) context = widget->viewer()->context();

  // display trihedron
  Handle(Geom_Axis2Placement) placement = new Geom_Axis2Placement(gp_Pnt(), gp::DZ(), gp::DX());
  Handle(AIS_Trihedron) trihedron = new AIS_Trihedron(placement);
  context->Display(trihedron, AIS_WireFrame/*displayMode*/, -1, Standard_True);

  BRep_Builder bb;
  TopoDS_Shape shape;
  //
  if (fileName.isEmpty() || !BRepTools::Read(shape, fileName.toStdString().c_str(), bb))
  {
    shape = createDefaultShape();
  }
  // display shape
  TopoDS_Shape shapeTopo = createDefaultShape();
  Handle(AIS_Shape) shapePrs = new AIS_Shape(shape);
  context->Display(shapePrs, AIS_Shaded/*displayMode*/, selectionMode, Standard_True);
}

//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
  OSD_Environment environment ("QTDIR");
  TCollection_AsciiString qtDirValue = environment.Value();
  if (!qtDirValue.IsEmpty())
  {
    QString plugindsDirName = QString(qtDirValue.ToCString()) + "/plugins";
    QApplication::addLibraryPath (plugindsDirName);
  }
  QApplication app(argc, argv);

  QMainWindow* mainWindow = new QMainWindow();

  QWidget* centralWidget = new QWidget(mainWindow);
  mainWindow->setCentralWidget(centralWidget);
  QHBoxLayout* lay = new QHBoxLayout(centralWidget);
  lay->setMargin(6);

  ViewerWidget* viewerWidget = new ViewerWidget(centralWidget);
  lay->addWidget(viewerWidget);

  // selectionMode is integer from AIS_Shape::SelectionMode, e.g. '2' is for edge, '4' - face.
  int selectionMode = argc > 2 ? QString(argv[2]).toInt() : AIS_Shape::SelectionMode(TopAbs_SHAPE);
  visualizePresentations(viewerWidget, (argc > 1 ? argv[1] : QString()), selectionMode);

  mainWindow->resize(900, 600);
  mainWindow->show();

  viewerWidget->interactor()->fitAll();
  viewerWidget->interactor()->zoom(0.75);

  return app.exec();
}
