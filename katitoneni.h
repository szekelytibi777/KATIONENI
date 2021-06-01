/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#define QT_NO_PRINTER

#include <QMainWindow>
#include <QImage>
#include <QSettings>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

#include "Log.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QSplitter;
class QListWidget;
class PagesList;
class QToolBar;
class QListWidgetItem;
class SrcWorkArea;
class DstWorkArea;
class CetliDock;
class QCheckBox;
class QSlider;

QT_END_NAMESPACE

//! [0]
class KatitoNeni : public QMainWindow
{
    Q_OBJECT

public:
	KatitoNeni();
	~KatitoNeni();
	void setPixel(const QPoint &p, bool update = false);
	void findEdges(const QPoint&p, int& top, int & bottom, int &left, int &right );
	int walkToEdge(const QPoint startPos, int xDir, int yDir);
	int radius();
	float scale;
	Log logToFile;
	static bool editEnabled;
	static QSize mainSize;
protected:
	virtual void resizeEvent(QResizeEvent* event);
public slots:
	void onItemDoubleClicked(QListWidgetItem *);
	void onCetliScaleChanged(int v);

private slots:
	void openDir();
	void fitToWindow();
	void shuffle();
	void alterSlicers();
	void saveCetlies();
private:
	QAction *createToolbarAction(const QString &iconFileName, const QString &name, const QString &toolTip,  const char*  method);
	void refreshSrcImage();
	void setSlicers();
    void createActions();
    void createMenus();
	QCheckBox *cb;
	QSlider *sliderCetliScale;

	void setImage(const QString &imagePath);
	bool loadCetlies();

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

	int slicerState;
	bool fixOrder();

	QImage srcImage;
	SrcWorkArea *srcWorkArea;
	DstWorkArea *dstWorkArea;
	CetliDock *cetliDock;
	QScrollArea * scrollArea[3];
	QSplitter *splitter;
	PagesList *pagesList;
    double scaleFactor;
	QToolBar *toolBar;
	QSettings settings;
	QString actImagePath;
	int colorDiffPower(const QRgb &a, const QRgb &b);
	int pow(int a){
		return a*a;
	}


#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

};

#endif
