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

#pragma once

//
#include "Viewer.h"

// Qt includes
#include <QWidget>

class ViewerWidget : public QWidget
{
  Q_OBJECT

public:

  ViewerWidget(QWidget* parent);

  virtual QPaintEngine* paintEngine() const override { return 0; }

protected:

  //! Avoids Qt standard execution of this method, redraw V3d view
  //! \param an event
  virtual void paintEvent(QPaintEvent* theEvent) override;

  //! Avoids Qt standard execution of this method, do mustBeResized for V3d view, Init view if it is the first call
  //! \param an event
  virtual void resizeEvent(QResizeEvent* theEvent) override;

  virtual void mouseReleaseEvent(QMouseEvent* theEvent) override;

  //virtual void mouseMoveEvent(QMouseEvent* theEvent) override;

  virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

private:

  Viewer* m_viewer;
  bool    m_isInitialized;
  bool    m_startDrawing;
  QPoint  m_startP;
};
