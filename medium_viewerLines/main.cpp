#include <QApplication>

#include "viewerLines_widget.h"
#include "viewerLines_actions.h"

#include <QHBoxLayout>
#include <QMainWindow>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QMainWindow* mainWindow = new QMainWindow();

  QWidget* centralWidget = new QWidget(mainWindow);
  mainWindow->setCentralWidget(centralWidget);
  QHBoxLayout* lay = new QHBoxLayout(centralWidget);
  lay->setMargin(6);

  viewerLines_widget* viewer = new viewerLines_widget(centralWidget);
  lay->addWidget(viewer);

  viewerLines_actions* actions = new viewerLines_actions(centralWidget, viewer->scene());
  lay->addWidget(actions->widgetControl());
  lay->setStretchFactor(viewer, 1);

  mainWindow->resize(1200, 725);
  mainWindow->show();

  return app.exec();
}
