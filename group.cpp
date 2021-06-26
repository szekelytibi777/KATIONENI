#include "group.h"
#include "cetlidock.h"

Group::Group(CetliDock *dock)
{
}

void Group::addOnce(Cetli *c)
{
	if (!contains(c))
		push_back(c);
}


QString Group::toString() const
{
	QString ret=QString("[");
	for(Cetli *c : *this){
		ret+=c->name += ", ";
	}
	ret += QString("]");
	return ret;
}
