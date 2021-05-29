#ifndef GROUP_H
#define GROUP_H

#include <QString>
#include <QObject>
#include <QList>
#include "cetli.h"


class CetliDock;
class Group : public QList<Cetli *>
{

public:
	Group(CetliDock *dock = 0);
	void addOnce(Cetli *c);
	QString toString() const;
private:
};

#endif // GROUP_H
