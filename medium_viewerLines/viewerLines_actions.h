#pragma once
#include "viewerLines_presentations.h"

#include <TopoDS_Shape.hxx>

#include <QObject>
#include <QWidget>

class QSpinBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QTimer;

class viewerLines_scene;

class viewerLines_actions : public QObject
{
  Q_OBJECT
public:
  viewerLines_actions(QObject* parent, viewerLines_scene* viewerMaker);

  QWidget* widgetControl() const { return m_widget; }

private slots:
  void doTrihedron(bool checked);
  void doShapePresentation(bool checked);
  void doCustomPresentation(bool checked);
  void doNumberOfLinesChanged();
  void doStartIndexChanged();
  void doRunAnimation();
  void doTimerTimeout();

  void doFitAll();
  void doTop();

protected:
  void updatePresentations();

private:
  QTimer* m_timer;
  Standard_Integer m_timerIndex, m_timerMaxIndex;
  Standard_Boolean m_isRun;

  viewerLines_scene* m_scene;
  QWidget* m_widget;
  QPushButton* m_animate;
  QSpinBox* m_numberOfLines;

  QPushButton* m_shapeBtn;
  QPushButton* m_customBtn;

  QSpinBox* m_startIndex;
  QSpinBox* m_rate;

  NCollection_List<gp_Pnt> m_points;
};
