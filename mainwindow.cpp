#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QMargins>
#include "pageslist.h"
#include <QSplitter>
#include <QSizePolicy>
#include <QUrl>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    ,ui(new Ui::MainWindow)
    ,imageLabel(new SrcWorkArea(this))
    ,scrollArea(new QScrollArea)
    ,settings("TBSoft", "KATITONENI")
    ,actImagePath("")
    ,cetliDock(0)
    ,scale(1)
    ,fileDialog(this, "Könyvtár liválasztása" )
{

    ui->setupUi(this);
    actImagePath = settings.value("actImagePath").toString();
    if(settings.contains("rect"))
        setGeometry(settings.value("rect").toRect());
    if(settings.contains("scale"))
        scale = settings.value("scale").toFloat();
	QRect r(60,60,1200,800);
	if(settings.contains("geometry"))
		 r = settings.value("geometry").toRect();


    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(onItemDoubleClicked(QListWidgetItem *)));
   // emit actImagePathChanged(actImagePath);

    connect(this, SIGNAL(pagesPathChanged(QString)), ui->dockWidget, SLOT(onPagesPathChanged(QString)), Qt::QueuedConnection);
    connect(ui->dockWidget,SIGNAL(startProgress(int)), this, SLOT(onStartProgress(int)));
    connect(ui->dockWidget,SIGNAL(changeProgress(int)), this, SLOT(onChangeProgress(int)));
    connect(&fileDialog, SIGNAL(accepted()), this, SLOT(onDirectorySelected()));
    QFileInfo fi(actImagePath);


    ui->dockWidget->pagesView = ui->listWidget;
    ui->verticalLayout->setGeometry(rect());

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(onRadiusChanged(int)));

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->main = this;


    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);

    setCentralWidget(scrollArea);

    //setImage(actImagePath);

    cetliDock = new DstWorkArea(this);
    addDockWidget(Qt::RightDockWidgetArea, cetliDock);
    cetliDock->setSizePolicy(QSizePolicy::MinimumExpanding,
                             QSizePolicy::MinimumExpanding);
    cetliDock->setMinimumWidth(500);
    if(settings.contains("radius")){
        int r = settings.value("radius").toInt();
        ui->spinBox->setValue(r);
    }

    ui->horizontalSlider->setValue(100*scale);
	setGeometry(r);
	repaint();
}

MainWindow::~MainWindow()
{
    settings.setValue("actImagePath", actImagePath);
    settings.setValue("rect", rect());
    settings.setValue("radius", ui->spinBox->value());
    settings.setValue("scale", scale);
	settings.setValue("geometry",geometry());
    delete ui;
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
	QFileInfo fi(actImagePath);
	QString dir = actImagePath.contains(".") ? fi.path() : actImagePath;
    QString file = item->text();
    actImagePath = QString("%1/%2").arg(dir).arg(file);
    setImage(actImagePath);
    emit actImagePathChanged(actImagePath);
}

void MainWindow::setImage(const QString &imagePath)
{
	QFile f(imagePath);
	if (f.open(QIODevice::ReadOnly) )
	{
		QByteArray ba = f.readAll();
		f.close();

		image.loadFromData(ba);
		int h = float(image.height())*float(ui->horizontalSlider->value())/100.0f;
		imageLabel->setPixmap(QPixmap::fromImage(image));
		scrollArea->setVisible(true);
		imageLabel->adjustSize();
	}
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    scale = float(ui->horizontalSlider->value())/100.0f;
//    setImage(actImagePath);
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
    int top, bottom, left, right;
    findEdges(event->pos(), top, bottom ,left, right);
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
}

void MainWindow::findEdges(const QPoint&p, int& top, int & bottom, int &left, int &right )
{
    int n = ui->spinBox->value();
    left   = p.x()-walkToEdge(p, -1, 0)-n;
    right  = p.x()+walkToEdge(p, 1, 0)+n;
    top    = p.y()-walkToEdge(p, 0, -1)-n;
    bottom = p.y()+walkToEdge(p, 0, 1)+n;
    //setPixel(p,true);

    QImage i = image.copy(left, right, right-left, bottom - top);
    int w = i.width()*scale;
    int h = i.height()*scale;

    cetliDock->dstView->addCetli(image.copy(left, top, right-left, bottom - top).scaled(w, h));
}

void MainWindow::setPixel(const QPoint &p, bool update)
{
    image.setPixelColor(p, Qt::green);
    if(update){
        int h = float(image.height())*float(ui->horizontalSlider->value())/100.0f;
        imageLabel->setPixmap(QPixmap::fromImage(image));
        repaint();
    }
}

float MainWindow::grayscale(const QRgb &rgb)
{
    float av = (qRed(rgb) + qGreen(rgb) + qBlue(rgb))/3;
    float sat =  qAbs(qRed(rgb)-av) * qAbs(qGreen(rgb)-av) * qAbs(qBlue(rgb)-av);
    return float(sat*av/1000);
}

float MainWindow::getGrayscalePixel(const QPoint& pos)
{
    int radius = ui->spinBox->value();
    float acc=0;
    int count = 0;
    for(int y = -radius; y <= radius; y++)
        for(int x = -radius; x <= radius; x++){
            QPoint p(pos.x()+x, pos.y()+y);
            if(image.rect().contains(p)){
              acc += grayscale(image.pixel(p));
              count ++;
              //setPixel(p);
            }
        }
  //  setPixel(pos, true);
    return acc/count;
}

int MainWindow::radius()
{
    return ui->spinBox->value();
}

void MainWindow::onRadiusChanged(int r)
{
}

int MainWindow::walkToEdge(const QPoint startPos, int xDir, int yDir)
{
    float edge = ui->spinBoxEdge->value()*10;
    float g;
    int d = 0;
    QRgb prev = image.pixel(startPos);
    QPoint p = startPos;
    do{
        QRgb cur = image.pixel(p);
        int dif = colorDiffPower(cur, prev);
     //   if(p!=startPos)
      //      setPixel(p);
        d++;
        p.setX(p.x()+xDir);
        p.setY(p.y()+yDir);
        if(dif > edge)
            break;
    }while(image.rect().contains(p));
    return d;
}

int MainWindow::colorDiffPower(const QRgb &a, const QRgb &b)
{
    return pow(qRed(b)-qRed(a))+pow(qGreen(b)-qGreen(a))+pow(qBlue(b)-qBlue(a));

}

void MainWindow::on_pushButtonSave_released()
{
    QString dir = actImagePath.replace(".jpg", "");
	cetliDock->dstView->save(dir, ui->checkBoxSolved->isChecked());
}

void MainWindow::on_pushButtonShuffle_released()
{
    cetliDock->dstView->shuffle();

}

void MainWindow::onChangeProgress(int value)
{
    ui->progressBar->setValue(value);
    repaint();
}

void MainWindow::onStartProgress(int max)
{
    ui->progressBar->setMaximum(max);
}

void MainWindow::onDirectorySelected()
{
    QUrl url = fileDialog.selectedUrls()[0];
    actImagePath = url.path();
	if(actImagePath[0] == '/')
		actImagePath = actImagePath.mid(1);
    fileDialog.close();
    emit pagesPathChanged(actImagePath);
}


void MainWindow::on_pushButtonOpen_clicked()
{
    QFileInfo fi(actImagePath);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    fileDialog.setDirectory(fi.path());
    fileDialog.open();
}

