#ifndef CETLI_H
#define CETLI_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>
#include "SubArea.h"
class Cetli : public QImage
{
public:
	bool isAlive;
	enum Sides{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	void kill(){
		isAlive = false;
	}

	Cetli() {uID =  -1;}
	Cetli(const QImage &img, const QString &n, const QPoint &_pos = QPoint(0,0));
    QPoint pos;
    QPoint dragOffset;
    bool selected;
	int uID;
	QString name;
	void scale(float scale);

	bool operator==(const Cetli& b){
		bool  r = this == &b;
		return r;
	}

	

	SubArea* getActiveArea() {
		return activeArea;
	}

	void setActiveArea(SubArea* area) {
		activeArea = area;
	}

	void handleDrop();


	QImage imgScaled;
	QRect scaledRect();
	QSize scaledSize();
private :
	SubArea* activeArea;
	static int idCount;
};

#endif // CETLI_H
