#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>
#include <QScrollArea>
#include <QLabel>
#include <QSplitter>
#include "cetlidock.h"
#include "srcworkarea.h"
#include <QMouseEvent>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CetliDock *cetliDock;
    void setPixel(const QPoint &p, bool update = false);
    void findEdges(const QPoint&p, int& top, int & bottom, int &left, int &right );
    int radius();
    float scale;

private:
	//Ui::MainWindow *ui;
    QSettings settings;

    QImage image;
    QSplitter splitter;
    QString actImagePath;
    QScrollArea *scrollArea;
    SrcWorkArea *imageLabel;
    QFileDialog fileDialog;

    void setImage(const  QString &imagePath);
    float grayscale(const QRgb &rgb);
    float getGrayscalePixel(const QPoint& p);
    int walkToEdge(const QPoint startPos, int xDir, int yDir);
    int colorDiffPower(const QRgb &a, const QRgb &b);
    int pow(int a){
        return a*a;
    }

protected:

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);


public slots:
    void onRadiusChanged(int r);
    void onChangeProgress(int value);
    void onStartProgress(int max);
    void onDirectorySelected();
private slots:
    void onItemDoubleClicked(QListWidgetItem *item);

    void on_horizontalSlider_valueChanged(int value);

    void on_pushButtonSave_released();

    void on_pushButtonShuffle_released();

    void on_pushButtonOpen_clicked();

signals:
    void actImagePathChanged(QString path);
    void pagesPathChanged(QString path);
};

#endif // MAINWINDOW_H
