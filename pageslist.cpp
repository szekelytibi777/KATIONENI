#include "pageslist.h"
#include <QDir>
#include <QApplication>

#include <QStandardItemModel>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QAbstractItemView>
#include <QLayout>
#include <QThread>
#include <QFile>
#include <QImageReader>
#include <QImageWriter>
#include <QListView>
#include "Log.h"

PagesList::PagesList(QWidget *parent)
	: QListWidget(parent)
    , pagesPath("")
    , settings("TBSoft", "KATITONENI")
{
    pagesPath = settings.value("pagesPath", QDir::currentPath()+"/SCANNEDPAGES").toString();
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setMinimumWidth(200);
	setMaximumWidth(200);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

PagesList::~PagesList()
{
    settings.setValue("pagesPath", pagesPath);
}

void PagesList::onPagesPathChanged(QString path)
{
    pagesPath = path;
    updateList();
}

void PagesList::updateList()
{
	setResizeMode(QListWidget::Fixed);
	QDir dir(pagesPath+"/thumbnails");
	if(!dir.exists())
		createThumbnails();
	QStringList entries = dir.entryList();
	
	listItems.resize(entries.size());

	Log::writeLine(dir.absolutePath());
	Log::writeLine(QString("entries:   ")+QString::number(entries.size()));
	Log::writeLine(QString("listItems: ") + QString::number(listItems.size()));
	setIconSize(QSize(150,200));
	setViewMode(QListWidget::IconMode);
	//clear();

	for(int i = 0; i < entries.size();i++)
	{
		QString &entry = entries[i];
		if(entry.contains(".jpg"))
		{
	
			QListWidgetItem &item = listItems[i];
			QString p = QString("%1/thumbnails/%2").arg(pagesPath).arg(entry);
			QString b = QString("%1/%2").arg(pagesPath).arg(entry);
			item.setData(Qt::UserRole, b);
			item.setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
			QImageReader reader(p);
			QImage img = reader.read();
			setIconSize(img.size());
			//Log::writeLine(p); Log::writeLine(reader.errorString());
			if(!img.isNull())
			{				
				QPixmap pm = QPixmap::fromImage(img);
				if(!pm.isNull()){
					QIcon icon(pm);
					item.setIcon(icon);
					item.setText(entry);
					addItem(&item);
					repaint();
				}
			}
		}
	}
}

void PagesList::createThumbnails()
{
	QDir dir;
	dir.mkpath(pagesPath + "/thumbnails");
	dir.setPath(pagesPath);
	QStringList entries = dir.entryList();
	for(int i = 0; i < entries.size();i++)
	{
		QString &entry = entries[i];
		if(entry.contains(".jpg"))
		{
			QString p = QString("%1/%2").arg(pagesPath).arg(entry);
			QString t = QString("%1/thumbnails/%2").arg(pagesPath).arg(entry);
			QImageWriter writer(t);
			QImageReader reader(p);
			QImage img = reader.read();
			if(!img.isNull())
			{
				writer.write(img.scaledToWidth(160));
			}
		}
	}
}

void PagesList::resizeEvent(QResizeEvent * event)
{
	QListWidget::resizeEvent(event);
	resize(size());
    //pagesView->layout()->setGeometry(rect());
}
