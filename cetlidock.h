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
	//Group selectedGroup();
	void cleanupNeighbours(Cetli *cetli = 0);
	//QMap<Cetli *, Group>  groups;
	void setAreaWidth(int w){
		areaWidth = w*.85;
	}

	void addToGroup(Cetli *c, Cetli* key = 0);
	void removeFromGroup(Cetli *c);

	void addToGroup(Cetli* c, Group& g);

protected:
	void paintEvent(QPaintEvent *event) override;
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void keyPressEvent(QKeyEvent * event);
    virtual void resizeEvent(QResizeEvent * event);
private:
	bool mouseDrag = false;
	bool groupDrag = false;
	bool fixOrder;
	float scale;
	int areaWidth;
    QVector<QListWidgetItem> listItems;
    void fillImage(QImage &img, QRgb color);
    int rand(int min = 0, int max = RAND_MAX);
	void walkNeighbours(Cetli* c, QList<Cetli*>& g);
	void logCetlies();

	void setHooveredCetlies(QRect &rect, Cetli* m);
	void setHooveredCetlies(QPoint& point);

	void setHooveredGroup(QRect& rect);
	void setHooveredGroup(QPoint& point);

	QList<Cetli> cetlies;
	QList<Cetli*> hooveredCetlies;
	QList<Group> groups;
	Cetli *selected;
	Group* hooveredGroup;
	Group* selectedGroup;
	void snap(Cetli* c0, Cetli *c1);
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
	void onFixOrderToggled(bool);
};
#endif // CETLILIST_H
