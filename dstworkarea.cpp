#include "dstworkarea.h"
#include "cetli.h"
#include <QPainter>
#include <QDebug>
#include"cetlidock.h"
#include <QPainter>
#include <algorithm>
#include <QDir>
#include <QScrollArea>
#include "helper.h"

DstWorkArea::DstWorkArea(QWidget * parent)
 : QScrollArea(parent)

// , scrollArea(new QScrollArea(this))
{

}


void DstWorkArea::animate()
{
	elapsed = 0;
	update();
}
