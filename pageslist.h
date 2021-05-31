#ifndef PAGESLIST_H
#define PAGESLIST_H

#include <QWidget>
#include <QDockWidget>
#include <QSettings>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>

class PagesList : public QListWidget
{
    Q_OBJECT
public:
    explicit PagesList(QWidget *parent = nullptr);
    ~PagesList();
    int radius;
    void updateList();
	void createThumbnails();
	QString &path(){
		return pagesPath;
	}

	void  setPath(const QString &path){
		pagesPath = path;
	}

protected:
    virtual void	resizeEvent(QResizeEvent * event);
private:
    QString pagesPath;
    QSettings settings;
    QVector<QListWidgetItem> listItems;


signals:
    void changeProgress(int value);
    void startProgress(int max);

public slots:
    void onPagesPathChanged(QString path);
};

#endif // PAGESLIST_H
