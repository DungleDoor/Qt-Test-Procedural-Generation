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

	//allocate tiles
	for(int i = 0; i < MAPSIZE; i++)
	{
		for(int j = 0; j < MAPSIZE; j++)
			tiles[i][j] = scene->addRect(i*GRIDSIZE,j*GRIDSIZE,GRIDSIZE,GRIDSIZE);
	}

    //connect settings
    connect(ui->noiseSlider, SIGNAL(valueChanged(int)), this, SLOT(updateNoise(int)));
    connect(ui->generationSlider, SIGNAL(valueChanged(int)), this, SLOT(updateGenerations(int)));
    connect(ui->aliveSlider, SIGNAL(valueChanged(int)), this, SLOT(updateNumAliveThreshold(int)));
    connect(ui->overpopSlider, SIGNAL(valueChanged(int)), this, SLOT(updateOverpopulation(int)));

    connect(ui->generateBtn, &QPushButton::clicked, this, &MainWindow::generateNew);

	connect(this, &MainWindow::mapUpdated, this, &MainWindow::updateMapLength);

	generateNew();
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
			switch(tilemap[i][j])
			{
				case 0:
					tiles[i][j]->setBrush(Qt::black);
					tiles[i][j]->setPen(QPen(Qt::transparent));
				break;
				case 1:
					tiles[i][j]->setBrush(Qt::white);
					tiles[i][j]->setPen(QPen(Qt::transparent));
				break;
				case 2:
					tiles[i][j]->setBrush(Qt::green);
					tiles[i][j]->setPen(QPen(Qt::transparent));
				break;
				case 3:
					tiles[i][j]->setBrush(Qt::red);
					tiles[i][j]->setPen(QPen(Qt::transparent));
				break;
				case 4:
					tiles[i][j]->setBrush(Qt::blue);
					tiles[i][j]->setPen(QPen(Qt::transparent));
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
    1. over a threshhold alive
    2. below threshhold dead
	3. over a threshold dead - overpopulation
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
	maxPath = 0;
	int numAttempts = 0;
	while(maxPath < MINMAPLENGTH && numAttempts < MAXMAPATTEMPTS)
	{
		generateNoise();
		gameOfLife();
		updateCells();
		numAttempts++;
	}
	drawTiles();
	emit mapUpdated();
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
	
	//pick start in lower left of map
	int startX, startY;
	startX = 0;
	startY = MAPSIZE-1;
	bool startFound = false;
	for(int i = 0; !startFound && i < MAPSIZE/4; i++)
	{
		for(int j = MAPSIZE-1; !startFound &&  j > 3*MAPSIZE/4; j--)
		{
			if(tilemap[i][j] == 1)
			{
				tilemap[i][j] = 2;
				startX = i;
				startY = j;
				startFound = true;
			}
		}
	}

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

	//find endpoint
	/*
	loops through map and finds the point furthest away from starting point
	sets that value in tilemap to 4 to be drawn blue later
	*/
	int endX, endY;
	for(int i = 0; i < MAPSIZE; i++)
	{
		for(int j = 0; j < MAPSIZE; j++)
		{
			if(tilemap[i][j] == 1 && dijkstraMap[i][j] == MAPSIZE*MAPSIZE)
			{
				tilemap[i][j] = 3;
			}
			else if(tilemap[i][j] == 1 && dijkstraMap[i][j] > maxPath)
			{
				maxPath = dijkstraMap[i][j];
				endX = i;
				endY = j;
			}
		}
	}
	tilemap[endX][endY] = 4;

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

void MainWindow::updateMapLength()
{
	QString num;
	num.setNum(maxPath);
	ui->distanceLabel->setText(QString("Distance to exit: ").append(num));
}
