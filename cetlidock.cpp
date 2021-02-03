#include "cetlidock.h"
#include <QDir>
#include <QApplication>

#include <QStandardItemModel>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QAbstractItemView>
#include <QDragMoveEvent>
#include <QPainter>
#include <QImageReader>
#include <QTextStream>
#include "dstworkarea.h"
#include "cetli.h"

CetliDock::CetliDock(DstWorkArea *parent)
	: QWidget(reinterpret_cast<QWidget *>(parent))
	, dstView(parent)
	, actPos(0,50)
	, gap(5)
	, rowHeight(0)
	, selected(0)
	, moveStart(0,0)
	, areaWidth(500)
	, scale(1.0f)
	, fixOrder(false)
{
	resize(4000, 2000);
	grabKeyboard();
}

void CetliDock::paintEvent(QPaintEvent *e)
{
	QPainter p;
	p.begin(this);
	p.fillRect(rect(),Qt::white);
//	QList<Cetli *> group = selectedGroup();
	for(int i = 0; i < cetlies.count(); i++){
		Cetli &c = cetlies[i];
		if(!c.isAlive)
			continue;
		QSize size = c.size();
		if(!c.selected)
			p.drawImage(c.pos*scale , c.scaled(size*scale));


	}

	foreach(Cetli* c, selectedGroup()){
		QSize size = c->size()*scale;
		QPoint pos = c->pos*scale;
		p.setPen(QColor(255,0,0,64));
		p.drawImage(pos, c->scaled(size));
		QPoint po(pos.x()-1, pos.y()-1);
		QSize s(size.width()+1, size.height()+1);
		p.fillRect(QRect(po,s), QColor(255,0,0,64));
	}

	p.end();
	QWidget::paintEvent(e);
}

void CetliDock::onCetliAdded(QImage &img)
{
	QSize s = img.size() * scale;
	addCetli(img.scaled(s));
}

void CetliDock::fillImage(QImage &img, QRgb color)
{
    for(int y = 0; y < img.height(); y++ )
        for(int x = 0; x < img.width(); x++ ){
            img.setPixel(x, y, color);
        }
}

void CetliDock::resizeEvent(QResizeEvent * event)
{
	QWidget::resizeEvent(event);
    dstView->resize(size());
}

int CetliDock::rand(int min, int max)
{
    float r = (float)qrand()/(float)RAND_MAX;
    int ret = (float)min*(1-r)+(float)max*r;
    return ret;
}

Cetli CetliDock::nullCetli;

void CetliDock::addCetli(QImage img)
{
//	qDebug() << img.width() * img.height();
	Cetli c(img, QString("cetli%1").arg(cetlies.count(), 3, 10, QLatin1Char('0')), actPos);
	cetlies.push_back(c);
	if(c.height() > rowHeight)
		rowHeight = c.height();
	actPos.setX(actPos.x() + gap + c.width());
	if(actPos.x() > areaWidth){
		actPos.setX(0);
		actPos.setY(actPos.y()+rowHeight+gap);

		rowHeight = 0;
	}
	repaint();
}

void CetliDock::reArrange()
{
	actPos = QPoint(0,50);
	rowHeight = 0;
	if(selected)
		selected->selected = false;
	selected = 0;
	for(int i = 0; i < cetlies.count(); i++){
		Cetli &c = cetlies[i];
		c.pos = actPos;
		if(c.height() > rowHeight)
			rowHeight = c.height();
		actPos.setX(actPos.x() + gap + c.width());
		if(actPos.x() > areaWidth){
			actPos.setX(0);
			actPos.setY(actPos.y()+rowHeight+gap);
			rowHeight = 0;
		}
	}
	cleanupNeighbours();
}

void CetliDock::mousePressEvent(QMouseEvent * event)
{
	 if(event->button() == Qt::RightButton)
		 return;
	QPoint mousePos = transformed(event->pos());
	int s = -1;
	for(int i = 0; i < cetlies.count(); i++ ){
		Cetli &c =  cetlies[i];
		QRect r(c.pos, c.size());
		if(c.selected = r.contains(mousePos)){
			c.dragOffset = c.pos-event->pos();
			s = i;
			c.selected = true;
			selected =&c;
		}
	}
	if(s>=0)
		cetlies.move(s, cetlies.count()-1);
	Group group = selectedGroup();
	for(int i = 0;i < group.count(); i++){
		Cetli *gc = group[i];
		gc->dragOffset = gc->pos-mousePos;
	}
	QWidget::mousePressEvent(event);
}

void CetliDock::mouseReleaseEvent(QMouseEvent * event)
{
	if(cetlies.isEmpty())
		return;

	QPoint mousePos = transformed(event->pos());

	if(event->button() ==  Qt::RightButton && selected){
		unhookSelected();
		selected->selected = false;
		selected = 0;
		repaint();
		return;
	}

	if(selected)
		cetlies.last().pos =selected->pos;

	Cetli *p = snap(&cetlies.last());

	Group group = selectedGroup();

	qDebug() << selectedGroup().toString();
	groups[group.getMainCetli()] = selectedGroup();

	repaint();
	QWidget::mouseReleaseEvent(event);
}

void CetliDock::mouseMoveEvent(QMouseEvent * event)
{
	QPoint mousePos = transformed(event->pos());
	Group group = selectedGroup();
	if(selected)
	{
		selected->pos = mousePos+selected->dragOffset;
	}
	for(int i = 0;i < group.count(); i++){
		Cetli *gc = group[i];
		gc->pos = mousePos+gc->dragOffset;
	}
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CetliDock::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Delete && selected){
		remove(selected->uID);
		reArrange();
		repaint();
	}

	if(event->key() == Qt::Key_Escape && selected){

		unhookSelected();
		selected->selected = false;
		selected = 0;
		repaint();
	}

	QWidget::keyPressEvent(event);
}

void CetliDock::shuffle()
{
	if(selected)
		selected->selected = false;
	selected = 0;
	std::random_shuffle(cetlies.begin(), cetlies.end());
	reArrange();
	repaint();
}

void CetliDock::save(bool solved)
{
	QDir dir(cetliPath);
	dir.removeRecursively();
	dir.mkpath(cetliPath);

	for(int i = 0; i < cetlies.count(); i++ ){
		Cetli &c =  cetlies[i];
		if(!c.isAlive)
			continue;
		QString fn = QString("%1/%2.png").arg(cetliPath).arg(c.name);
		c.save(fn);
	}

	if(fixOrder){
		QString fn = QString("%1/solution.txt").arg(cetliPath);
		QFile f(fn);
		if(f.open(QFile::ReadWrite | QFile::Truncate)){
			QTextStream s(&f);
			foreach(const Group &g, groups.values()){
				s << g.toString() << "\r\n";
			}
		}
	}
}

void CetliDock::load()
{
	QDir dir(cetliPath);
	QStringList entries = dir.entryList();
	clear();

	QString fn = QString("%1/solution.txt").arg(cetliPath);
	QFile f(fn);
	if(f.open(QFile::ReadOnly)){
		QString s = QString(f.readAll());
		QStringList lines = s.split("\r\n");
		foreach(const QString &l, lines){
			int i = l.indexOf(":");
			QStringList names = l.mid(i+1).split(",");
			foreach(const QString n, names){
				QString e = n.trimmed() + ".png";
				QString p = QString("%1/%2").arg(cetliPath).arg(e);
				QImageReader reader(p);
				QImage img = reader.read();
				if(!img.isNull())
				{
					QSize s = img.size() * scale;
					addCetli(img.scaled(s));
				}
			}
		}
	}
	else{
		for(int i = 0; i < entries.size();i++)
		{
			QString &entry = entries[i];
			if(entry.contains(".png"))
			{
				QString p = QString("%1/%2").arg(cetliPath).arg(entry);
				QImageReader reader(p);
				QImage img = reader.read();
				if(!img.isNull())
				{
					QSize s = img.size() * scale;
					addCetli(img.scaled(s));
				}
			}
		}
	}
}

Cetli *CetliDock::snap(Group& group)
{

}


Cetli *CetliDock::snap( Cetli *s)
{
	QRect r0(s->pos, s->rect().size());
	for(int i = 0; i < cetlies.count(); i++){
		Cetli &c = cetlies[i];
		if(&c == s)
			continue;
		QRect r(c.pos, c.rect().size());
		if(r.intersects(r0)){
			int ld = qAbs(r0.left() - r.right());
			int rd = qAbs(r0.right() - r.left());
			int td = qAbs(r0.top() - r.bottom());
			int bd = qAbs(r0.bottom() - r.top());

			if(ld < rd && ld < td && ld < bd){//LEFT
				s->pos = c.pos+QPoint(c.width() , 0);
				s->neigbour[Cetli::LEFT] =  &c;
				c.neigbour[Cetli::RIGHT] = s;
			}
			else if(rd < ld && rd < td && rd < bd){ //RIGHT
				s->pos = c.pos-QPoint(s->width() , 0);
				s->neigbour[Cetli::RIGHT] =  &c;
				c.neigbour[Cetli::LEFT]  = s;
			}
			else if(td < ld && td < rd && td < bd){ //TOP
				s->pos = c.pos+QPoint(0, c.height());
				s->neigbour[Cetli::TOP] =  &c;
				c.neigbour[Cetli::BOTTOM]  = s;
			}
			else if(bd < ld && bd < td && bd < ld){//BOTTOM
				s->pos = c.pos-QPoint(0, s->height());
				s->neigbour[Cetli::BOTTOM] =  &c;
				c.neigbour[Cetli::TOP]  = s;
			}
			/*
			s->selected  = false;
			selected  = nullCetli;
			*/

			return &c;
		}
	}
	return s;
}

void CetliDock::fixElementPositions(Group group)
{
	for(int i =1; i < group.count(); i++){
		Cetli &c = *group[i];
		if(c.neigbour[Cetli::LEFT])
			c.pos = c.neigbour[Cetli::LEFT]->pos+QPoint(c.neigbour[Cetli::LEFT]->width() ,0);
		else if(c.neigbour[Cetli::LEFT])
			c.pos = c.neigbour[Cetli::RIGHT]->pos-QPoint(c.neigbour[Cetli::RIGHT]->width() ,0);
		else if(c.neigbour[Cetli::TOP])
			c.pos = c.neigbour[Cetli::TOP]->pos+QPoint(0, c.neigbour[Cetli::TOP]->height());
		else if(c.neigbour[Cetli::BOTTOM])
			c.pos = c.neigbour[Cetli::BOTTOM]->pos-QPoint(0, c.neigbour[Cetli::BOTTOM]->height());
	}
}

void CetliDock::remove(int id)
{
	int r = -1;
	for(int i = 0;i < cetlies.count(); i++){
		Cetli &c = cetlies[i];
		if(c.uID == id)
			c.kill();
	}
}

void CetliDock::clear()
{
	actPos = QPoint(0,50);
	rowHeight = 0;
	cetlies.clear();
	selected = 0;
	update();
}

Group CetliDock::selectedGroup()
{
	Group group(this);
	Cetli *c = selected;
	walkNeighbours(selected, group);
	return group;
}


void CetliDock::walkNeighbours(Cetli *c, QList<Cetli*> &g)
{
	if(c && !g.contains(c)){
		g.push_back(c);
	}
	else
		return;

	for(int i = 0; i<4; i++)
		if(c->neigbour[i])
			walkNeighbours(c->neigbour[i], g);
}

void CetliDock::unhookSelected()
{
	Group group = selectedGroup();
	foreach(Cetli*c, group){
		for(int i = 0; i < 4; i++)
			c->neigbour[i] = 0;
	}
}

void CetliDock::cleanupNeighbours(Cetli *cetli)
{
	if(cetli){
		for(int j = 0; j < 4; j++)
			cetli->neigbour[j] = 0;
	}
	else{
		for(int i = 0; i < cetlies.count(); i++){
			Cetli &c = cetlies[i];
			for(int j = 0; j < 4; j++){
				c.neigbour[j] = 0;
			}
		}
	}
}

void CetliDock::onCetliScaleChanged(int v)
{
	scale = float(v)/100;
	repaint();
//	load();
}

QPoint CetliDock::transformed(const QPoint &p)
{
	return p/scale;
}

void CetliDock::addToGroup(Cetli *c, Cetli *key)
{
	removeFromGroup(c);
	groups[key].push_back(c);
}

void CetliDock::removeFromGroup(Cetli *c)
{
	if(c->groupKey){
		groups[c->groupKey].removeOne(c);
		c->groupKey = 0;
		groups[0].push_back(c);
	}
}

void CetliDock::onFixOrderToggled(bool state)
{
	fixOrder = state;
	qDebug() << state;
}
