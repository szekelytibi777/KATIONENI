//#include "ResultArea.h"
#include "ResultWorkArea.h"
#include "SubArea.h"
#include <QKeyEvent>
#include <QVariant>
#include <QDebug>
#include <QEvent>
#include <QColor>
#include "KatitoNeni.h"

ResultArea::ResultArea(ResultWorkArea* parent)
	: CetliDock(reinterpret_cast<DstWorkArea*>(parent))
	
{
	resize(1000, 1000);
	grabKeyboard();
	setMouseTracking(true);
	areas.createAreas(size(), 2, 4);
	
}


void ResultArea::mouseMoveEvent(QMouseEvent* event)
{
	repaint();
	
	CetliDock::mouseMoveEvent(event);
}

void ResultArea::mousePressEvent(QMouseEvent* event)
{
	mouseDrag = true;
	//QPoint mousePos = QPoint((unsigned)event->screenPos().x(), (unsigned)event->screenPos().y());

	CetliDock::mousePressEvent(event);
}

void ResultArea::mouseReleaseEvent(QMouseEvent* event)
{
	mouseDrag = false;
	repaint();
	CetliDock::mouseReleaseEvent(event);
}

void ResultArea::keyPressEvent(QKeyEvent* event)
{

	if (event->key() == Qt::Key_Shift) {
		shiftDown = true;
	}

	if (event->key() == Qt::Key_Delete && selected) {
		repaint();
	}

	if (event->key() == Qt::Key_Escape && selected) {
		//unhookSelected();
		if (selected) {
			selected->selected = false;
			selected = 0;
		}
		repaint();
	}

	CetliDock::keyPressEvent(event);
}


void ResultArea::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Shift) {
		shiftDown = false;
	}
	QWidget::keyReleaseEvent(event);
}


void ResultArea::addCetli(Cetli* cetli, QPoint pos)
{
	assert(cetli);
	Cetli c(*cetli);
	SubArea* area = areas.area(0);
	c.pos = area->actPos() + area->areaOffset();
	area->allocSize(c.size());
	cetlies.push_back(c);
	repaint();
}

void ResultArea::clear()
{
	cetlies.clear();
	repaint();
}


