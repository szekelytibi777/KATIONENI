#include "katitoneni.h"
#include "pageslist.h"
#include "srcworkarea.h"
#include "ResultWorkArea.h"
#include  "ResultArea.h"
#include "cetlidock.h"
#include <QtWidgets>

#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif


bool KatitoNeni::editEnabled = true;
QSize KatitoNeni::mainSize;
KatitoNeni* KatitoNeni::instance_ = 0;
KatitoNeni* KatitoNeni::instance()
{
	assert(KatitoNeni::instance_);
	return KatitoNeni::instance_;
}

KatitoNeni::KatitoNeni()
   : resultWorkArea(new ResultWorkArea(this))
   , srcWorkArea(new SrcWorkArea(this))
   , dstWorkArea(new DstWorkArea(this))
   , cetliDock(new CetliDock(dstWorkArea))
   , resultArea(new ResultArea(resultWorkArea))
   , splitter(new QSplitter(this))
   , pagesList(new PagesList(this))
   , toolBar(new QToolBar(this))
   , scale(.3f)
   , settings("TBSoft", "KATITONENI")
   , slicerState(KatitoNeni::editEnabled ? 0 : 1)
   , cb(new QCheckBox(this))
   , sliderCetliScale(new QSlider(this))
   , scaleFactor(1.0)
   , logToFile()
{
	KatitoNeni::instance_ = this;
	createActions();
	scrollArea[0] = new QScrollArea;
	scrollArea[1] = new QScrollArea;
	scrollArea[2] = dstWorkArea;
	scrollArea[3] = resultWorkArea;


	splitter->addWidget(scrollArea[0]);
	splitter->addWidget(scrollArea[1]);
	splitter->addWidget(scrollArea[2]);
	splitter->addWidget(scrollArea[3]);

	QString path = QDir::currentPath()+"/SCANNEDPAGES/oldal003.jpg";
	actImagePath = "";// settings.value("actImagePath", path).toString();

	
	if (KatitoNeni::editEnabled) {
		slicerState = 0;// settings.value("slicerState", 0).toInt();
	}
	
	setGeometry(settings.value("mainRect", QRect(100, 100, 600, 400)).toRect());
	setSlicers();
	cetliDock->cetliPath = actImagePath.replace(".jpg", "");

	mainSize = property("size").toSize();
	QSize ps = pagesList->property("size").toSize();
	mainSize.setWidth(mainSize.width() - ps.width());



	sliderCetliScale->setRange(52, 200);
	sliderCetliScale->setOrientation(Qt::Horizontal);
	sliderCetliScale->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	sliderCetliScale->setMaximumWidth(500);
	connect(sliderCetliScale, SIGNAL(valueChanged(int)), cetliDock, SLOT(onCetliScaleChanged(int)));
	connect(sliderCetliScale, SIGNAL(valueChanged(int)), this, SLOT(onCetliScaleChanged(int)));
	if (!KatitoNeni::editEnabled) {
		cb->hide();
		sliderCetliScale->hide();
		scrollArea[1]->hide();
	}

	srcWorkArea->setBackgroundRole(QPalette::Base);
	srcWorkArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	srcWorkArea->setScaledContents(true);

	scrollArea[1]->setBackgroundRole(QPalette::Dark);
	scrollArea[1]->setWidget(srcWorkArea);
	scrollArea[1]->setVisible(false);
	setCentralWidget(splitter);

	scrollArea[0]->setLayout(new QHBoxLayout);
	scrollArea[0]->layout()->addWidget(pagesList);

	scrollArea[2]->setWidget(cetliDock);
	scrollArea[3]->setWidget(resultArea);
	pagesList->updateList();//onPagesPathChanged("c:/Users/szekelytibi/Documents/WORK/KATITONENI/SCANNEDPAGES");

	connect(pagesList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(onItemDoubleClicked(QListWidgetItem *)));
	if(!actImagePath.isEmpty())
		setImage(actImagePath);

	sliderCetliScale->setValue(settings.value("cetliScle", 100).toInt()); // Loat cetlies in cetlidock too

}

KatitoNeni::~KatitoNeni()
{
	settings.setValue("mainRect", geometry());
	settings.setValue("slicerState",slicerState);
	settings.setValue("actImagePath",actImagePath);
	settings.setValue("cetliScle", sliderCetliScale->value());
}


void KatitoNeni::setImage(const QString &imagePath)
{
	QImageReader reader(imagePath);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
								 tr("Cannot load %1: %2")
								 .arg(QDir::toNativeSeparators(imagePath)));
		return;
	}

	srcImage = newImage;
	refreshSrcImage();
}

void KatitoNeni::refreshSrcImage()
{
	QSize s = srcImage.size() * scaleFactor;
	srcWorkArea->setPixmap(QPixmap::fromImage(srcImage.scaled(s)));

	scrollArea[1]->setVisible(true);
	srcWorkArea->setScaledContents(false);
	srcWorkArea->setGeometry(0,0,s.width(),s.height());
	repaint();
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void KatitoNeni::openDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
												 pagesList->path(),
												 QFileDialog::ShowDirsOnly
												 | QFileDialog::DontResolveSymlinks);    QFileDialog dialog(this, tr("Open File"));
	pagesList->setPath(dir);
	pagesList->updateList();
}

void KatitoNeni::resizeEvent(QResizeEvent* event)
{
	QSize ps = pagesList->property("size").toSize();
	mainSize = property("size").toSize();

	mainSize.setWidth(mainSize.width() - ps.width());
	QWidget::resizeEvent(event);
}

void KatitoNeni::fitToWindow()
{
	scrollArea[1]->setWidgetResizable(true);
}

QAction *KatitoNeni::createToolbarAction(const QString &iconFileName, const QString &name, const QString &toolTip, const char*  method)
{
	const QIcon openIcon = QIcon::fromTheme("document-new", QIcon(":/res/"+iconFileName));
	QAction *action = new QAction(openIcon, name, this);
	action->setStatusTip(toolTip);
	connect(action, SIGNAL(triggered(bool)), this, method);
	return action;
}


void KatitoNeni::createActions()
{
	//    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

	
	if (KatitoNeni::editEnabled) {
		toolBar = addToolBar(tr("KatitoNeni"));
		toolBar->addAction(createToolbarAction("open.png", tr("&Open"), tr("OLdalak könyvtár megnyitása"), SLOT(openDir())));
		toolBar->addAction(createToolbarAction("shuffle.png", tr("&Shuffle"), tr("Cetlik összekeverése"), SLOT(shuffle())));
		toolBar->addAction(createToolbarAction("sidebar.png", tr("&AlterView"), tr("Nézet váltása"), SLOT(alterSlicers())));
		toolBar->addAction(createToolbarAction("save.png", tr("&Save"), tr("Cetlik mentése"), SLOT(saveCetlies())));
		cb->setToolTip(tr("Megoldás mentése pontos sorrenddel"));
		toolBar->addWidget(cb);
		sliderCetliScale->setToolTip(tr("Cetlik méretezése"));
		toolBar->addWidget(sliderCetliScale);
	}
	else
	{
		toolBar->addAction(createToolbarAction("save.png", tr("&Save"), tr("Cetlik mentése"), SLOT(saveCetlies())));
	}
}

void KatitoNeni::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void KatitoNeni::onItemDoubleClicked(QListWidgetItem *item)
{
	actImagePath = item->data(Qt::UserRole).toString();
	cetliDock->cetliPath = actImagePath.replace(".jpg", "");
	loadCetlies();
	setImage(actImagePath);
}

void KatitoNeni::findEdges(const QPoint&p, int& top, int & bottom, int &left, int &right )
{
	QPoint sp = p/scaleFactor;
	int n = 3;//ui->spinBox->value();
	left   = sp.x()-walkToEdge(sp, -1, 0)-n;
	right  = sp.x()+walkToEdge(sp, 1, 0)+n;
	top    = sp.y()-walkToEdge(sp, 0, -1)-n;
	bottom = sp.y()+walkToEdge(sp, 0, 1)+n;

	QImage i = srcImage.copy(left, right, right-left, bottom - top);
	int w = i.width()*scale;
	int h = i.height()*scale;

	cetliDock->addCetli(srcImage.copy(left, top, right-left, bottom - top).scaled(w, h));
}

void KatitoNeni::onCetliScaleChanged(int v)
{
	scaleFactor = float(v)/200;
	refreshSrcImage();
}

void KatitoNeni::setPixel(const QPoint &p, bool update)
{
	int w = 10;
	int h = 10;
	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++)
			srcImage.setPixelColor(p.x()+x, p.y()+y, Qt::green);
	if(update){
		refreshSrcImage();
	}
}

int KatitoNeni::walkToEdge(const QPoint startPos, int xDir, int yDir)
{
	float edge = 2000;//ui->spinBoxEdge->value()*10;
	int d = 0;
	QRgb prev = srcImage.pixel(startPos);
	QPoint p = startPos;
	do{
		QRgb cur = srcImage.pixel(p);
		int dif = colorDiffPower(cur, prev);
		//if(p!=startPos)
			//setPixel(p);
		d++;
		p.setX(p.x()+xDir);
		p.setY(p.y()+yDir);
		if(dif > edge)
			break;
		prev = cur;
	}while(srcImage.rect().contains(p));
	return d;
}

int KatitoNeni::colorDiffPower(const QRgb &a, const QRgb &b)
{
	return pow(qRed(b)-qRed(a))+pow(qGreen(b)-qGreen(a))+pow(qBlue(b)-qBlue(a));

}

void KatitoNeni::shuffle()
{
	cetliDock->shuffle();
}

void KatitoNeni::alterSlicers()
{
	slicerState++;
	if(slicerState  > 3)
		slicerState = 0;
	setSlicers();
}

void  KatitoNeni::setSlicers()
{
	int s = (width()-200)/2;
	QList<int> iSizes;
	/*
	switch(slicerState){
		case 0:
			iSizes << 200 << s << s;
			cetliDock->setAreaWidth(s);
			break;
		case 1:
			iSizes << 200 << 0 << width()-200;
			cetliDock->setAreaWidth(width()-200);
			break;
		case 2:
			iSizes << 200 << width()-200 << 0;
			break;
		case 3:
			iSizes << 0 << 0 << width();
			cetliDock->setAreaWidth(width()-200);
			break;
	}
	*/

	iSizes << 200 << s << s << s;
	splitter->setSizes(iSizes);

}
void KatitoNeni::saveCetlies()
{
	cetliDock->save(true);
}

bool KatitoNeni::loadCetlies()
{
	QDir dir(cetliDock->cetliPath);
	if(dir.exists()){
		cetliDock->load();
		return true;
	}
	else
		cetliDock->clear();

	return false;
}

