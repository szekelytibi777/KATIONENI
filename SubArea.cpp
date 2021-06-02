#include "SubArea.h"
#include <QDebug>
SubArea::SubArea(const QRect& r)
	: actPos_(0,0)
	, actSize_(0,0)
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
	int newX = actPos_.x() + size.width() +gap;
	if (newX + size.width() > rect_.width()) {
		newX = 0;
		actPos_.setY(maxY + gap);
	}
	if ((actPos_.y() + size.height()) > maxY)
		maxY = actPos_.y() + size.height();
	actPos_.setX(newX);
}

SubArea* SubArea::area(const QPoint& point)
{
	for (SubArea* sa :areas) {
		if (sa->rect().contains(point))
			return sa;
	}
	return 0;
}

void SubArea::createAreas(const QSize& parentArea, int numHorisontal, int numVertical)
{
	int stepX = parentArea.width() / numHorisontal;
	int stepY = parentArea.height() / numVertical;

	for (int x = 0; x < parentArea.width(); x += stepX) {
		for (int y = 0; y < parentArea.height(); y += stepY) {
			SubArea *a = new SubArea(QRect(x, y, stepX, stepY));
			areas.push_back(a);
		}
	}
}

void SubArea::deleteAreas()
{
	for (SubArea* a : areas) {
		assert(a);
		delete a;
	}
	areas.clear();
}


void SubArea::paintAreas(QPainter& painter)
{
	painter.setPen(QColor(Qt::GlobalColor::darkBlue));
	for (SubArea* a : areas) {
		assert(a);
		painter.drawRect(a->rect());
	}
}


QVector<SubArea* > SubArea::areas;
