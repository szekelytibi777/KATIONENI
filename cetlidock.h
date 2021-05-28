#ifndef CETLILIST_H
#define CETLILIST_H

#include <QObject>
#include <QWidget>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include "dstworkarea.h"
#include "group.h"
class KatitoNeni;

class CetliDock : public QWidget
{
    Q_OBJECT
public:
	explicit CetliDock(DstWorkArea *parent = nullptr);
	void addCetli(QImage img);
	void shuffle();
	void save( bool solved = false);
	void load();
	void clear();
	DstWorkArea *dstView;
	QString cetliPath;
	Group selectedGroup();
	void cleanupNeighbours(Cetli *cetli = 0);
	QMap<Cetli *, Group>  groups;
	void setAreaWidth(int w){
		areaWidth = w*.85;
	}

	void addToGroup(Cetli *c, Cetli* key = 0);
	void removeFromGroup(Cetli *c);

protected:
	void paintEvent(QPaintEvent *event) override;
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void keyPressEvent(QKeyEvent * event);
    virtual void resizeEvent(QResizeEvent * event);
private:
	bool fixOrder;
	float scale;
	int areaWidth;
    QVector<QListWidgetItem> listItems;
    void fillImage(QImage &img, QRgb color);
    int rand(int min = 0, int max = RAND_MAX);
	void fixElementPositions(Group group);
	void walkNeighbours(Cetli *c, QList<Cetli*> &g);

	QList<Cetli> cetlies;
	Cetli *selected;
	Cetli *snap(Cetli *s);
	Cetli *snap(Group& group);
	void unhookSelected();
	QPoint moveStart;
	QPoint transformed(const QPoint &p);
	const int gap;
	int rowHeight;
	void remove(int id);

	void reArrange();
	static Cetli nullCetli;

	QPoint actPos;
signals:

public slots:
    void onCetliAdded(QImage &img);
	void onCetliScaleChanged(int);
	void onFixOrderToggled(bool);
};
#endif // CETLILIST_H
