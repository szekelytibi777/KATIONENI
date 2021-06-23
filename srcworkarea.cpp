#include "srcworkarea.h"
#include "mainwindow.h"
#include "katitoneni.h"
#include <QDebug>
SrcWorkArea::SrcWorkArea(KatitoNeni *parent)
	: QLabel(reinterpret_cast<QWidget *>(parent))
	, katitoNeni(parent)

{
}

void SrcWorkArea::mousePressEvent(QMouseEvent * event)
{
	if(katitoNeni){
        int top, bottom, left, right;
		katitoNeni->findEdges(event->pos(), top, bottom ,left, right);
    }
}

void SrcWorkArea::mouseMoveEvent(QMouseEvent * event)
{
    /*
    if(main)
        main->setPixel(event->pos());
        */
}

