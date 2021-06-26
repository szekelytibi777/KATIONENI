#include "SubArea.h"
#include "cetli.h"
#include <QDebug>

SubArea::SubArea(const QRect& r)
	: actPos_(0, 0)
	, actSize_(0, 0)
	, maxY(0)
	, rect_(r)
	, gap(3)
{
}

SubArea::SubArea(const SubArea& other)
	: actPos_(0, 0)
	, actSize_(0, 0)
	, maxY(0)
	, gap(3)
	//, rect_(other.rect())
{
}

void SubArea::allocSize(const QSize& size)
{
	actPos_.setX(actPos_.x() + actSize_.width());
	int newX = actPos_.x() + size.width() + gap;
	if (newX + size.width() > rect_.width()) {
		newX = 0;
		actPos_.setY(maxY + gap);
	}
	if ((actPos_.y() + size.height()) > maxY)
		maxY = actPos_.y() + size.height();
	actPos_.setX(newX);
}

QSize SubArea::getContentsBoud(QList<Cetli>& cetlies)
{
	QRect r;
	QSize s;
	for (Cetli& c : cetlies) {
		if (c.getActiveArea() == this) {
	//		qDebug() << r;
			r = r.united(c.scaledRect());
			
		}
	}
	s = r.size();
	changeSize(s);
	return s;
}

void SubArea::rearrangeContent(QList<Cetli>& cetlies)
{
	QRect r;
	QSize s;
	QList<Cetli*> tmp;
	for (Cetli& c : cetlies) {
		if (c.getActiveArea() == this) {
			tmp.push_back(&c);
		}
	}
	reset();
	for (Cetli* c : tmp) {
		c->pos = actPos() + areaOffset();
		allocSize(c->scaledSize());
	}
}

void SubArea::changeSize(const QSize& newSize, const QPoint newPos)
{
	rect_.setSize(newSize);
	if (newPos != QPoint(-1, -1)) {
		rect_.setX(newPos.x());
		rect_.setY( newPos.y());
	}
}




