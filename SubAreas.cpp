#include "SubArea.h"
#include "SubAreas.h"
#include <QDebug>

SubAreas::SubAreas()
	: actPos_(0,0)
	, actSize_(0,0)
	, maxY(0)
	, gap(3)
{
}



void SubAreas::allocSize(const QSize& size)
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

SubArea* SubAreas::area(const QPoint& point)
{
	for (SubArea* sa :areas) {
		if (sa->rect().contains(point))
			return sa;
	}
	return 0;
}

SubArea* SubAreas::area(const int index)
{
	assert(index < areas.size());
	return areas[index];
}
void SubAreas::createAreas(const QSize& parentArea, bool area0)
{	

	int w = parentArea.width() / 15;
	int h = parentArea.height() / 5;
	if (area0) {
		areas.push_back(new SubArea(QRect(0, 0, w, parentArea.height())));
	}
	else
		w = 0;
	int stepX = parentArea.width() / 6;
	int stepY = parentArea.height() / 3;
	areas.push_back(new SubArea(QRect(0, 0, w, parentArea.height())));
	for(int i = w;  i < parentArea.width()-w; i+=stepX){
		areas.push_back(new SubArea(QRect(i, 0, stepX, h)));
	}
	//int width = parentArea.width()/2;
	for (int y = h; y < parentArea.height(); y += stepY) {
		SubArea *a = new SubArea(QRect(w, y, parentArea.width()-w, stepY));
		areas.push_back(a);
	}
}

void SubAreas::deleteAreas()
{
	for (SubArea* a : areas) {
		assert(a);
		delete a;
	}
	areas.clear();
}


void SubAreas::paintAreas(QPainter& painter)
{
	painter.setPen(QColor(Qt::GlobalColor::darkBlue));
	for (SubArea* a : areas) {
		assert(a);
		painter.drawRect(a->rect());
	}
}



