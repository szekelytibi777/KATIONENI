#include "cetli.h"

Cetli::Cetli(const QImage &img,const QString &n, const QPoint &_pos)
    : pos(_pos)
    , QImage( img )
    , selected(false)
	, uID(idCount++)
	, name(n)
	, isAlive(true)
	, imgScaled(scaled(size()*0.75))
	//, groupKey(0)
{
	
	if(isNull())
		uID = -1;
}

QRect Cetli::scaledRect()
{
	return imgScaled.rect();

}

QSize Cetli::scaledSize()
{
	return imgScaled.size();
}

void Cetli::scale(float scale)
{

}


int Cetli::idCount = 0;
