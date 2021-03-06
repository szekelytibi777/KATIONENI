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
	, gap(3)
	, rowHeight(0)
	, selected(0)
	, moveStart(0, 0)
	, areaWidth(500)
	, scale(1.0f)
	, fixOrder(false)
	, hooveredGroup(0)
	, selectedGroup(0)
	, scrollAnimation(this, "pos")
	, prevMousePos(QPoint(0, 0))
	, resultArea(0)
	, hooveredCetli(0)
	, paintEnabled(false)
{

	resize(2000, 1200);
	grabKeyboard();
	setMouseTracking(true);
}

void CetliDock::paintEvent(QPaintEvent *e)
{
	if (paintEnabled) {
		QPainter p;

		p.begin(this);
		p.fillRect(rect(), Qt::white);
	

		areas.paintAreas(p);

		for (int i = 0; i < cetlies.count(); i++) {
			Cetli& c = cetlies[i];
			if (!c.isAlive)
				continue;
			p.drawImage(c.pos, c.imgScaled);
		}


#if 1

		if (true) {
			if (hooveredGroup) {
				for (Cetli* c : *hooveredGroup) {
					QPoint po(c->pos.x() - 1, c->pos.y() - 1);
					QSize s(c->imgScaled.size().width() + 1, c->imgScaled.size().height() + 1);
					
					p.fillRect(QRect(po*scale, s*scale), QColor(255, 64, 64, 64));
				}
			}
			else {
				for (Cetli* hc : hooveredCetlies) {
					QPoint po(hc->pos.x() - 1, hc->pos.y() - 1);
					QSize s(hc->imgScaled.size().width() + 1, hc->imgScaled.size().height() + 1);
					//s *= scale;
					p.fillRect(QRect(po*scale, s*scale), QColor(255, 32, 0, 32));
				}
			}
		}

		if (selected) {
			QPoint po(selected->pos.x() - 1, selected->pos.y() - 1);
			QSize s(selected->imgScaled.size().width() + 1, selected->imgScaled.size().height() + 1);
			//s *= scale;
			p.fillRect(QRect(po * scale, s * scale), QColor(255, 32, 128, 32));

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
	SubArea* area = areas.area(0);

	bool alloc = pos == QPoint(-1, -1);
	
	Cetli c(img, QString("cetli%1").arg(cetlies.count(), 3, 10, QLatin1Char('0')), pos);
	c.pos = alloc ? area->actPos() + area->areaOffset() : pos ;
	if (alloc)
		area->allocSize(c.imgScaled.size());
	SubArea* a = areas.area(c.scaledRect());
	c.setActiveArea(a);
	cetlies.push_back(c);
	repaint();
}

void CetliDock::attachToArea(Cetli& cetli, bool rearrangePrevArea) {
	SubArea* prevArea = cetli.getActiveArea();
	SubArea* currentArea = areas.area(cetli.scaledRect());
	
	if (currentArea) {
		cetli.pos = currentArea->absPos();
		currentArea->allocSize(cetli.scaledSize());
		cetli.setActiveArea(currentArea);
		if (prevArea && rearrangePrevArea && (prevArea != currentArea))
		{
			int oldX = areas.area(0)->rect().right() + 1;
			QSize s = areas.area(0)->getContentsBoud(cetlies);
			int newX = s.width();

			if (abs(oldX-newX) > 10) {
				areas.adjustX(oldX, newX);
			}
			areas.rearrangeAreaContents(cetlies);
		}
		if (currentArea) {
			currentArea->rearrangeContent(cetlies);
		}
	}
}




void CetliDock::reArrange(SubArea * area)
{
	SubArea* a = area ? area : areas.area(0);
	a->reset();
	if(selected)
		selected->selected = false;
	selected = 0;
	for (int i = 0; i < cetlies.count(); i++) {
		Cetli& c = cetlies[i];
		c.pos = a->actPos() + a->areaOffset();
		a->allocSize(c.imgScaled.size());
	}

}


void CetliDock::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!hasCetlies())
		return;

	if (event->button() == Qt::LeftButton)
	{
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
	QPoint mousePos = transformed(event->pos()/scale);
	for (Cetli& c : cetlies) {
		QRect r(c.pos, c.imgScaled.size());
		if (r.contains(mousePos)) {
			
			c.dragOffset = (c.pos - event->pos() / scale) ;
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
		QRect r(c.pos, c.imgScaled.size());
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
	QPoint mousePos = event->pos()/scale;//QPoint((int)event->screenPos().x(), (int)event->screenPos().y());
	const int borderWidth = 10;
	const int step = 100;
	const int animDuration = 200;
	QSize s = KatitoNeni::mainSize;

	QPoint actPos = property("pos").toPoint();

	QPoint cornerBtm = mapToGlobal(parent()->property("rect").toRect().bottomRight());
	QPoint cornerTop = mapToGlobal(parent()->property("rect").toRect().topLeft());

	
#if 0
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
		SubArea* area = areas.area(selected->scaledRect());
		qDebug() << "["<<area<<"]";
		selected->selected = false;

		if (selectedGroup) {
			for (Cetli* c : *selectedGroup) {
				if (c != selected) {
					c->pos += groupDrag;
				}
			}
		}
		
		
		
		QPoint po(selected->pos.x() - 1, selected->pos.y() - 1);
		QSize s(selected->imgScaled.size().width() + 1, selected->imgScaled.size().height() + 1);
	
		QRect r(selected->pos,selected->scaledSize());
		setHooveredGroup(r);
		setHooveredCetlies(r, selected);

		//qDebug() << hooveredCetlies.size();
	}
	else {
		
		setHooveredCetlies(mousePos);
		setHooveredGroup(mousePos);
	}
	if (selected) {
		QRect r(selected->pos, selected->scaledSize());
		qDebug() << r;
		setHooveredCetlies(r, selected);
	}
	else
		setHooveredCetlies(mousePos);
	hooveredCetli = cetliOnPosition(mousePos);
	

	prevMousePos = mousePos;
	repaint();
	
	QWidget::mouseMoveEvent(event);
}



void CetliDock::mouseReleaseEvent(QMouseEvent * event)
{
	if (!hasCetlies())
		return;

	selected = hooveredCetli;
	selectedGroup = hooveredGroup;
	mouseDrag = false;
	QPoint mousePos = transformed(event->pos()/scale);
	

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
	else if (event->button() == Qt::LeftButton) {
		if (shiftDown && selected) {
			attachToArea(*selected, true);
		}
		else if (!KatitoNeni::editEnabled && selected) {
			SubArea* area = selected->getActiveArea();
			int bottom = 0;
			if (area) {
				selected->setActiveArea(0);
				SubArea* poolArea1 = areas.area(0);
				SubArea* poolArea2 = areas.area(1);
				QSize s1 = poolArea1->getContentsBoud(cetlies);
				QSize s2 = poolArea2->getContentsBoud(cetlies);
				QRect r1 = poolArea1->rect();
				QRect r2 = poolArea2->rect();
				int bottom = qMax(r1.bottom(), r2.bottom());
			}
			
			SubArea* newArea = areas.area(mousePos);
			if (newArea) {
				QRect rNew = newArea->rect();
				rNew.setTop(bottom);
				newArea->changeRect(rNew);
			}
			repaint();
			if (!hooveredCetlies.empty()) {
				Cetli* hc = hooveredCetlies.back();
				if ((hc && selected) && (hc != selected)) {
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
			
		}
		else {
			if (mouseDrag && selected) {
				selected->pos = mousePos + selected->dragOffset;
			}
			if (!hooveredCetlies.empty()) {
				Cetli* hc = hooveredCetlies.back();
				if ((hc && selected) && (hc != selected)) {
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
			
			qDebug() << "==============";
		}
		handleDrop();
	}
	
	
	//hooveredGroup = 0;
	hooveredCetli = 0;
	repaint();
	QWidget::mouseReleaseEvent(event);
}


void CetliDock::handleDrop()
{
	int count = 0;
	for (Cetli& c : cetlies) {
		if (c.getActiveArea() && c.getActiveArea() == areas.area(0)) {
			count++;
		}
	}
	qDebug() << "-----------\n" << count << "\n-----------";
}


void CetliDock::setHooveredCetlies(QRect &rect, Cetli* m)
{
	for (Cetli& c : cetlies) {
		if (&c != m) {
			QPoint p(c.pos.x() - 1, c.pos.y() - 1);
			QSize s(c.size().width() + 1, c.imgScaled.size().height() + 1);
			QRect r(p, c.scaledSize() );
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
		QSize s(c.size().width() + 1, c.imgScaled.size().height() + 1);
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
				QSize s(c->imgScaled.size().width() + 1, c->imgScaled.size().height() + 1);
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


	if(event->key() == Qt::Key_Delete){
		if (selected) {
			remove(selected->uID);
			reArrange();
			repaint();
		}
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
		areas.saveAreas(stream);
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
		QDir dir(cetliPath);
		QStringList entries = dir.entryList();
		clear();
		hooveredCetlies.clear();

		clearGroups();
		selected = 0;
		groups.clear();
		hooveredGroup = 0;
		areas.deleteAreas();
		

		paintEnabled = true;

		
		QFile fp(QString("%1/positions.dat").arg(cetliPath));
		QMap<QString, QPoint> map;
		if (fp.open(QFile::ReadOnly)) {
			
			QDataStream data(&fp);
			areas.loadAreas(data);
			
			while (!data.atEnd()) {
				QString name;
				QPoint pos;
				data >> name >> pos;
				pos.setX(pos.x());
				map[name] = pos;
			}
			
			fp.close();		
		}
		else
			areas.createAreas(size(),true);
		
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
		else{
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
		/*
		int oldX = areas.area(0)->rect().right()+1;
		QSize s = areas.area(0)->getContentsBoud (cetlies);
		int newX = s.width();
		areas.adjustX(oldX, newX);
		*/
		repaint();
		

	}
	
}

void CetliDock::snap(Cetli* c0, Cetli* c1)
{
	QRect r0(c0->pos, c0->imgScaled.size());
	QRect r1(c1->pos, c1->imgScaled.size());

	int rd = abs(r0.left() - r1.right());
	int ld = abs(r0.right() - r1.left());
	int td = abs(r0.top() - r1.bottom());
	int bd = abs(r0.bottom() - r1.top());

	int h = qMin(rd, ld);
	int v = qMin(td, bd);


	enum SideEnum {
		None,
		Bottom,
		Top,
		Left,
		Right
	};

	
	if(r0.intersects(r1))
	{
		SideEnum se = None;
		if (h < v) {
			se = rd < ld ? Left : Right;
		}
		else
		{
			se = td < bd ? Top : Bottom;
		}

		
		switch (se) {
			case Top:
				qDebug() << "TOP";
				c1->pos = c0->pos - QPoint(0, c1->scaledSize().height());
				break;
			case Bottom:
				qDebug() << "Bottom";
				c1->pos = c0->pos + QPoint(0, c0->imgScaled.height());
				break;
			case Left:
				qDebug() << "Left";
				c1->pos = c0->pos - QPoint(c0->imgScaled.width(), 0);
				break;
			case Right:
				qDebug() << "Right";
				c1->pos = c0->pos + QPoint(c0->imgScaled.width(), 0);
				break;
			default:
				break;
		}
		repaint();
	}
	
}

void CetliDock::remove(int id)
{
	for(int i = 0;i < cetlies.count(); i++){
		Cetli &c = cetlies[i];
		if(c.uID == id)
			c.kill();
	}
}

void CetliDock::clear()
{
	areas.deleteAreas();
	areas.createAreas(size(), KatitoNeni::editEnabled);
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
