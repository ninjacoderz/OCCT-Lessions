#pragma once
#include <Aspect_VKeyFlags.hxx>

#include <QWidget>

class QMouseEvent;
class viewerLines_scene;

class viewerLines_widget : public QWidget
{
  Q_OBJECT
public:
  viewerLines_widget(QWidget* parent);

  viewerLines_scene* scene() const { return m_scene; }

  virtual QPaintEngine* paintEngine() const override { return 0; }

protected:
  virtual void paintEvent(QPaintEvent* theEvent) override;
  virtual void resizeEvent(QResizeEvent* theEvent) override;

  virtual void mousePressEvent(QMouseEvent* theEvent) override;
  virtual void mouseReleaseEvent(QMouseEvent* theEvent) override;
  virtual void mouseMoveEvent(QMouseEvent* theEvent) override;

private:
  static Aspect_VKeyFlags keyFlag(const int theModifierId);
  static Aspect_VKeyMouse keyMouse(const int theButtonId);

private:
  viewerLines_scene* m_scene;
  bool m_firstPaint;
};
