#pragma once
#include <QRect>
#include <QPoint>
#include <QPainter>
#include <QVector>
#include <QRect>
#include "SubArea.h"


class SubAreas
{
public:
	SubAreas();
	const QRect rect()const { return rect_; }
	QPoint& actPos() { return actPos_; }
	QPoint areaOffset() { return rect_.topLeft(); }
	void allocSize(const QSize& size);
	SubArea* area(const QRect& rect);
	SubArea* area(const QPoint& point);
	SubArea* area(const int index);
	void createAreas(const QSize& parentArea, bool area0 = false);
	void paintAreas(QPainter &painter);
	void deleteAreas();
	void resizeArea(QSize& newSize, int areaIdx = 0);
	void adjustX(int oldX, int newX);
	void adjustY(int oldY, int newY);
	void rearrangeContent();

private:
	QVector<SubArea*> areas;
	const QRect rect_;
	int gap;
	int maxY;
	QPoint actPos_;
	QSize actSize_;
};

