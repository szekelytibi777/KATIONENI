#ifndef DSTWORKAREA_H
#define DSTWORKAREA_H

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QVector>
#include "cetli.h"
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QLabel>
#include "helper.h"

class DstWorkArea : public QScrollArea
{
    Q_OBJECT
public:
	DstWorkArea(QWidget * parent = 0);


protected:



private:


	QPoint moveStart;
	QScrollArea *scrollArea;
	Helper helper;
	int elapsed;


public slots:
	void animate();
};


#endif // DSTWORKAREA_H
