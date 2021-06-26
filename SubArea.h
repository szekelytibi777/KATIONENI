#pragma once
#include <QRect>
#include <QList>

class QPoint;
class QSize;
class QPainter;
class Cetli;

class SubArea
{
public:
	SubArea(const SubArea& other);
	SubArea(const QRect &r);

	const QRect rect()const { return rect_; }
	void changeRect(const QRect newRect){rect_ = newRect;	}
	void changeSize(const QSize& newSize, const QPoint newPos = QPoint(-1, -1));
	QPoint absPos() { return rect_.topLeft() + actPos_; }
	QPoint& actPos() { return actPos_; }
	QPoint areaOffset() { return rect_.topLeft(); }
	void allocSize(const QSize& size);
	void reset() {	actPos_ = QPoint(0, 0); maxY = 0; }
	void rearrangeContent(QList<Cetli>& cetlies, bool byPosition = false);
	//void fitContent();
	QSize getContentsBoud(QList<Cetli>& cetlies);

private:
	QRect rect_;
	int gap;
	int maxY;
	QPoint actPos_;
	QSize actSize_;
	
};

