#include "viewerLines_actions.h"
#include "viewerLines_scene.h"
#include "viewerLines_linePresentation.h"

#include "AIS_DisplayMode.hxx"
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>

#include <QApplication>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

viewerLines_actions::viewerLines_actions(QObject* parent, viewerLines_scene* viewerMaker)
: QObject(parent), m_scene(viewerMaker), m_timerMaxIndex(INT_MAX), m_isRun(false)
{
  m_widget = new QWidget();

  QVBoxLayout* lay = new QVBoxLayout(m_widget);
  lay->setMargin(0);

  QPushButton* btn = new QPushButton("AIS_Trihedron", m_widget);
  btn->setCheckable(true);
  connect(btn, SIGNAL(clicked(bool)), this, SLOT(doTrihedron(bool)));
  lay->addWidget(btn);

  m_shapeBtn = new QPushButton("AIS_Shape", m_widget);
  m_shapeBtn->setCheckable(true);
  connect(m_shapeBtn, SIGNAL(clicked(bool)), this, SLOT(doShapePresentation(bool)));
  lay->addWidget(m_shapeBtn);

  m_customBtn = new QPushButton("viewerLines_linePresentation", m_widget);
  m_customBtn->setCheckable(true);
  connect(m_customBtn, SIGNAL(clicked(bool)), this, SLOT(doCustomPresentation(bool)));
  lay->addWidget(m_customBtn);

  lay->addStretch();

  QGroupBox* group = new QGroupBox("", m_widget);
  QGridLayout* grouplay = new QGridLayout(group);

  m_numberOfLines = new QSpinBox(m_widget);
  m_numberOfLines->setMaximum(INT_MAX);
  m_numberOfLines->setValue(10000);
  m_numberOfLines->setMinimumWidth(100);
  grouplay->addWidget(new QLabel("lines"), 0, 0);
  grouplay->addWidget(m_numberOfLines, 0, 1);
  connect(m_numberOfLines, SIGNAL(valueChanged(int)), this, SLOT(doNumberOfLinesChanged()));
  doNumberOfLinesChanged();

  lay->addWidget(group);

  m_startIndex = new QSpinBox(m_widget);
  m_startIndex->setMaximum(INT_MAX);
  m_startIndex->setValue(8000);
  connect(m_startIndex, SIGNAL(valueChanged(int)), this, SLOT(doStartIndexChanged()));
  grouplay->addWidget(new QLabel("lines now"), 1, 0);
  grouplay->addWidget(m_startIndex, 1, 1);

  m_rate = new QSpinBox(m_widget);
  m_rate->setValue(20);
  grouplay->addWidget(new QLabel("animation speed"), 2, 0);
  grouplay->addWidget(m_rate, 2, 1);
  grouplay->addWidget(new QLabel("msec"), 2, 2);

  m_animate = new QPushButton("run", m_widget);
  m_animate->setCheckable(true);
  connect(m_animate, SIGNAL(clicked(bool)), this, SLOT(doRunAnimation()));
  grouplay->addWidget(m_animate, 3, 0, 1, 3);

  lay->addWidget(group);

  lay->addStretch();

  btn = new QPushButton("fit all", m_widget);
  connect(btn, SIGNAL(clicked()), this, SLOT(doFitAll()));
  lay->addWidget(btn);

  btn = new QPushButton("top", m_widget);
  connect(btn, SIGNAL(clicked()), this, SLOT(doTop()));
  lay->addWidget(btn);

  m_timer = new QTimer();
  connect(m_timer, SIGNAL(timeout()), this, SLOT(doTimerTimeout()));
}

void viewerLines_actions::doTrihedron(bool checked)
{
  Handle(AIS_InteractiveObject) prs = viewerLines_presentations::presentation(m_scene->context(),
    STANDARD_TYPE(AIS_Trihedron)->Name());
  if (checked) {
    if (prs.IsNull()) {
      prs = viewerLines_presentations::createTrihedron();
      m_scene->displayPresentation(prs, AIS_WireFrame);
      m_scene->setDisplayMode(prs, AIS_WireFrame);
    }
    else {
      m_scene->redisplayPresentation(prs);
    }
  }
  else
    m_scene->erasePresentation(prs);
}

void viewerLines_actions::doShapePresentation(bool checked)
{
  Handle(AIS_InteractiveObject) prs = viewerLines_presentations::presentation(m_scene->context(),
    STANDARD_TYPE(AIS_Shape)->Name());
  if (checked) {
    if (prs.IsNull()) {
      prs = viewerLines_presentations::createShapePresentation();
      viewerLines_presentations::updateCustomPresentation(prs, m_points, m_startIndex->value());
      m_scene->displayPresentation(prs, AIS_WireFrame);
      m_scene->setDisplayMode(prs, AIS_Shaded);
    }
    else {
      m_scene->redisplayPresentation(prs);
    }
  }
  else {
    m_scene->erasePresentation(prs);
  }
}

void viewerLines_actions::doCustomPresentation(bool checked)
{
  Handle(AIS_InteractiveObject) prs = viewerLines_presentations::presentation(m_scene->context(),
    STANDARD_TYPE(viewerLines_linePresentation)->Name());
  if (checked) {
    if (prs.IsNull()) {
      prs = viewerLines_presentations::createCustomPresentation();
      viewerLines_presentations::updateCustomPresentation(prs, m_points, m_startIndex->value());
      m_scene->displayPresentation(prs, AIS_WireFrame);
      m_scene->setDisplayMode(prs, AIS_WireFrame);
    }
    else {
      m_scene->redisplayPresentation(prs);
    }
  }
  else {
    m_scene->erasePresentation(prs);
  }
}

void viewerLines_actions::doNumberOfLinesChanged()
{
  m_points = viewerLines_presentations::generatePointsOfLines(m_numberOfLines->value());
}

void viewerLines_actions::doStartIndexChanged()
{
  updatePresentations();
}

void viewerLines_actions::doRunAnimation()
{
  if (m_animate->isChecked())
  {
    m_timer->start(m_rate->value());
    m_isRun = true;
  }
  else {
    m_timer->stop();
    m_isRun = false;
  }
}

void viewerLines_actions::doFitAll()
{
  m_scene->fitAll();
}

void viewerLines_actions::doTop()
{
  m_scene->setTop();
}

void viewerLines_actions::doTimerTimeout()
{
  int value = m_startIndex->value();
  if (value > m_timerMaxIndex || value >= m_numberOfLines->value()) {
    m_timer->stop();
    m_animate->setChecked(false);
  }
  else {
    updatePresentations();

    m_timer->start(m_rate->value());
    m_startIndex->setValue(value + 1);
  }
}

void viewerLines_actions::updatePresentations()
{
  Handle(AIS_InteractiveObject) prs;
  if (m_shapeBtn->isChecked())
    prs = viewerLines_presentations::presentation(m_scene->context(), STANDARD_TYPE(AIS_Shape)->Name());
  else if (m_customBtn->isChecked())
    prs = viewerLines_presentations::presentation(m_scene->context(), STANDARD_TYPE(viewerLines_linePresentation)->Name());
  else
    return;
  viewerLines_presentations::updateCustomPresentation(prs, m_points, m_startIndex->value());
  m_scene->redisplayPresentation(prs);
}
