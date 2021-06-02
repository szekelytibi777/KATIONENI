#pragma once
#include <QRect>
#include <QPoint>
#include <QPainter>
#include <QVector>
class SubArea
{
public:
	SubArea(const QRect& rect);
	SubArea(const SubArea& sa);
	const QRect rect()const { return rect_; }
	QPoint& actPos() { return actPos_; }
	QPoint areaOffset() { return rect_.topLeft(); }
	void allocSize(const QSize& size);
	static QVector<SubArea*> areas;
	static SubArea* area(const QPoint& point);
	static void createAreas(const QSize& parentArea, int numHorisontal, int numVertical);
	static void paintAreas(QPainter &painter);
	static void deleteAreas();

private:
	const QRect rect_;
	int gap;
	int maxY;
	QPoint actPos_;
	QSize actSize_;
};

