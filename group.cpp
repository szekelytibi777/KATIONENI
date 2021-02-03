#include "group.h"
#include "cetlidock.h"

Group::Group(CetliDock *dock)
	: cetliDock(dock)
	, mainCetli(0)
{

}

void Group::add(Cetli *c)
{
	if(cetliDock){
		cetliDock->addToGroup(c, mainCetli);
	}
}

Cetli *Group::getMainCetli() const
{
	if(mainCetli == 0){
		foreach(Cetli *c, *this){
			int count = 0;
			for(int i = 0; i < 4; i++){
				if(c->neigbour[i])
					count++;
			}
			if(count == 1 || count == 0){
				mainCetli = c;
				break;
			}
		}
	}
	return mainCetli;
}

QString Group::mainCetliName() const
{
	return getMainCetli() ? mainCetli->name : "(null)";

}

QString Group::toString() const
{
	QString ret=QString("[") + mainCetliName() + QString("]:");
	foreach(Cetli *c, *this){
		if(ret[ret.size()-1] != ':')
			ret += ", ";
		ret+=c->name;
	}
	return ret;
}
