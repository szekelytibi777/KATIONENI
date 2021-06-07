#include "SubArea.h"
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


