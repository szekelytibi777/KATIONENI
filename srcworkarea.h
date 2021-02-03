#ifndef SRCWORKAREA_H
#define SRCWORKAREA_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
class KatitoNeni;

class MainWindow;

class SrcWorkArea : public QLabel
{
    Q_OBJECT
public:
	SrcWorkArea(KatitoNeni *parent = 0);
	KatitoNeni *katitoNeni;
protected:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
public slots:

};

#endif // SRCWORKAREA_H
