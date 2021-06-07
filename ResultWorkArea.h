#pragma once
#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include "helper.h"
#include "ResultArea.h"

class ResultWorkArea :  public QScrollArea
{
	Q_OBJECT

public:
	ResultWorkArea(QWidget* parent = 0);
protected:
private:
	QPoint moveStart;
	QScrollArea* scrollArea;



public slots:
	void animate();

};

