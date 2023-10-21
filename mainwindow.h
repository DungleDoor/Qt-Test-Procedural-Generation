#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QDebug>

#define GRIDSIZE 10 //size of an individual grid square
#define MAPSIZE 100 //size of map square in grids
#define MINMAPLENGTH 150
//max number of times a map will be attempted to reach min map length
//safety in case minmap length is too high
#define MAXMAPATTEMPTS 20
/*
Values that work best so far
noise percent 40
generations 10
threshold 4
minmaplength 150
*/

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
	int dijkstraMap[MAPSIZE][MAPSIZE];
	void generateNoise();
	void drawTiles();
	void gameOfLife();
	
	QGraphicsScene* scene = nullptr;

    int noise_percent;
    int generations;
    int num_alive_to_live;
    int overpopulation;
	int maxPath;
	QGraphicsRectItem* tiles[MAPSIZE][MAPSIZE];

signals:
	void mapUpdated();

private slots:
    void updateNoise(int);
    void updateGenerations(int);
    void updateNumAliveThreshold(int);
    void updateOverpopulation(int);
    void generateNew();
	void updateCells();
	int valueOfLowestNeighbor(int, int);
	void updateMapLength();
};
#endif // MAINWINDOW_H
