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
	SubArea* area(const QPoint& point);
	SubArea* area(const int index);
	void createAreas(const QSize& parentArea, int numHorisontal, int numVertical);
	void paintAreas(QPainter &painter);
	void deleteAreas();

private:
	QVector<SubArea*> areas;
	const QRect rect_;
	int gap;
	int maxY;
	QPoint actPos_;
	QSize actSize_;
};

