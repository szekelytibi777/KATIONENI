#include "cetli.h"

Cetli::Cetli(const QImage &img,const QString &n, const QPoint &_pos)
    : pos(_pos)
    , QImage( img )
    , selected(false)
	, uID(idCount++)
	, name(n)
	, isAlive(true)
	//, groupKey(0)
{
	neigbour[LEFT] = 0;
	neigbour[RIGHT] = 0;
	neigbour[TOP] = 0;
	neigbour[BOTTOM] = 0;
	if(isNull())
		uID = -1;
}

void Cetli::scale(float scale)
{

}


int Cetli::idCount = 0;
