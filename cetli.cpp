#include "cetli.h"

Cetli::Cetli(const QImage &img,const QString &n, const QPoint &_pos)
    : pos(_pos)
    , QImage( img )
    , selected(false)
	, uID(idCount++)
	, name(n)
	, isAlive(true)
	, imgScaled(scaled(size()*0.66))
	, activeArea(0)
	//, groupKey(0)
{
	
	if(isNull())
		uID = -1;
}

QRect Cetli::scaledRect()
{
	QRect r(pos.x(), pos.y(), scaledSize().width(), scaledSize().height());
	return r;// imgScaled.rect();

}

QSize Cetli::scaledSize()
{
	return imgScaled.size();
}

void Cetli::handleDrop()
{
	if (activeArea) {
		QRect areaRect = activeArea->rect();
		//qDebug() << scaledRect() << activeArea->rect();
		if (!areaRect.contains(scaledRect()))
			activeArea = 0;
	}
}

void Cetli::scale(float scale)
{

}


int Cetli::idCount = 0;
