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
#include <QRandomGenerator>
#include <QScreen>
#include <QPropertyAnimation>
#include "dstworkarea.h"
#include "cetli.h"
#include "katitoneni.h"
#include <opencv2/core/core.hpp>
#include "SubArea.h"
#include "ResultArea.h"

CetliDock::CetliDock(DstWorkArea* parent)
	: QWidget(reinterpret_cast<QWidget*>(parent))
	, dstView(parent)
	, actPos(0, 50)
	, gap(5)
	, rowHeight(0)
	, selected(0)
	, moveStart(0, 0)
	, areaWidth(500)
	, scale(1.0f)
	, fixOrder(false)
	, hooveredGroup(0)
	, selectedGroup(0)
	, scrollAnimation(this, "pos")
	, prevMousePos(QPoint(0,0))
	, resultArea(0)
	, hooveredCetli(0)
	, paintEnabled(false)
{
	
	resize(4000, 2000);
	grabKeyboard();
	setMouseTracking(true);
}

void CetliDock::paintEvent(QPaintEvent *e)
{
	if (paintEnabled) {
		QPainter p;

		p.begin(this);
		p.fillRect(rect(), Qt::white);
		int w = property("width").toInt() - 3;
		int h = property("height").toInt() - 3;

		areas.paintAreas(p);

		for (int i = 0; i < cetlies.count(); i++) {
			Cetli& c = cetlies[i];
			if (!c.isAlive)
				continue;
			QSize size = c.size();
			p.drawImage(c.pos * scale, c.scaled(size * scale));
		}


#if 1

		if (true) {
			if (hooveredCetli) {

				QPoint po(hooveredCetli->pos.x() - 1, hooveredCetli->pos.y() - 1);
				QSize s(hooveredCetli->width() + 1, hooveredCetli->height() + 1);
				p.fillRect(QRect(po, s), QColor(64, 255, 64, 64));
			}
			for (Cetli* hc : hooveredCetlies) {
				QPoint po(hc->pos.x() - 1, hc->pos.y() - 1);
				QSize s(hc->size().width() + 1, hc->size().height() + 1);
				p.fillRect(QRect(po, s), QColor(255, 32, 0, 32));
			}
			if (hooveredGroup) {
				for (Cetli* c : *hooveredGroup) {
					QPoint po(c->pos.x() - 1, c->pos.y() - 1);
					QSize s(c->size().width() + 1, c->size().height() + 1);
					p.fillRect(QRect(po, s), QColor(255, 64, 64, 64));
				}
			}


			if (selected) {
				Cetli& c = *selected;
				p.drawImage(c.pos * scale, c.scaled(c.size() * scale));
				QPoint pt(c.pos.x() - 1, c.pos.y() - 1);
				QSize s(c.size().width() + 1, c.size().height() + 1);
				p.fillRect(QRect(pt, s), QColor(0, 255, 0, 64));
			}
		}



#endif
		p.end();

	}
	QWidget::paintEvent(e);
}

void CetliDock::onCetliAdded(QImage &img)
{
	QSize s = img.size() * scale;
	addCetli(img.scaled(s));
}

void CetliDock::addToGroup(Cetli* c, Group& g) {
	g.push_back(c);
}

void CetliDock::logCetlies()
{
	qDebug() << "-------------------------";
	for (Cetli &c: cetlies) {
		qDebug() << &c << c.uID;

	}
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
	int ret = QRandomGenerator::global()->bounded(min, max);
    return ret;
}

Cetli CetliDock::nullCetli;

void CetliDock::addCetli(QImage img, QPoint pos)
{
	QPoint& p = pos;
	if (p.x() < 0)
		p = actPos;
	Cetli c(img, QString("cetli%1").arg(cetlies.count(), 3, 10, QLatin1Char('0')), pos);
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
	//cleanupNeighbours();
}


void CetliDock::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!hasCetlies())
		return;

	if (event->button() == Qt::LeftButton)
	{
		ResultArea* ra = KatitoNeni::instance()->getResultArea();
		assert(ra);
		Cetli* cetli = cetliOnPosition(event->pos());
		if (cetli) {
			ra->addCetli(selected);
		}
	}
}

void CetliDock::mousePressEvent(QMouseEvent* event)
{
	if (!hasCetlies())
		return;

	if (event->button() == Qt::RightButton) {
		if (hooveredGroup)
			hooveredGroup->clear();
		return;
	}

	mouseDrag = true;
	QPoint mousePos = transformed(event->pos());
	int s = -1;
	for (Cetli& c : cetlies) {
		QRect r(c.pos, c.size());
		if (r.contains(mousePos)) {
			c.dragOffset = c.pos - event->pos();
			selected = &c;
			int i = cetlies.indexOf(*selected);
			cetlies.swapItemsAt(i, cetlies.size() - 1);
		}
	}

	selectedGroup = hooveredGroup;


	QWidget::mousePressEvent(event);
}

Cetli* CetliDock::cetliOnPosition(const QPoint& pos)
{
	Cetli* ret = 0;
	for (Cetli& c : cetlies) {
		QRect r(c.pos, c.size());
		if (r.contains(pos)) {
			ret = &c;
			return ret;
		}
	}
	return ret;
}

void CetliDock::scrollTimerUpdate()
{

}

void CetliDock::mouseMoveEvent(QMouseEvent* event)
{
	if (!hasCetlies())
		return;
	static int count = 0;	hooveredCetlies.clear();
	QPoint mousePos = event->pos();//QPoint((int)event->screenPos().x(), (int)event->screenPos().y());
	const int borderWidth = 10;
	const int step = 100;
	const int animDuration = 200;
	QSize s = KatitoNeni::mainSize;

	QPoint actPos = property("pos").toPoint();

	QPoint cornerBtm = mapToGlobal(parent()->property("rect").toRect().bottomRight());
	QPoint cornerTop = mapToGlobal(parent()->property("rect").toRect().topLeft());

	
	int tp = cornerTop.y() + borderWidth;
	int bt = cornerBtm.y() - borderWidth;
	int lf = cornerTop.x() + borderWidth;
	int rt = cornerBtm.x() - borderWidth;

	int topTriggerValue = cornerTop.y() + borderWidth;
	int bottomTriggerValue = cornerBtm.y() - borderWidth;
	int leftTriggerValue = cornerTop.x() + borderWidth;
	int rightTriggerValue = cornerBtm.x() - borderWidth;

	bool topTrigger = (mousePos.y() < topTriggerValue) && (mousePos.y() > topTriggerValue);
	bool bottomTrigger = (mousePos.y() > topTriggerValue) && (mousePos.y() < topTriggerValue);
	bool leftTrigger = (mousePos.x() < leftTriggerValue) && (mousePos.x() > leftTriggerValue);
#if 0
	bool rightTrigger = (mousePos.x() > rightTriggerValue) && (mousePos.x() < rightTriggerValue);
	qDebug() << prevMousePos << " top:" << topTriggerValue << " left:" << leftTriggerValue << " bottom:" << bottomTriggerValue << " right:"<< rightTriggerValue;
	if (topTrigger) {
		qDebug() << "TOP";
	}
	if (bottomTrigger) {
		qDebug() << "BOTTOM";
	}
	if (leftTrigger) {
		qDebug() << "LEFT";
	}
	if (rightTrigger) {
		qDebug() << "RIGHT";
	}
	/*
	if (cursorPoint.y()+offset.y() > bt) {
		if (scrollAnimation.state() != QAbstractAnimation::Running) {
			scrollAnimation.setDuration(animDuration);
			scrollAnimation.setStartValue(actPos);
			QPoint endPos(actPos.x(), actPos.y() - step);
			scrollAnimation.setEndValue(endPos);
			scrollAnimation.start();
			offset.setY(offset.y() - step);
		}
	}
	else if (cursorPoint.y() + offset.y() < tp) {
		if (scrollAnimation.state() != QAbstractAnimation::Running) {
			scrollAnimation.setDuration(animDuration);
			scrollAnimation.setStartValue(actPos);
			QPoint endPos(actPos.x(), actPos.y() + step);
			scrollAnimation.setEndValue(endPos);
			scrollAnimation.start();
			offset.setY(offset.y() + step);
		}
	}
	if (cursorPoint.x() + offset.x() < lf) {
		if (scrollAnimation.state() != QAbstractAnimation::Running) {
			scrollAnimation.setDuration(animDuration);
			scrollAnimation.setStartValue(actPos);
			QPoint endPos(actPos.x() + step, actPos.y());
			scrollAnimation.setEndValue(endPos);
			scrollAnimation.start();
			offset.setX(offset.x() + step);
		}
	}
	else if (cursorPoint.x() + offset.x() > rt) {
		if (scrollAnimation.state() != QAbstractAnimation::Running) {
			scrollAnimation.setDuration(animDuration);
			scrollAnimation.setStartValue(actPos);
			QPoint endPos(actPos.x() - step, actPos.y());
			scrollAnimation.setEndValue(endPos);
			scrollAnimation.start();
			offset.setX(offset.x() - step);
		}
	}
	*/
#endif
	if (mouseDrag && selected) {

		QPoint newPos = mousePos + selected->dragOffset;
		QPoint groupDrag = newPos - selected->pos;
		//setProperty("pos", actPos);
		
		selected->pos = newPos;
		selected->selected = false;

		if (selectedGroup) {
			for (Cetli* c : *selectedGroup) {
				if (c != selected) {
					c->pos += groupDrag;
				}
			}
		}
		
		
		
		QPoint po(selected->pos.x() - 1, selected->pos.y() - 1);
		QSize s(selected->size().width() + 1, selected->size().height() + 1);
		QRect r(po, s);	
		setHooveredGroup(r);
		setHooveredCetlies(r, selected);
	}
	else {
		
		setHooveredCetlies(mousePos);
		setHooveredGroup(mousePos);
	}
	
	//setHooveredCetlies(mousePos);
	hooveredCetli = cetliOnPosition(mousePos);
	if (hooveredCetli)
		qDebug() << hooveredCetli;

	prevMousePos = mousePos;
	repaint();
	
	QWidget::mouseMoveEvent(event);
}



void CetliDock::mouseReleaseEvent(QMouseEvent * event)
{
	if (!hasCetlies())
		return;

	mouseDrag = false;
	QPoint mousePos = transformed(event->pos());

	
	if (shiftDown && selected) {
		SubArea* area = areas.area(mousePos);
		if (area) {
			selected->pos = area->actPos() + area->areaOffset();
			area->allocSize(selected->size());
		}
	}
	else {


		if (mouseDrag && selected) {
			selected->pos = mousePos + selected->dragOffset;
		}

		if (!hooveredCetlies.empty()) {
			Cetli* hc = hooveredCetlies.back();
			if (hc && selected) {
				snap(hc, selected);
				int i = cetlies.indexOf(*hc);
				cetlies.swapItemsAt(i, cetlies.size() - 1);

				Group g;
				Group* gp = &g;
				if (hooveredGroup)
					gp = hooveredGroup;
				gp->addOnce(selected);
				gp->addOnce(hc);
				groups.push_back(g);
				selected = hooveredCetlies.back();
			}
		}

		if (event->button() == Qt::RightButton) {
			if (hooveredGroup)
				hooveredGroup->clear();
			if (selectedGroup)
				selectedGroup->clear();
			if (selected) {
				selected = 0;
			}
			repaint();
			return;
		}
	}
	//hooveredGroup = 0;
	hooveredCetli = 0;
	repaint();
	QWidget::mouseReleaseEvent(event);
}



void CetliDock::setHooveredCetlies(QRect &rect, Cetli* m)
{
	for (Cetli& c : cetlies) {
		if (&c != m) {
			QPoint p(c.pos.x() - 1, c.pos.y() - 1);
			QSize s(c.size().width() + 1, c.size().height() + 1);
			QRect r(p, s);
			if (rect.intersects(r)) {
				hooveredCetlies.push_back(&c);
			}
		}
	}

}

void CetliDock::setHooveredCetlies(QPoint& point)
{
	for (Cetli& c : cetlies) {
		QPoint p(c.pos.x() - 1, c.pos.y() - 1);
		QSize s(c.size().width() + 1, c.size().height() + 1);
		QRect r(p, s);
		if (r.contains(point)) {
			hooveredCetlies.push_back(&c);
		}
	}
}

void CetliDock::setHooveredGroup(QRect& rect)
{
	if (true /* || mutexHoovered.tryLock(10)*/) {
		//mutexHoovered.lock();
		hooveredGroup = 0;
		for (Group& g : groups) {
			for (Cetli* c : g) {
				QPoint p(c->pos.x() - 1, c->pos.y() - 1);
				QSize s(c->size().width() + 1, c->size().height() + 1);
				QRect r(p, s);
				if (r.intersects(rect)) {
					hooveredGroup = &g;
					return;
				}
			}
		}
		//mutexHoovered.unlock();
	}
}

void CetliDock::setHooveredGroup(QPoint& point)
{
	if (true /* || mutexHoovered.tryLock(10)*/) {
		//mutexHoovered.lock();
		hooveredGroup = 0;
		for (Group& g : groups) {
			for (Cetli* c : g) {
				QPoint p(c->pos.x() - 1, c->pos.y() - 1);
				QSize s(c->width() + 1, c->height() + 1);
				QRect r(p, s);
				if (r.contains(point)) {
					hooveredGroup = &g;
					return;
				}
			}
		}
		//mutexHoovered.unlock();
	}
}


void CetliDock::keyPressEvent(QKeyEvent * event)
{

	if (event->key() == Qt::Key_Shift) {
		shiftDown = true;
	}

	if(event->key() == Qt::Key_Delete && selected){
		remove(selected->uID);
		reArrange();
		repaint();
	}

	if(event->key() == Qt::Key_Escape && selected){
		//unhookSelected();
		if (selected) {
			selected->selected = false;
			selected = 0;
		}
		repaint();
	}

	QWidget::keyPressEvent(event);
}

void CetliDock::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Shift) {
		shiftDown = false;
	}
	QWidget::keyReleaseEvent(event);
}

void CetliDock::shuffle()
{
	if (selected) {
		selected->selected = false;
		selected = 0;
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(cetlies.begin(), cetlies.end(), std::default_random_engine(seed));
	//reArrange();
	repaint();
}

void CetliDock::save(bool solved)
{
	QDir dir(cetliPath);
	dir.removeRecursively();
	dir.mkpath(cetliPath);

	QFile file(cetliPath+"/positions.dat");
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream  stream(&file);
		for (int i = 0; i < cetlies.count(); i++) {
			Cetli& c = cetlies[i];
			if (!c.isAlive)
				continue;
			//QString fn = QString("%1(%2,%3)").arg(c.name).arg(c.pos.x()).arg(c.pos.y());
			stream << c.name << c.pos;
		}
		file.close();
	}

	for(int i = 0; i < cetlies.count(); i++ ){
		Cetli &c =  cetlies[i];
		if(!c.isAlive)
			continue;
		QString fn = QString("%1/%2.png").arg(cetliPath).arg(c.name);
		c.save(fn);
	}
}

void CetliDock::load()
{
	paintEnabled = false;
	if(true) {
		
		ResultArea* ra = KatitoNeni::instance()->getResultArea();
		ra->paintEnabled = false;
		ra->clear();
		QDir dir(cetliPath);
		QStringList entries = dir.entryList();
		clear();
		hooveredCetlies.clear();

		clearGroups();
		ra->clearGroups();
		selected = 0;
		ra->selected = 0;
		groups.clear();
		hooveredGroup = 0;
		areas.deleteAreas();
		areas.createAreas(size(), 3, 4);

		paintEnabled = true;
		ra->paintEnabled = true;


		QFile fp(QString("%1/positions.dat").arg(cetliPath));
		QMap<QString, QPoint> map;
		if (fp.open(QFile::ReadOnly)) {
			QDataStream data(&fp);
			while (!data.atEnd()) {
				QString name;
				QPoint pos;
				data >> name >> pos;
				map[name] = pos;
			}
			fp.close();
		}

		QString fn = QString("%1/solution.txt").arg(cetliPath);
		QFile f(fn);
		if (f.open(QFile::ReadOnly)) {
			QString s = QString(f.readAll());
			QStringList lines = s.split("\r\n");
			foreach(const QString & l, lines) {
				int i = l.indexOf(":");
				QStringList names = l.mid(i + 1).split(",");
				foreach(const QString n, names) {
					QString e = n.trimmed() + ".png";
					QString p = QString("%1/%2").arg(cetliPath).arg(e);
					QImageReader reader(p);
					QImage img = reader.read();
					if (!img.isNull())
					{
						QSize s = img.size() * scale;
						if (map.find(e) != map.end())
							addCetli(img.scaled(s), map[e]);
						else
							addCetli(img.scaled(s));
					}
				}
			}
		}
		else {
			for (int i = 0; i < entries.size(); i++)
			{
				QString& entry = entries[i];
				if (entry.contains(".png"))
				{
					QString p = QString("%1/%2").arg(cetliPath).arg(entry);
					QImageReader reader(p);
					QImage img = reader.read();
					if (!img.isNull())
					{
						QSize s = img.size() * scale;
						QString key = entry.trimmed().remove(".png");
						if (map.find(key) != map.end())
							addCetli(img, map[key]);
						else
							addCetli(img);
					}
				}
			}
		}

	}
	
}

void CetliDock::snap(Cetli* c0, Cetli* c1)
{
	QRect r0(c0->pos, c0->rect().size());
	QRect r1(c1->pos, c1->rect().size());

	int ld = qAbs(r0.left() - r1.right());
	int rd = qAbs(r0.right() - r1.left());
	int td = qAbs(r0.top() - r1.bottom());
	int bd = qAbs(r0.bottom() - r1.top());

	if(ld < rd && ld < td && ld < bd){//LEFT
		//qDebug() << "LEFT";
		c1->pos = c0->pos - QPoint(c0->width(), 0);
	}
	else if(rd < ld && rd < td && rd < bd){ //RIGHT
	//	qDebug() << "RIGHT";
		c1->pos = c0->pos + QPoint(c0->width(), 0);
	}
	else if(td < ld && td < rd && td < bd){ //TOP
	//	qDebug() << "TOP";
		c1->pos = c0->pos - QPoint(0, c1->height());
	}
	else if(bd < ld && bd < td && bd < ld){//BOTTOM
	//	qDebug() << "BOTTOM";
		c1->pos = c0->pos + QPoint(0, c0->height());
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
	if (selected) {
		selected->selected = false;
		selected = 0;
	}
	update();
}

QPoint CetliDock::transformed(const QPoint &p)
{
	return p/scale;
}

void CetliDock::onFixOrderToggled(bool state)
{
	fixOrder = state;
}
