//-----------------------------------------------------------------------------
// Created on: 15 October 2021
//-----------------------------------------------------------------------------
// Copyright (c) 2021, Sergey Slyadnev (sergey.slyadnev@gmail.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Sample includes
#include "ViewerWidget.h"
#include "Viewer.h"

// Qt includes
#include <QMouseEvent>

//-----------------------------------------------------------------------------

ViewerWidget::ViewerWidget(QWidget* parent)
: QWidget        (parent),
  m_isInitialized(false),
  m_startDrawing (false)
{
  // Qt documentation says "The contents of parent widgets are propagated by default
  // to each of their children as long as Qt::WA_PaintOnScreen is not set.
  // Custom widgets can be written to take advantage of this feature 
  // by updating irregular regions(to create non - rectangular child widgets),
  // or painting with colors that have less than full alpha component.
  setAttribute(Qt::WA_PaintOnScreen);

  // Qt documentation says "Indicates that the widget has no background, i.e. 
  // when the widget receives paint events, the background is not automatically repainted. 
  setAttribute(Qt::WA_NoSystemBackground);

  setMouseTracking(true);
}

//-----------------------------------------------------------------------------

void ViewerWidget::paintEvent(QPaintEvent* /*theEvent*/)
{
  if ( !m_isInitialized )
  {
    m_viewer = new Viewer((Aspect_Handle) winId());
    m_isInitialized = true;
  }
  m_viewer->redrawView();
}

//-----------------------------------------------------------------------------

void ViewerWidget::resizeEvent(QResizeEvent* /*theEvent*/)
{
  if ( !m_isInitialized )
  {
    m_viewer = new Viewer((Aspect_Handle) winId());
    m_isInitialized = true;
  }
  m_viewer->resizeView();
}

//-----------------------------------------------------------------------------

void ViewerWidget::mouseReleaseEvent(QMouseEvent* theEvent)
{
  QPoint newPoint = theEvent->pos();
  //
  m_viewer->drawPoint(newPoint);
  //
  if ( !m_startP.isNull())
    m_viewer->drawLine(m_startP, newPoint);

  m_startP = newPoint;
}

//-----------------------------------------------------------------------------

void ViewerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  m_startDrawing = false;
}
