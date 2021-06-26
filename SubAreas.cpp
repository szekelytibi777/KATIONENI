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

SubArea* SubAreas::area(const QRect& rect)
{
	int i = 0;
	for (SubArea* sa : areas) {
		if (sa->rect().contains(rect)){
			qDebug() << "========\n" << i << "\n========";
			return sa;
		}
		i++;
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

	int w0 = 400;
	int w = parentArea.width();
	int h = parentArea.height();
	if (area0) {
		areas.push_back(new SubArea(QRect(0, 0, w0, parentArea.height())));
	}
	else {
		areas.push_back(new SubArea(QRect(0, 0, 0, 0)));
		w = 0;
	}

	int stepX = w / 3;
	int stepY = h / 3;
	for (int x = w0; x < w; x += stepX) {
		areas.push_back(new SubArea(QRect(x, 0, stepX, stepY)));
	}
	
	for (int y = stepY; y < parentArea.height(); y += stepY) {
		SubArea *a = new SubArea(QRect(w0, y, w, stepY));
		areas.push_back(a);
	}

	//qDebug() << areas.size();
}

void SubAreas::resizeArea(QSize& newSize, int areaIdx)
{
	SubArea* a0 = area(areaIdx);
	if (a0) {
		a0->changeSize(newSize);
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

SubArea* SubAreas::adjustX(int oldX, int newX)
{
	SubArea* ret = 0;
	for (int i = 0; i < areas.size();i++) {
		SubArea* a = areas[i];
		SubArea* nextArea = i + 1 < areas.size() ? areas[i + 1] : 0;
		QRect r = a->rect();
		if (abs(r.left() - oldX) < 5) {
			int x = r.left();
			int y = r.top();
			int w = r.width();
			int h = r.height();
			x = newX;
			a->changeRect(QRect(x, y, w, h));
			if (nextArea && nextArea->rect().top() == a->rect().top()) {
				int ox = newX - oldX;
				QRect r2 = nextArea->rect();
				nextArea->changeRect(QRect(r2.left()+ox, r2.top(), r2.width(), r2.height()));
				ret = nextArea;
			}
		}
	}
	return ret;
}

void SubAreas::adjustY(int oldY, int newY)
{
	for (SubArea* a : areas) {
		QRect r = a->rect();
		if (r.top() == oldY) {
			int x = r.left();
			int y = r.top();
			int w = r.width();
			int h = r.height();
			y = newY;
			h += oldY - newY;
			a->changeRect(QRect(x, y, w, h));
		}
	}
}

void SubAreas::rearrangeAreaContents(QList<Cetli>& cetlies)
{
	for (SubArea* a : areas) {
		a->rearrangeContent(cetlies);
	}
}




