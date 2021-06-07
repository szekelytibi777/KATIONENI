#pragma once
#include <QRect>
class QPoint;
class QSize;
class QPainter;
class SubArea
{
public:
	SubArea(const SubArea& other);
	SubArea(const QRect &r);

	const QRect rect()const { return rect_; }
	QPoint& actPos() { return actPos_; }
	QPoint areaOffset() { return rect_.topLeft(); }
	void allocSize(const QSize& size);
	

private:
	const QRect rect_;
	int gap;
	int maxY;
	QPoint actPos_;
	QSize actSize_;
};

