//-----------------------------------------------------------------------------
// Created on: 13 March 2022
// Copyright (c) 2022, Quaoar Studio (natalia@quaoar.pro)
//----------------------------------------------------------------------------

#include <QApplication>

#include "Viewer.h"
#include "ViewerInteractor.h"
#include "ViewerWidget.h"

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Manipulator.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom_Axis2Placement.hxx>
#include <OSD_Environment.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <QHBoxLayout>
#include <QMainWindow>

//-----------------------------------------------------------------------------

TopoDS_Shape createCylinder(const gp_Pnt&       location,
                            const gp_Dir&       direction,
                            const Standard_Real radius,
                            const Standard_Real height)
{
  return BRepPrimAPI_MakeCylinder(gp_Ax2(location, direction), radius, height);
}

//-----------------------------------------------------------------------------

void visualizePresentations(ViewerWidget* widget)
{
  Handle(AIS_InteractiveContext) context = widget->viewer()->context();

  // display near cylinder
  gp_Pnt location(100., 100., 0.);
  gp_Dir direction = gp::OZ().Direction();
  Standard_Real radius = 10.;
  Standard_Real height = 50.;
  TopoDS_Shape shapeTopo = createCylinder(location, direction, radius, height);
  Handle(AIS_Shape) shape = new AIS_Shape(shapeTopo);
  shape->SetMaterial(Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Pewter);
  context->Display(shape, AIS_Shaded, AIS_Shape::SelectionMode(TopAbs_SHAPE), Standard_True);

  // display manipulator
  Handle(AIS_Manipulator) manipulator = new AIS_Manipulator();
  manipulator->SetModeActivationOnDetection(true);
  manipulator->SetZoomPersistence(true);
  manipulator->Attach(shape);

  // display far cylinder
  location.Translate(gp_Vec(-60., 0., 0.));
  shapeTopo = createCylinder(location, direction, radius, height);
  shape = new AIS_Shape(shapeTopo);
  shape->SetMaterial(Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Pewter);
  context->Display(shape, AIS_Shaded, AIS_Shape::SelectionMode(TopAbs_SHAPE), Standard_True);
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

  ViewerWidget* widget = new ViewerWidget(centralWidget);
  lay->addWidget(widget);

  visualizePresentations(widget);

  mainWindow->resize(900, 600);
  mainWindow->show();

  widget->interactor()->fitAll();
  widget->interactor()->zoom(0.75);

  return app.exec();
}
