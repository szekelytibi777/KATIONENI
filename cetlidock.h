#ifndef CETLILIST_H
#define CETLILIST_H

#include <QObject>
#include <QWidget>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QMutex>
#include "dstworkarea.h"
#include "group.h"
#include "SubAreas.h"
class KatitoNeni;
class ResultArea;

class CetliDock : public QWidget
{
    Q_OBJECT
public:
	explicit CetliDock(DstWorkArea *parent = nullptr);
	virtual void addCetli(QImage img, QPoint pos = QPoint(-1,-1));
	void shuffle();
	void save( bool solved = false);
	void load();
	void clear();
	void setResultArea(ResultArea* ra) { resultArea = ra; }
	DstWorkArea *dstView;
	QString cetliPath;
	//Group selectedGroup();
	//void cleanupNeighbours(Cetli *cetli = 0);
	//QMap<Cetli *, Group>  groups;
	void setAreaWidth(int w){
		areaWidth = w*.85;
	}

	void clearGroups() {
		for (Group& g : groups) {
			g.clear();
		}
	}

	Cetli* selected;

	virtual bool hasCetlies() { return !cetlies.isEmpty(); }

	void addToGroup(Cetli *c, Cetli* key = 0);
	void removeFromGroup(Cetli *c);

	void addToGroup(Cetli* c, Group& g);
	bool paintEnabled;
	bool positionsAreLoaded = true;
	void handleDrop();
	QList<Cetli>& getCetlies() { return cetlies; }
	void attachToArea(Cetli& cetli, bool rearrangePrevArea = false);


protected:
	void paintEvent(QPaintEvent *event) override;
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void resizeEvent(QResizeEvent * event);

	bool mouseDrag = false;
	bool groupDrag = false;
	bool shiftDown = false;
	bool fixOrder;
	float scale;
	QList<Cetli> cetlies;
	Cetli* hooveredCetli;


	QList<Group> groups;
	Group* hooveredGroup;
	Group* selectedGroup;
	SubAreas areas;

private:

	int areaWidth;
	QPoint prevMousePos;
    QVector<QListWidgetItem> listItems;
    void fillImage(QImage &img, QRgb color);
    int rand(int min = 0, int max = RAND_MAX);
	void logCetlies();
	Cetli* cetliOnPosition(const QPoint& pos);
	QPoint offset;
	QPropertyAnimation scrollAnimation;
	ResultArea* resultArea;
	bool deleteSelected = false;

	void setHooveredCetlies(QRect &rect, Cetli* m);
	void setHooveredCetlies(QPoint& point);

	void setHooveredGroup(QRect& rect);
	void setHooveredGroup(QPoint& point);


	QRecursiveMutex  mutexPaint;
	QList<Cetli*> hooveredCetlies;

	

	
	void snap(Cetli* c0, Cetli *c1);
	QPoint moveStart;
	QPoint transformed(const QPoint &p);
	const int gap;
	int rowHeight;
	void remove(int id);

	void reArrange(SubArea* area = 0);
	static Cetli nullCetli;

	QPoint actPos;
	
signals:

public slots:
    void onCetliAdded(QImage &img);
	void onFixOrderToggled(bool);
	void scrollTimerUpdate();
};
#endif // CETLILIST_H
