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
	updateCells();
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
			QGraphicsRectItem* rect = scene->addRect(i*GRIDSIZE,j*GRIDSIZE,GRIDSIZE,GRIDSIZE);
			switch(tilemap[i][j])
			{
				case 0:
					rect->setBrush(Qt::black);
					rect->setPen(QPen(Qt::transparent));
				break;
				case 1:
					rect->setBrush(Qt::white);
					rect->setPen(QPen(Qt::transparent));
				break;
				case 2:
					rect->setBrush(Qt::green);
					rect->setPen(QPen(Qt::transparent));
				break;
				case 3:
					rect->setBrush(Qt::red);
					rect->setPen(QPen(Qt::transparent));
				break;
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
	updateCells();
    drawTiles();
}

void MainWindow::updateCells()
{
	/*
	for this function:
	pick a start grid square
	set its value to 2 in tilemap to be drawn green later
	create a dijkstra map from the start point
	if the tile is unreachable after the map is created, set value to 3
	draw it red later
	*/
	
	//for now pick start as close to middle as possible
	int startX, startY;
	startX = startY = MAPSIZE/2;
	while(startX < MAPSIZE && tilemap[startX][startY] != 1)
	{
		startX++;
	}
	if(startX == MAPSIZE) //if this is common i can fix later
		return;
	tilemap[startX][startY] = 2;

	bool changesMade = true;
	//initialize dijkstra map as more than any possible path 
	for(int i = 0; i < MAPSIZE; i++)
	{
		for(int j = 0; j < MAPSIZE; j++)
		{
			dijkstraMap[i][j] = MAPSIZE * MAPSIZE;
		}
	}
	//set start square as 0 - origin of where we measure distance from
	dijkstraMap[startX][startY] = 0;

	//probably a better way to do this
	while(changesMade)
	{
		changesMade = false;
		for(int i = 0; i < MAPSIZE; i++)
		{
			for(int j = 0; j < MAPSIZE; j++)
			{
				if(tilemap[i][j] == 1 && dijkstraMap[i][j] > valueOfLowestNeighbor(i,j) + 1)
				{
					dijkstraMap[i][j] = valueOfLowestNeighbor(i,j) + 1;
					changesMade = true;
				}
			}
		}
	}

	
	for(int i = 0; i < MAPSIZE; i++)
	{
		for(int j = 0; j < MAPSIZE; j++)
		{
			if(tilemap[i][j] == 1 && dijkstraMap[i][j] == MAPSIZE*MAPSIZE)
			{
				tilemap[i][j] = 3;
			}
		}
	}

}

int MainWindow::valueOfLowestNeighbor(int x, int y)
{
	int values[] = {MAPSIZE*MAPSIZE, MAPSIZE*MAPSIZE, MAPSIZE*MAPSIZE, MAPSIZE*MAPSIZE};
	//top
	if(y != 0)
		values[0] = dijkstraMap[x][y-1];
	//bottom
	if(y != MAPSIZE-1)
		values[1] = dijkstraMap[x][y+1];
	//left
	if(x != 0)
		values[2] = dijkstraMap[x-1][y];
	//right
	if(x != MAPSIZE-1)
		values[3] = dijkstraMap[x+1][y];

	int min = MAPSIZE * MAPSIZE + 1; //will always be greater
for(int i = 0; i < 4; i++)
	{
		if(values[i] < min)
			min = values[i];
	}
	return min;
}
