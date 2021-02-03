#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <QList>
#include "cetli.h"


class CetliDock;
class Group : public QList<Cetli *>
{

public:
	Group(CetliDock *dock = 0);
	void add(Cetli *c);
	Cetli *getMainCetli() const;
	QString toString() const;
	QString mainCetliName() const;
private:
	mutable Cetli *mainCetli;
	CetliDock *cetliDock;
};

#endif // GROUP_H
