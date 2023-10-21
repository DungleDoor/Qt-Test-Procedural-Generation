#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QDebug>

#define GRIDSIZE 10 //size of an individual grid square
#define MAPSIZE 100 //size of map square in grids
/*
Values that work best so far
noise percent 40
generations 10
threshold 4
*/
//#define NOISE_PERCENT 40 //initial percentage of walls to room tiles
//#define LIFE_GENERATIONS 10 //number of times life algorithm will be applied to noise
//#define LIFE_THRESHOLD 4 //number of live cells required to live


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

	int tilemap[MAPSIZE][MAPSIZE]; //stores map tiles as a 0 for wall and 1 for room
	void generateNoise();
	void drawTiles();
	void gameOfLife();
	
	QGraphicsScene* scene = nullptr;

    int noise_percent;
    int generations;
    int num_alive_to_live;
    int overpopulation;

private slots:
    void updateNoise(int);
    void updateGenerations(int);
    void updateNumAliveThreshold(int);
    void updateOverpopulation(int);
    void generateNew();
	void updateCells();
};
#endif // MAINWINDOW_H
