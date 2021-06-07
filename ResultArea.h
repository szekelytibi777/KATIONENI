#pragma once
#include <QObject>
#include <QWidget>
#include "Cetli.h"

#include "SubAreas.h"
#include "CetliDock.h"
class ResultWorkArea;
class ResultArea :  public CetliDock
{
	Q_OBJECT

public:
	ResultArea(ResultWorkArea* parent = nullptr);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void addCetli(Cetli* cetli, QPoint pos = QPoint(-1, -1));
	virtual bool hasCetlies() { return !cetlies.isEmpty(); }
	void clear();

protected:
private:
};

