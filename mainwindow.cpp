#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialize settings to default values
    noise_percent = 40;
    generations = 10;
    num_alive_to_live = 4;
    overpopulation = 9;

	//setup scene and set as scene for graphics view
	scene = new QGraphicsScene(0,0,MAPSIZE*GRIDSIZE, MAPSIZE*GRIDSIZE);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setBackgroundBrush(Qt::gray);
	ui->graphicsView->setScene(scene);

    //connect settings
    connect(ui->noiseSlider, SIGNAL(valueChanged(int)), this, SLOT(updateNoise(int)));
    connect(ui->generationSlider, SIGNAL(valueChanged(int)), this, SLOT(updateGenerations(int)));
    connect(ui->aliveSlider, SIGNAL(valueChanged(int)), this, SLOT(updateNumAliveThreshold(int)));
    connect(ui->overpopSlider, SIGNAL(valueChanged(int)), this, SLOT(updateOverpopulation(int)));

    connect(ui->generateBtn, &QPushButton::clicked, this, &MainWindow::generateNew);

	generateNoise();
    gameOfLife();
    drawTiles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateNoise()
{
	//loops over every grid square and assigns a 0 or 1 randomly
	int randomVal;
	for(int i = 0; i < MAPSIZE; i++)
	{
		for(int j = 0; j < MAPSIZE; j++)
		{
			randomVal = QRandomGenerator::global()->bounded(0,100); //rand between 0 and 99
            if(randomVal < noise_percent)
				tilemap[i][j] = 1;
			else
				tilemap[i][j] = 0;
		}
	}
}

void MainWindow::drawTiles()
{
	//update rectangle colors based on tilemap array
	for(int i = 0; i < MAPSIZE; i++)
	{
		for(int j = 0; j < MAPSIZE; j++)
		{
			if(tilemap[i][j] == 0)
			{
				QGraphicsRectItem* rect = scene->addRect(i*GRIDSIZE,j*GRIDSIZE,GRIDSIZE,GRIDSIZE);
                rect->setBrush(Qt::black);
				rect->setPen(QPen(Qt::transparent));
			}
            else
			{
				QGraphicsRectItem* rect = scene->addRect(i*GRIDSIZE,j*GRIDSIZE,GRIDSIZE,GRIDSIZE);
                rect->setBrush(Qt::white);
				rect->setPen(QPen(Qt::transparent));
			}
		}
	}
}

void MainWindow::gameOfLife()
{
	/*
	game of life -
	1. live cells with 2 or 3 neighbors survives
	2. dead cell with 3 live neightbours becomes alive
	3. all other live cells die and dead cells remain

    this is a simplified version for better generation
    over a threshhold alive
    below threshhold dead
	over a threshold dead - overpopulation
	*/
	int nextTileMap[MAPSIZE][MAPSIZE];
	int aliveNeighbors = 0;
    for(int k = 0; k < generations; k++)
	{
		for(int i = 0; i < MAPSIZE; i++)
		{
			for(int j = 0; j < MAPSIZE; j++)
			{
				//gets the number of alive neighbors
				//top left
				if((i != 0 && j != 0) && tilemap[i-1][j-1] == 1)
					aliveNeighbors++;
				//top middle
				if(j != 0 && tilemap[i][j-1] == 1)
					aliveNeighbors++;
				//top right
				if((i != MAPSIZE-1 && j != 0) && tilemap[i+1][j-1] == 1)
					aliveNeighbors++;
				//middle left
				if((i != 0) && tilemap[i-1][j] == 1)
					aliveNeighbors++;
				//middle right
				if((i != MAPSIZE-1) && tilemap[i+1][j] == 1)
					aliveNeighbors++;
				//bottom left
				if((i != 0 && j != MAPSIZE-1) && tilemap[i-1][j+1] == 1)
					aliveNeighbors++;
				//bottom middle
				if(j != MAPSIZE-1 && tilemap[i][j+1] == 1)
					aliveNeighbors++;
				//bottom right
				if((i != MAPSIZE-1 && j != MAPSIZE-1) && tilemap[i+1][j+1] == 1)
					aliveNeighbors++;

				//alive tiles are room tiles
                if(aliveNeighbors >= overpopulation)
                    nextTileMap[i][j] = 0;
                else if(aliveNeighbors >= num_alive_to_live)
                    nextTileMap[i][j] = 1;
                else
                    nextTileMap[i][j] = 0;
                //qDebug() << aliveNeighbors << " " << nextTileMap[i][j] << '\n';
				aliveNeighbors = 0;
            }
		}
		//assign new tilemap
        totalAlive = 0;
		for(int i = 0; i < MAPSIZE; i++)
		{
			for(int j = 0; j < MAPSIZE; j++)
            {
                tilemap[i][j] = nextTileMap[i][j];
            }
		}
	}
}

void MainWindow::updateNoise(int noise)
{
    noise_percent = noise;
    ui->noiseLabel->setNum(noise_percent);
}
void MainWindow::updateGenerations(int gen)
{
	generations = gen;
	ui->generationLabel->setNum(generations);
}
void MainWindow::updateNumAliveThreshold(int a)
{
	num_alive_to_live = a;
	ui->aliveLabel->setNum(num_alive_to_live);
}
void MainWindow::updateOverpopulation(int op)
{
	overpopulation = op;
    ui->overpopLabel->setNum(overpopulation);
}
void MainWindow::generateNew()
{
    generateNoise();
    gameOfLife();
    drawTiles();
}
