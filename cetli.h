#ifndef CETLI_H
#define CETLI_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>

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
	Cetli* neigbour[4];
	Cetli* groupKey;
private :
	static int idCount;
};

#endif // CETLI_H
