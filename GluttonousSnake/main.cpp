#include<graphics.h>
#include<conio.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<pthread.h>
#include"cJSON.h"

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "pthreadVC2.lib")

#define true 1
#define false 0
#define MAP_X 30
#define MAP_Y 30
#define DX 20
#define DY 20
#define SNAKE_INIT_LENGTH 5
#define KEY_LEFT_1 75
#define KEY_UP_1 72
#define KEY_RIGHT_1 77
#define KEY_DOWN_1 80
#define KEY_LEFT_2_0 'a'
#define KEY_UP_2_0 'w'
#define KEY_RIGHT_2_0 'd'
#define KEY_DOWN_2_0 's'
#define KEY_LEFT_2_1 'A'
#define KEY_UP_2_1 'W'
#define KEY_RIGHT_2_1 'D'
#define KEY_DOWN_2_1 'S'
#define SLEEP_TIME_SIMPLE 100
#define SLEEP_TIME_DIFFICULT 70
#define SLEEP_TIME_IMPOSSIBLE 50
#define NUM_OF_POI_WEEDS_SIMPLE 3
#define NUM_OF_POI_WEEDS_DIFFICULT 5
#define NUM_OF_POI_WEEDS_IMPOSSIBLE 10
#define NUM_OF_LANDMINE_SIMPLE 2
#define NUM_OF_LANDMINE_DIFFICULT 4
#define NUM_OF_LANDMINE_IMPOSSIBLE 8
#define NUM_OF_BRAIN_SIMPLE 3
#define NUM_OF_BRAIN_DIFFICULT 2
#define NUM_OF_BRAIN_IMPOSSIBLE 1
#define NUM_OF_FOODS_SIMPLE 5
#define NUM_OF_FOODS_DIFFICULT 3
#define NUM_OF_FOODS_IMPOSSIBLE 1
#define NUM_OF_BARRIER_SIMPLE 3
#define NUM_OF_BARRIER_DIFFICULT 5
#define NUM_OF_BARRIER_IMPOSSIBLE 10
#define MAX_NUM_OF_TONE_PER_BARRIER 8
#define MIN_NUM_OF_TONE_PER_BARRIER 2
#define SCORE_SIMPLE 300
#define SCORE_DIFFICULT 500
#define SCORE_IMPOSSIBLE 700
#define FLASH_FREQ 3
#define POI_WEEDS_TIME 5000
#define BRAIN_TIME 8000
#define KINDS_OF_FOODS 12
#define BRAIN_MAINTAIN_TIME 10000

#pragma warning(disable:4996)

typedef enum {
	single_Model,
	double_Model,
	computer_Model
}Model;
typedef enum {
	simple_Difficulty,
	difficult_Difficulty,
	impossible_Difficulty
}Difficulty;
typedef enum {
	startGame,
	modelSetting,
	difficultySetting,
	continueGame,
	exitGame,
	rankList,
	nothing
}Operation;
typedef enum {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	ERR
}Direction;
typedef enum {
	P1,
	P2,
	none
}Player;
typedef enum {
	NOTHING,
	BODY_1,
	HEAD_1,
	BODY_2,
	HEAD_2,
	FOOD,
	POI_WEED,
	LANDMINE,
	BRAIN,
	TONE,
	NO_ANYTHING
}MapValue;
typedef char Bool;
typedef struct point {
	int x;
	int y;
}Point;
typedef struct snake {
	Point point;
	Bool isHead = false;
	Bool isTail = false;
	struct snake *previous = NULL;
	struct snake *next = NULL;
}Snake;

int sleepTime = SLEEP_TIME_SIMPLE;
int numOfPoiWeeds = NUM_OF_POI_WEEDS_SIMPLE;
int numOfLandmine = NUM_OF_LANDMINE_SIMPLE;
int numOfBrain = NUM_OF_BRAIN_SIMPLE;
int numOfFood = NUM_OF_FOODS_SIMPLE;
int numOfBarrier = NUM_OF_BARRIER_SIMPLE;
int food_x[NUM_OF_FOODS_SIMPLE];
int food_y[NUM_OF_FOODS_SIMPLE];
int poiWeeds_x[NUM_OF_POI_WEEDS_IMPOSSIBLE];
int poiWeeds_y[NUM_OF_POI_WEEDS_IMPOSSIBLE];
int landmine_x[NUM_OF_LANDMINE_IMPOSSIBLE];
int landmine_y[NUM_OF_LANDMINE_IMPOSSIBLE];
int brain_x[NUM_OF_BRAIN_SIMPLE];
int brain_y[NUM_OF_BRAIN_SIMPLE];
int tone_x[NUM_OF_BARRIER_IMPOSSIBLE*MAX_NUM_OF_TONE_PER_BARRIER];
int tone_y[NUM_OF_BARRIER_IMPOSSIBLE*MAX_NUM_OF_TONE_PER_BARRIER];
int snakeLength_1 = SNAKE_INIT_LENGTH;
int snakeLength_2 = SNAKE_INIT_LENGTH;
int foodIndex[NUM_OF_FOODS_SIMPLE];
int scoreThresholdValue = SCORE_SIMPLE;
int top3Score[3] = { 0, 0, 0 };
int highestScore = 0;

MapValue map[MAP_Y][MAP_X];

Snake *head_1, *tail_1;
Snake *head_2, *tail_2;

Bool paintPoiWeeds = false;
Bool hasFood = false;
Bool hasPoiWeeds = false;
Bool hasLandmine = false;
Bool hasBrain = false;
Bool hasTone = false;
Bool keyUsed_1 = false;
Bool keyUsed_2 = false;
Bool doContinue = false;
Bool isOver = false;
Bool isWin;
Bool isPause = false;
Bool poi_weedOff = false;
Bool brainOff = false;
Bool autoMove_1 = false;
Bool autoMove_2 = false;
Bool isMainGUI = true;

IMAGE startBackground;
IMAGE readyImage;
IMAGE gameBackground;
IMAGE settingBackground;
IMAGE rankBackground;
IMAGE b_continueGame;
IMAGE b_difficultySetting;
IMAGE b_modelSetting;
IMAGE b_exit;
IMAGE b_rank;
IMAGE foods[12];
IMAGE poiWeed;
IMAGE landmine;
IMAGE brain;
IMAGE tone;
IMAGE snake_1[5][4];
IMAGE snake_2[5][4];
IMAGE snakeBrain[4];
IMAGE singleModel;
IMAGE doubleModel;
IMAGE computerModel;
IMAGE choose;
IMAGE settingBack;
IMAGE simpleModel;
IMAGE difficultModel;
IMAGE impossibleModel;
IMAGE singleLoss;
IMAGE singleWin;
IMAGE P1Win;
IMAGE P2Win;
IMAGE allWin;
IMAGE b_menu;
IMAGE b_pause;

Model currentModel = single_Model;

Difficulty currentDifficulty = simple_Difficulty;

Direction direction_1;
Direction direction_2;

pthread_t newThread_1;
pthread_t newThread_2;
pthread_t newThread_3;

void initMap();
void paintSnake(int, int, int, int, Player, Bool);
void paintMap(Bool, Bool);
void initSnake(int, Player);
void updateMap();
void move(Player);
Direction listenDirection(int, Player);
void createFood(int, int, int, int);
void paintCell(int, int, int, int, int x, int y);
void growUp(Player);
Bool eatFood(Player);
void createPoiWeeds(int, int, int, int);
void reduceCell(Player);
Bool eatPoiWeed(Player);
void createLandmine(int, int, int, int);
void reduceHalf(Player);
Bool eatLandmine(Player);
void createBrain(int, int, int, int);
Bool eatBrain(Player);
void createTone(int, int, int, int);
Bool eatTone(Player);
Bool outOfBoundary(Player);
void gameStart_single();
void gameStart_double();
void loadAllImages();
void loadAllMusic();
Operation startMouseListening();
IMAGE* getSnakeImage(Snake*, Player, Bool);
void paintModelSetting();
void paintDifficultySetting();
void paintRankList();
void paintSingleScore(int score);
void paintDoubleScore(int score_1, int score_2);
void writeFile();
Bool readFile();
void paintGameOver(Bool);
void paintWinner(Player);
void initParameter();
void paintButtons();
void doContinueGame(Bool);
void initSingleSnake(int);
void initDoubleSnake(int, int);
void getNextXY(Direction, Player, int*, int*);
void getCurrentXY(Player, int*, int*);
Direction autoGetDirection(Player, Model);
void setIsMainGUI(Bool);
void delay(int);
void *playEatFoodMusic(void*);
void *playEatPoiWeedMusic(void*);
void *playEatLandmineMusic(void*);
void *playEatBrainMusic(void*);
void *playBrainOverMusic(void*);
void initArr();
void readTop3();
void writeTop3();

int main() {
	srand((unsigned int)time(0));
	initgraph(800, 620);
	loadAllImages();
	loadAllMusic();
	Operation op;
start:
	mciSendString(_T("play menuMusic repeat"), NULL, 0, NULL);
	initArr();
	readTop3();
	putimage(0, 0, &startBackground);
	putimage(30, 560, &b_modelSetting);
	putimage(300, 560, &b_difficultySetting);
	putimage(570, 560, &b_continueGame);
	putimage(720, 15, &b_exit);
	putimage(20, 15, &b_rank);
	FlushMouseMsgBuffer();
	op = startMouseListening();
	mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
	switch (op) {
	case startGame:
		doContinue = false;
		goto startGame;
	case modelSetting:
		goto modelSetting;
	case difficultySetting:
		goto difficultySetting;
	case continueGame:
		doContinue = true;
		goto startGame;
	case exitGame:
		goto exit;
	case rankList:
		goto rankList;
	}
startGame:
	mciSendString(_T("stop menuMusic"), NULL, 0, NULL);
	setIsMainGUI(false);
	doContinueGame(doContinue);
	switch (currentModel)
	{
	case single_Model:
		gameStart_single();
		break;
	case double_Model:
		gameStart_double();
		break;
	case computer_Model:
		gameStart_double();
		break;
	}
	goto start;
modelSetting:
	paintModelSetting();
	goto start;
difficultySetting:
	paintDifficultySetting();
	goto start;
rankList:
	paintRankList();
	goto start;
exit:
	return 0;
}

void delay(int xms)
{
	int x, y;
	for (x = xms; x > 0; x--)
		for (y = 50000; y > 0; y--)
			setIsMainGUI(false);
	setIsMainGUI(true);
}

void setIsMainGUI(Bool flag) {
	isMainGUI = flag;
}

Direction autoGetDirection(Player player, Model model) {
	updateMap();
	Direction currentDirection = ERR;
	Direction directions[4] = { UP, RIGHT, DOWN, LEFT };
	int body;
	int head;
	int numOfDirection = 4;
	if (model == single_Model)
	{
		currentDirection = direction_1;
		body = BODY_1;
		head = -1;
	}
	else {
		switch (player)
		{
		case P1:
			currentDirection = direction_1;
			body = BODY_2;
			head = HEAD_2;
			break;
		case P2:
			currentDirection = direction_2;
			body = BODY_1;
			head = HEAD_1;
			break;
		}
	}
	int x, y;
	int x0, y0;
	for (int i = 0; i < 4; i++)
	{
		if (abs(directions[i]-currentDirection) == 2)
		{
			directions[i] = ERR;
			numOfDirection--;
			continue;
		}
		
		getNextXY(directions[i], player, &x, &y);
		if (x<0 || x>MAP_X - 1 || y<0 || y>MAP_Y - 1) {
			directions[i] = ERR;
			numOfDirection--;
			continue;
		}
		if (map[x][y] == body)
		{
			directions[i] = ERR;
			numOfDirection--;
			continue;
		}
		if (map[x][y] == head)
		{
			directions[i] = ERR;
			numOfDirection--;
			continue;
		}
		if (map[x][y] == TONE)
		{
			directions[i] = ERR;
			numOfDirection--;
			continue;
		}
	}

	if (numOfDirection <= 0)
	{
		return currentDirection;
	}
	if (numOfDirection == 1)
	{
		for (int i = 0; i < 4; i++) {
			if (directions[i] != ERR)
			{
				return directions[i];
			}
		}
	}

	getCurrentXY(player, &x0, &y0);
	int index;
	int minD = INT_MAX;
	int d;
	for (int i = 0; i < numOfFood; i++)
	{
		d = (int)(pow((food_x[i] - x0), 2) + pow((food_y[i] - y0), 2));
		if (d < minD)
		{
			minD = d;
			index = i;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (directions[i] == ERR) {
			continue;
		}
		getNextXY(directions[i], player, &x, &y);
		if (map[x][y] == LANDMINE)
		{
			directions[i] = ERR;
			numOfDirection--;
			if (numOfDirection == 1)
			{
				for (int i = 0; i < 4; i++) {
					if (directions[i] != ERR)
					{
						return directions[i];
					}
				}
			}
			continue;
		}
		if (map[x][y] == POI_WEED)
		{
			directions[i] = ERR;
			numOfDirection--;
			if (numOfDirection == 1)
			{
				for (int i = 0; i < 4; i++) {
					if (directions[i] != ERR)
					{
						return directions[i];
					}
				}
			}
			continue;
		}
		d = (int)(pow((food_x[index] - x), 2) + pow((food_y[index] - y), 2));
		if (d < minD)
		{
			return directions[i];
		}
	}
	int r = rand() % 4;
	while (directions[r] == ERR) {
		r = rand() % 4;
	}
	return directions[r];
}

void getNextXY(Direction direction, Player player, int*x, int*y) {
	Snake *head = NULL;
	switch (player)
	{
	case P1:
		head = head_1;
		break;
	case P2:
		head = head_2;
		break;
	}
	switch (direction)
	{
	case UP:
		*x = head->point.x;
		*y = head->point.y - 1;
		break;
	case RIGHT:
		*x = head->point.x + 1;
		*y = head->point.y;
		break;
	case DOWN:
		*x = head->point.x;
		*y = head->point.y + 1;
		break;
	case LEFT:
		*x = head->point.x - 1;
		*y = head->point.y;
		break;
	}
}

void getCurrentXY(Player player, int*x, int*y) {
	Snake *head = NULL;
	switch (player)
	{
	case P1:
		head = head_1;
		break;
	case P2:
		head = head_2;
		break;
	}
	*x = head->point.x;
	*y = head->point.y;
}

void initSingleSnake(int snakeLength_1) {
	initSnake(snakeLength_1, P1);
	Snake *temp = head_1;
	int i = 0;
	while (temp != NULL) {
		temp->point.x = MAP_X / 2 + i;
		temp->point.y = MAP_Y / 2;
		i++;
		temp = temp->next;
	}
}

void initDoubleSnake(int snakeLength_1, int snakeLength_2) {
	initSnake(snakeLength_1, P1);
	initSnake(snakeLength_1, P2);
	Snake *temp;
	int i;
	temp = head_1;
	i = 0;
	while (temp != NULL) {
		temp->point.x = MAP_X - 5;
		temp->point.y = MAP_Y - 10 + i;
		i++;
		temp = temp->next;
	}
	temp = head_2;
	i = 0;
	while (temp != NULL) {
		temp->point.x = 4;
		temp->point.y = MAP_Y - 10 + i;
		i++;
		temp = temp->next;
	}
}

void doContinueGame(Bool doContinue) {
	if (doContinue)
	{
		if (readFile()) {

		}
		else
		{
			initMap();
			initParameter();
		}
	}
	else
	{
		initMap();
		initParameter();
	}
}

void paintButtons() {
	putimage(635, 480, &b_pause);
	putimage(635, 550, &b_menu);
}

void initParameter() {
	paintPoiWeeds = false;
	hasFood = false;
	hasPoiWeeds = false;
	hasLandmine = false;
	hasTone = false;
	hasBrain = false;
	keyUsed_1 = false;
	keyUsed_2 = false;
	doContinue = false;
	isOver = false;
	snakeLength_1 = SNAKE_INIT_LENGTH;
	snakeLength_2 = SNAKE_INIT_LENGTH;
	isPause = false;
	poi_weedOff = false;
	brainOff = false;

	
	switch (currentDifficulty)
	{
	case simple_Difficulty:
		sleepTime = SLEEP_TIME_SIMPLE;
		numOfPoiWeeds = NUM_OF_POI_WEEDS_SIMPLE;
		numOfLandmine = NUM_OF_LANDMINE_SIMPLE;
		numOfBarrier = NUM_OF_BARRIER_SIMPLE;
		numOfFood = NUM_OF_FOODS_SIMPLE;
		break;
	case difficult_Difficulty:
		sleepTime = SLEEP_TIME_DIFFICULT;
		numOfPoiWeeds = NUM_OF_POI_WEEDS_DIFFICULT;
		numOfLandmine = NUM_OF_LANDMINE_DIFFICULT;
		numOfBarrier = NUM_OF_BARRIER_DIFFICULT;
		numOfFood = NUM_OF_FOODS_DIFFICULT;
		break;
	case impossible_Difficulty:
		sleepTime = SLEEP_TIME_IMPOSSIBLE;
		numOfPoiWeeds = NUM_OF_POI_WEEDS_IMPOSSIBLE;
		numOfLandmine = NUM_OF_LANDMINE_IMPOSSIBLE;
		numOfBarrier = NUM_OF_BARRIER_IMPOSSIBLE;
		numOfFood = NUM_OF_FOODS_IMPOSSIBLE;
		break;
	}
	switch (currentModel)
	{
	case single_Model:
		direction_1 = LEFT;
		autoMove_1 = false;
		initSingleSnake(SNAKE_INIT_LENGTH);
		break;
	case double_Model:
		direction_1 = UP;
		direction_2 = UP;
		autoMove_1 = false;
		autoMove_2 = false;
		initDoubleSnake(SNAKE_INIT_LENGTH, SNAKE_INIT_LENGTH);
		break;
	case computer_Model:
		direction_1 = UP;
		direction_2 = UP;
		autoMove_1 = false;
		autoMove_2 = true;
		initDoubleSnake(SNAKE_INIT_LENGTH, SNAKE_INIT_LENGTH);
		break;
	}
}

void paintGameOver(Bool isWin) {
	IMAGE *img;
	if (isWin)
	{
		img = &singleWin;
	}
	else {
		img = &singleLoss;
	}
	putimage(0, 200, img);
	//Sleep(5000);
	delay(5000);
}

void paintWinner(Player player) {
	IMAGE *img = NULL;
	switch (player)
	{
	case P1:
		img = &P1Win;
		break;
	case P2:
		img = &P2Win;
		break;
	case none:
		img = &allWin;
		break;
	}
	putimage(0, 200, img);
	//Sleep(5000);
	delay(5000);
}

void readTop3() {
	FILE *fp;
	if ((fp = fopen("memory.json", "r")) != NULL)
	{
		char fileStr[3000];
		char *sp = fileStr;
		char temp;
		temp = fgetc(fp);
		int i = 0;
		while (temp != EOF) {
			*sp = temp;
			sp++;
			temp = fgetc(fp);
			i++;
		}
		*sp = '\0';

		cJSON *memory = cJSON_Parse(fileStr);
		cJSON *rankList_arr = cJSON_GetObjectItem(memory, "rankList");
		if (rankList_arr == NULL)
		{
			return;
		}
		for (int i = 0; i < 3; i++)
		{
			top3Score[i] = atoi(cJSON_Print(cJSON_GetArrayItem(rankList_arr, i)));
		}

		cJSON_Delete(memory);
	}
}

void writeTop3() {
	FILE *fp;
	if ((fp = fopen("memory.json", "r")) != NULL)
	{
		char fileStr[3000];
		char *sp = fileStr;
		char temp;
		temp = fgetc(fp);
		int i = 0;
		while (temp != EOF) {
			*sp = temp;
			sp++;
			temp = fgetc(fp);
			i++;
		}
		*sp = '\0';

		cJSON *memory = cJSON_Parse(fileStr);
		cJSON *rankList_arr = cJSON_CreateArray();

		cJSON_AddItemToObject(memory, "rankList", rankList_arr);

		int t;
		for (int i = 0; i < 3; i++)
		{
			if (highestScore > top3Score[i]) {
				t = top3Score[i];
				top3Score[i] = highestScore;
				highestScore = t;
			}
			else if (highestScore == top3Score[i])
			{
				break;
			}
		}

		for (int i = 0; i < 3; i++)
		{
			cJSON_AddItemToArray(rankList_arr, cJSON_CreateNumber(top3Score[i]));
		}
		highestScore = 0;

		fp = fopen("memory.json", "w");
		fputs(cJSON_Print(memory), fp);
		fclose(fp);

		cJSON_Delete(memory);
	}
}

Bool readFile() {
	FILE *fp;
	if ((fp=fopen("memory.json", "r")) != NULL)
	{
		char fileStr[3000];
		char *sp = fileStr;
		char temp;
		temp = fgetc(fp);
		int i = 0;
		while (temp != EOF) {
			*sp = temp;
			sp++;
			temp = fgetc(fp);
			i++;
		}
		*sp = '\0';

		if (i < 10)
		{
			return false;
		}

		cJSON *memory = cJSON_Parse(fileStr);
		cJSON *snakes = cJSON_GetObjectItem(memory, "snakes");
		cJSON *snake_1 = cJSON_GetObjectItem(snakes, "snake_1");
		cJSON *snake_2 = cJSON_GetObjectItem(snakes, "snake_2");
		cJSON *snake_1_x_arr = cJSON_GetObjectItem(snake_1, "snake_1_x");
		cJSON *snake_2_x_arr = cJSON_GetObjectItem(snake_2, "snake_2_x");
		cJSON *snake_1_y_arr = cJSON_GetObjectItem(snake_1, "snake_1_y");
		cJSON *snake_2_y_arr = cJSON_GetObjectItem(snake_2, "snake_2_y");
		cJSON *food = cJSON_GetObjectItem(memory, "food");
		cJSON *poi_weed = cJSON_GetObjectItem(memory, "poi_weed");
		cJSON *landmine = cJSON_GetObjectItem(memory, "landmine");
		cJSON *tone = cJSON_GetObjectItem(memory, "tone");
		cJSON *brain = cJSON_GetObjectItem(memory, "brain");
		cJSON *food_x_arr = cJSON_GetObjectItem(food, "food_x");
		cJSON *food_y_arr = cJSON_GetObjectItem(food, "food_y");
		cJSON *foodIndex_arr = cJSON_GetObjectItem(food, "foodIndex");
		cJSON *poi_weed_x_arr = cJSON_GetObjectItem(poi_weed, "poi_weed_x");
		cJSON *poi_weed_y_arr = cJSON_GetObjectItem(poi_weed, "poi_weed_y");
		cJSON *landmine_x_arr = cJSON_GetObjectItem(landmine, "landmine_x");
		cJSON *landmine_y_arr = cJSON_GetObjectItem(landmine, "landmine_y");
		cJSON *tone_x_arr = cJSON_GetObjectItem(tone, "tone_x");
		cJSON *tone_y_arr = cJSON_GetObjectItem(tone, "tone_y");
		cJSON *brain_x_arr = cJSON_GetObjectItem(brain, "brain_x");
		cJSON *brain_y_arr = cJSON_GetObjectItem(brain, "brain_y");
		
		Model model = (Model)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "model")));
		Difficulty difficulty = (Difficulty)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "difficulty")));
		
		direction_1 = (Direction)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "direction_1")));
		poi_weedOff = (Bool)atoi(cJSON_Print(cJSON_GetObjectItem(poi_weed, "poi_weedOff")));
		brainOff = (Bool)atoi(cJSON_Print(cJSON_GetObjectItem(brain, "brainOff")));

		int temp_x, temp_y;
		int snakeSize_1 = cJSON_GetArraySize(snake_1_x_arr);
		
		initSnake(snakeSize_1, P1);

		Snake *snakeTemp;
		snakeTemp = head_1;
		for (int i = 0; i < snakeSize_1; i++)
		{
			temp_x = atoi(cJSON_Print(cJSON_GetArrayItem(snake_1_x_arr, i)));
			temp_y = atoi(cJSON_Print(cJSON_GetArrayItem(snake_1_y_arr, i)));
			snakeTemp->point.x = temp_x;
			snakeTemp->point.y = temp_y;
			snakeTemp = snakeTemp->next;
		}

		if (model == double_Model || model == computer_Model)
		{
			int snakeSize_2 = cJSON_GetArraySize(snake_2_x_arr);
			initSnake(snakeSize_2, P2);
			direction_2 = (Direction)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "direction_2")));
			snakeTemp = head_2;
			for (int i = 0; i < snakeSize_2; i++)
			{
				temp_x = atoi(cJSON_Print(cJSON_GetArrayItem(snake_2_x_arr, i)));
				temp_y = atoi(cJSON_Print(cJSON_GetArrayItem(snake_2_y_arr, i)));
				snakeTemp->point.x = temp_x;
				snakeTemp->point.y = temp_y;
				snakeTemp = snakeTemp->next;
			}
			snakeLength_2 = snakeSize_2;
		}

		if (model == double_Model)
		{
			autoMove_1 = false;
			autoMove_2 = false;
		}
		else if (model == single_Model)
		{
			autoMove_1 = false;
		}
		else if (model == computer_Model)
		{
			autoMove_1 = false;
			autoMove_2 = true;
		}

		int foodSize = cJSON_GetArraySize(food_x_arr);
		for (int i = 0; i < NUM_OF_FOODS_SIMPLE; i++)
		{
			if (i < foodSize)
			{
				food_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(food_x_arr, i)));
				food_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(food_y_arr, i)));
				foodIndex[i] = atoi(cJSON_Print(cJSON_GetArrayItem(foodIndex_arr, i)));
			}
			else {
				food_x[i] = -2;
				food_y[i] = -2;
			}	
		}

		int poi_weedSize = cJSON_GetArraySize(poi_weed_x_arr);
		for (int i = 0; i < NUM_OF_POI_WEEDS_IMPOSSIBLE; i++)
		{
			if (i < poi_weedSize) {
				poiWeeds_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(poi_weed_x_arr, i)));
				poiWeeds_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(poi_weed_y_arr, i)));
			}
			else {
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
			}
		}

		int landmineSize = cJSON_GetArraySize(landmine_x_arr);
		for (int i = 0; i < NUM_OF_LANDMINE_IMPOSSIBLE; i++)
		{
			if (i < landmineSize) {
				landmine_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(landmine_x_arr, i)));
				landmine_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(landmine_y_arr, i)));
			}
			else {
				landmine_x[i] = -2;
				landmine_y[i] = -2;
			}
		}

		int toneSize = cJSON_GetArraySize(tone_x_arr);
		for (int i = 0; i < NUM_OF_LANDMINE_IMPOSSIBLE*MAX_NUM_OF_TONE_PER_BARRIER; i++)
		{
			if (i < toneSize) {
				tone_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(tone_x_arr, i)));
				tone_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(tone_y_arr, i)));
			}
			else {
				tone_x[i] = -2;
				tone_y[i] = -2;
			}
		}

		int brainSize = cJSON_GetArraySize(brain_x_arr);
		for (int i = 0; i < NUM_OF_BRAIN_SIMPLE; i++)
		{
			if (i < brainSize) {
				brain_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(brain_x_arr, i)));
				brain_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(brain_y_arr, i)));
			}
			else {
				brain_x[i] = -2;
				brain_y[i] = -2;
			}
		}

		paintPoiWeeds = true;
		hasFood = true;
		hasPoiWeeds = true;
		hasLandmine = true;
		hasTone = true;
		hasBrain = true;
		isPause = false;

		currentModel = model;
		currentDifficulty = difficulty;
		snakeLength_1 = snakeSize_1;

		switch (currentDifficulty)
		{
		case simple_Difficulty:
			sleepTime = SLEEP_TIME_SIMPLE;
			numOfPoiWeeds = NUM_OF_POI_WEEDS_SIMPLE;
			numOfLandmine = NUM_OF_LANDMINE_SIMPLE;
			numOfBarrier = NUM_OF_BARRIER_SIMPLE;
			numOfFood = NUM_OF_FOODS_SIMPLE;
			scoreThresholdValue = SCORE_SIMPLE;
			break;
		case difficult_Difficulty:
			sleepTime = SLEEP_TIME_DIFFICULT;
			numOfPoiWeeds = NUM_OF_POI_WEEDS_DIFFICULT;
			numOfLandmine = NUM_OF_LANDMINE_DIFFICULT;
			numOfBarrier = NUM_OF_BARRIER_DIFFICULT;
			numOfFood = NUM_OF_FOODS_DIFFICULT;
			scoreThresholdValue = SCORE_DIFFICULT;
			break;
		case impossible_Difficulty:
			sleepTime = SLEEP_TIME_IMPOSSIBLE;
			numOfPoiWeeds = NUM_OF_POI_WEEDS_IMPOSSIBLE;
			numOfLandmine = NUM_OF_LANDMINE_IMPOSSIBLE;
			numOfBarrier = NUM_OF_BARRIER_IMPOSSIBLE;
			numOfFood = NUM_OF_FOODS_IMPOSSIBLE;
			scoreThresholdValue = SCORE_IMPOSSIBLE;
			break;
		}

		if (memory == NULL)
		{
			return false;
		}
		cJSON_Delete(memory);
		return true;
	}
	else
	{
		return false;
	}
}

void writeFile() {
	FILE *fp;
	cJSON *memory = cJSON_CreateObject();
	cJSON *snakes = cJSON_CreateObject();
	cJSON *snake_1 = cJSON_CreateObject();
	cJSON *snake_2 = cJSON_CreateObject();
	cJSON *snake_1_x_arr = cJSON_CreateArray();
	cJSON *snake_2_x_arr = cJSON_CreateArray();
	cJSON *snake_1_y_arr = cJSON_CreateArray();
	cJSON *snake_2_y_arr = cJSON_CreateArray();
	cJSON *food = cJSON_CreateObject();
	cJSON *poi_weed = cJSON_CreateObject();
	cJSON *landmine = cJSON_CreateObject();
	cJSON *tone = cJSON_CreateObject();
	cJSON *brain = cJSON_CreateObject();
	cJSON *food_x_arr = cJSON_CreateArray();
	cJSON *food_y_arr = cJSON_CreateArray();
	cJSON *foodIndex_arr = cJSON_CreateArray();
	cJSON *poi_weed_x_arr = cJSON_CreateArray();
	cJSON *poi_weed_y_arr = cJSON_CreateArray();
	cJSON *landmine_x_arr = cJSON_CreateArray();
	cJSON *landmine_y_arr = cJSON_CreateArray();
	cJSON *tone_x_arr = cJSON_CreateArray();
	cJSON *tone_y_arr = cJSON_CreateArray();
	cJSON *brain_x_arr = cJSON_CreateArray();
	cJSON *brain_y_arr = cJSON_CreateArray();

	cJSON_AddNumberToObject(memory, "model", currentModel);
	cJSON_AddNumberToObject(memory, "difficulty", currentDifficulty);
	cJSON_AddNumberToObject(poi_weed, "poi_weedOff", poi_weedOff);
	cJSON_AddNumberToObject(brain, "brainOff", brainOff);
	cJSON_AddNumberToObject(memory, "direction_1", direction_1);
	cJSON_AddNumberToObject(memory, "direction_2", direction_2);
	cJSON_AddItemToObject(memory, "snakes", snakes);
	cJSON_AddItemToObject(snakes, "snake_1", snake_1);
	cJSON_AddItemToObject(snakes, "snake_2", snake_2);
	cJSON_AddItemToObject(snake_1, "snake_1_x", snake_1_x_arr);
	cJSON_AddItemToObject(snake_1, "snake_1_y", snake_1_y_arr);
	cJSON_AddItemToObject(snake_2, "snake_2_x", snake_2_x_arr);
	cJSON_AddItemToObject(snake_2, "snake_2_y", snake_2_y_arr);
	cJSON_AddItemToObject(memory, "food", food);
	cJSON_AddItemToObject(food, "food_x", food_x_arr);
	cJSON_AddItemToObject(food, "food_y", food_y_arr);
	cJSON_AddItemToObject(food, "foodIndex", foodIndex_arr);
	cJSON_AddItemToObject(memory, "poi_weed", poi_weed);
	cJSON_AddItemToObject(poi_weed, "poi_weed_x", poi_weed_x_arr);
	cJSON_AddItemToObject(poi_weed, "poi_weed_y", poi_weed_y_arr);
	cJSON_AddItemToObject(memory, "landmine", landmine);
	cJSON_AddItemToObject(landmine, "landmine_x", landmine_x_arr);
	cJSON_AddItemToObject(landmine, "landmine_y", landmine_y_arr);
	cJSON_AddItemToObject(memory, "tone", tone);
	cJSON_AddItemToObject(tone, "tone_x", tone_x_arr);
	cJSON_AddItemToObject(tone, "tone_y", tone_y_arr);
	cJSON_AddItemToObject(memory, "brain", brain);
	cJSON_AddItemToObject(brain, "brain_x", brain_x_arr);
	cJSON_AddItemToObject(brain, "brain_y", brain_y_arr);

	Snake *temp;
	temp = head_1;
	while (temp != NULL) {
		cJSON_AddItemToArray(snake_1_x_arr, cJSON_CreateNumber(temp->point.x));
		cJSON_AddItemToArray(snake_1_y_arr, cJSON_CreateNumber(temp->point.y));
		temp = temp->next;
	}

	temp = head_2;
	while (temp != NULL) {
		cJSON_AddItemToArray(snake_2_x_arr, cJSON_CreateNumber(temp->point.x));
		cJSON_AddItemToArray(snake_2_y_arr, cJSON_CreateNumber(temp->point.y));
		temp = temp->next;
	}

	for (int i = 0; i < numOfFood; i++)
	{
		cJSON_AddItemToArray(food_x_arr, cJSON_CreateNumber(food_x[i]));
		cJSON_AddItemToArray(food_y_arr, cJSON_CreateNumber(food_y[i]));
		cJSON_AddItemToArray(foodIndex_arr, cJSON_CreateNumber(foodIndex[i]));
	}

	for (int i = 0; i < numOfPoiWeeds; i++)
	{
		cJSON_AddItemToArray(poi_weed_x_arr, cJSON_CreateNumber(poiWeeds_x[i]));
		cJSON_AddItemToArray(poi_weed_y_arr, cJSON_CreateNumber(poiWeeds_y[i]));
	}

	for (int i = 0; i < numOfLandmine; i++)
	{
		cJSON_AddItemToArray(landmine_x_arr, cJSON_CreateNumber(landmine_x[i]));
		cJSON_AddItemToArray(landmine_y_arr, cJSON_CreateNumber(landmine_y[i]));
	}

	for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
	{
		cJSON_AddItemToArray(tone_x_arr, cJSON_CreateNumber(tone_x[i]));
		cJSON_AddItemToArray(tone_y_arr, cJSON_CreateNumber(tone_y[i]));
	}

	for (int i = 0; i < numOfBrain; i++)
	{
		cJSON_AddItemToArray(brain_x_arr, cJSON_CreateNumber(brain_x[i]));
		cJSON_AddItemToArray(brain_y_arr, cJSON_CreateNumber(brain_y[i]));
	}

	fp = fopen("memory.json", "w");
	fputs(cJSON_Print(memory), fp);
	fclose(fp);

	cJSON_Delete(memory);
}

void paintSingleScore(int score) {
	char scoreStr[5];
	char destinationStr[5];
	itoa(scoreThresholdValue, destinationStr, 10);
	itoa(score, scoreStr, 10);
	setbkmode(TRANSPARENT);
	settextcolor(BLUE);
	settextstyle(25, 0, "黑体");
	outtextxy(660, 10, "目标：");
	outtextxy(730, 10, destinationStr);
	settextcolor(BROWN);
	settextstyle(60, 0, "隶书");
	outtextxy(650, 50, "得分");
	settextcolor(RED);
	settextstyle(40, 0, "幼圆");
	outtextxy(690, 130, scoreStr);
}

void paintDoubleScore(int score_1, int score_2) {
	char scoreStr_1[5];
	char scoreStr_2[5];
	itoa(score_1, scoreStr_1, 10);
	itoa(score_2, scoreStr_2, 10);
	setbkmode(TRANSPARENT);
	settextcolor(BROWN);
	settextstyle(40, 0, "隶书");
	outtextxy(630, 50, "红方得分");
	outtextxy(630, 250, "绿方得分");
	settextcolor(RED);
	settextstyle(40, 0, "幼圆");
	outtextxy(670, 100, scoreStr_1);
	outtextxy(670, 300, scoreStr_2);
}

IMAGE* getSnakeImage(Snake* snakeBody, Player player, Bool hasBrain) {
	IMAGE(*snakeArr)[4] = NULL;
	switch (player)
	{
		case P1:
			snakeArr = snake_1;
			break;
		case P2:
			snakeArr = snake_2;
			break;
	}
	if (snakeLength_1 == 1)
	{
		switch (direction_1) {
		case UP:
			if (hasBrain)
			{
				return &snakeBrain[0];
			}
			return &snakeArr[0][0];
		case RIGHT:
			if (hasBrain)
			{
				return &snakeBrain[1];
			}
			return &snakeArr[0][1];
		case DOWN:
			if (hasBrain)
			{
				return &snakeBrain[2];
			}
			return &snakeArr[0][2];
		case LEFT:
			if (hasBrain)
			{
				return &snakeBrain[3];
			}
			return &snakeArr[0][3];
		}
	}

	int px, py;
	int x, y;
	int nx, ny;
	if (snakeBody == head_1 || snakeBody == head_2)
	{
		x = snakeBody->point.x;
		y = snakeBody->point.y;

		nx = snakeBody->next->point.x;
		ny = snakeBody->next->point.y;
		if (hasBrain)
		{
			if (x == nx && y == ny - 1)
			{
				return &snakeBrain[0];
			}
			else if (x == nx + 1 && y == ny)
			{
				return &snakeBrain[1];
			}
			else if (x == nx && y == ny + 1)
			{
				return &snakeBrain[2];
			}
			else if (x == nx - 1 && y == ny)
			{
				return &snakeBrain[3];
			}
			return NULL;
		}

		if (x == nx && y == ny - 1)
		{
			return &snakeArr[0][0];
		}
		else if (x == nx + 1 && y == ny)
		{
			return &snakeArr[0][1];
		}
		else if (x == nx && y == ny + 1)
		{
			return &snakeArr[0][2];
		}
		else if (x == nx - 1 && y == ny)
		{
			return &snakeArr[0][3];
		}
		return NULL;
	}
	else if (snakeBody == tail_1 || snakeBody == tail_2)
	{
		x = snakeBody->point.x;
		y = snakeBody->point.y;

		px = snakeBody->previous->point.x;
		py = snakeBody->previous->point.y;

		if (px == x && py == y - 1)
		{
			return &snakeArr[4][0];
		}
		else if (px == x + 1 && py == y)
		{
			return &snakeArr[4][1];
		}
		else if (px == x && py == y + 1)
		{
			return &snakeArr[4][2];
		}
		else if (px == x - 1 && py == y)
		{
			return &snakeArr[4][3];
		}
	}
	else
	{
		x = snakeBody->point.x;
		y = snakeBody->point.y;

		px = snakeBody->previous->point.x;
		py = snakeBody->previous->point.y;

		nx = snakeBody->next->point.x;
		ny = snakeBody->next->point.y;

		if (px == nx && py == ny - 2)
		{
			return &snakeArr[1][0];
		}
		else if (px == nx + 2 && py == ny)
		{
			return &snakeArr[1][1];
		}
		else if (px == nx && py == ny + 2)
		{
			return &snakeArr[1][2];
		}
		else if (px == nx - 2 && py == ny)
		{
			return &snakeArr[1][3];
		}
		else if (px == nx - 1 && py == ny + 1 && x == nx)
		{
			return &snakeArr[2][0];
		}
		else if (px == nx - 1 && py == ny - 1 && x == px)
		{
			return &snakeArr[2][1];
		}
		else if (px == nx + 1 && py == ny - 1 && x == nx)
		{
			return &snakeArr[2][2];
		}
		else if (px == nx + 1 && py == ny + 1 && x == px)
		{
			return &snakeArr[2][3];
		}
		else if (px == nx + 1 && py == ny + 1 && x == nx)
		{
			return &snakeArr[3][0];
		}
		else if (px == nx - 1 && py == ny + 1 && x == px)
		{
			return &snakeArr[3][1];
		}
		else if (px == nx - 1 && py == ny - 1 && x == nx)
		{
			return &snakeArr[3][2];
		}
		else if (px == nx + 1 && py == ny - 1 && x == px)
		{
			return &snakeArr[3][3];
		}
	}
	return NULL;
}

Operation startMouseListening() {
	MOUSEMSG msg;
	int x, y;
	while (true) {
		if (MouseHit() && isMainGUI)
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 315 && x < 490 && y>415 && y < 470)
				{
					return startGame;
				}
				else if (x > 30 && x < 230 && y > 560 && y < 600)
				{
					//model setting
					return modelSetting;
				}
				else if (x > 300 && x < 500 && y > 560 && y < 600)
				{
					//difficulty setting
					return difficultySetting;
				}
				else if (x > 570 && x < 770 && y > 560 && y < 600) {
					//continue game
					return continueGame;
				}
				else if (x > 720 && x < 780 && y > 15 && y < 75)
				{
					//exit game
					return exitGame;
				}
				else if (x > 20 && x < 80 && y > 15 && y < 75)
				{
					//ranking list
					return rankList;
				}
			}
		}
	}
}

void paintDifficultySetting() {
	int choose_y;
	switch (currentDifficulty)
	{
	case simple_Difficulty:
		choose_y = 120;
		break;
	case difficult_Difficulty:
		choose_y = 220;
		break;
	case impossible_Difficulty:
		choose_y = 320;
		break;
	}
startPaint:
	putimage(0, 0, &settingBackground);
	putimage(300, 130, &simpleModel);
	putimage(300, 230, &difficultModel);
	putimage(300, 330, &impossibleModel);
	putimage(300, 430, &settingBack);
	putimage(520, choose_y, &choose);
	MOUSEMSG msg;
	int x, y;
	while (true) {
		if (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 300 && x < 500 && y>130 && y < 190)
				{
					//simple model
					choose_y = 120;
					sleepTime = SLEEP_TIME_SIMPLE;
					numOfPoiWeeds = NUM_OF_POI_WEEDS_SIMPLE;
					numOfLandmine = NUM_OF_LANDMINE_SIMPLE;
					numOfBarrier = NUM_OF_BARRIER_SIMPLE;
					numOfBrain = NUM_OF_BRAIN_SIMPLE;
					numOfFood = NUM_OF_FOODS_SIMPLE;
					currentDifficulty = simple_Difficulty;
					scoreThresholdValue = SCORE_SIMPLE;
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 230 && y < 290)
				{
					//difficult model
					choose_y = 220;
					sleepTime = SLEEP_TIME_DIFFICULT;
					numOfPoiWeeds = NUM_OF_POI_WEEDS_DIFFICULT;
					numOfLandmine = NUM_OF_LANDMINE_DIFFICULT;
					numOfBarrier = NUM_OF_BARRIER_DIFFICULT;
					numOfBrain = NUM_OF_BRAIN_DIFFICULT;
					numOfFood = NUM_OF_FOODS_DIFFICULT;
					currentDifficulty = difficult_Difficulty;
					scoreThresholdValue = SCORE_DIFFICULT;
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 330 && y < 390)
				{
					//impossible model
					choose_y = 320;
					sleepTime = SLEEP_TIME_IMPOSSIBLE;
					numOfPoiWeeds = NUM_OF_POI_WEEDS_IMPOSSIBLE;
					numOfLandmine = NUM_OF_LANDMINE_IMPOSSIBLE;
					numOfBarrier = NUM_OF_BARRIER_IMPOSSIBLE;
					numOfBrain = NUM_OF_BRAIN_IMPOSSIBLE;
					numOfFood = NUM_OF_FOODS_IMPOSSIBLE;
					currentDifficulty = impossible_Difficulty;
					scoreThresholdValue = SCORE_IMPOSSIBLE;
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 430 && y < 490)
				{
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto done;
				}
			}
		}
	}
done:;
}

void paintRankList() {
	putimage(0, 0, &rankBackground);
	putimage(300, 460, &settingBack);
	int singleWidth;
	char scoreStr[5];
	int top_1 = top3Score[0];
	int top_2 = top3Score[1];
	int top_3 = top3Score[2];
	setbkmode(TRANSPARENT);
	settextcolor(RED);
	settextstyle(30, 0, "幼圆");
	itoa(top_1, scoreStr, 10);
	singleWidth = 7 * (strlen(scoreStr) - 1);
	outtextxy(399 - singleWidth, 207, scoreStr);

	itoa(top_2, scoreStr, 10);
	singleWidth = 7 * (strlen(scoreStr) - 1);
	outtextxy(283 - singleWidth, 240, scoreStr);

	itoa(top_3, scoreStr, 10);
	singleWidth = 7 * (strlen(scoreStr) - 1);
	outtextxy(513 - singleWidth, 270, scoreStr);
	int x, y;
	MOUSEMSG msg;
	while (true)
	{
		if (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 300 && x < 500 && y>460 && y < 520)
				{
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto done;
				}
			}
		}
	}
done:;
}

void paintModelSetting() {
	int choose_y;
	switch (currentModel)
	{
	case single_Model:
		choose_y = 120;
		break;
	case double_Model:
		choose_y = 220;
		break;
	case computer_Model:
		choose_y = 320;
		break;
	}
startPaint:
	putimage(0, 0, &settingBackground);
	putimage(300, 130, &singleModel);
	putimage(300, 230, &doubleModel);
	putimage(300, 330, &computerModel);
	putimage(300, 430, &settingBack);
	putimage(520, choose_y, &choose);
	MOUSEMSG msg;
	int x, y;
	while (true) {
		if (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 300 && x < 500 && y>130 && y < 190)
				{
					//single model
					choose_y = 120;
					currentModel = single_Model;
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 230 && y < 290)
				{
					//double model
					choose_y = 220;
					currentModel = double_Model;
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 330 && y < 390)
				{
					//computer model
					choose_y = 320;
					currentModel = computer_Model;
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 430 && y < 490)
				{
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto done;
				}
			}
		}
	}
done:;
}

void loadAllImages() {
	loadimage(&startBackground, "images/background/background.jpg");
	loadimage(&readyImage, "images/background/ready.png");
	loadimage(&gameBackground, "images/background/gameBackground.png");
	loadimage(&settingBackground, "images/background/settingBackground.png");
	loadimage(&rankBackground, "images/background/rankBackground.png");
	loadimage(&b_continueGame, "images/buttons/b_continueGame.png", 200, 40);
	loadimage(&b_difficultySetting, "images/buttons/b_difficultySetting.png", 200, 40);
	loadimage(&b_modelSetting, "images/buttons/b_modelSetting.png", 200, 40);
	loadimage(&b_exit, "images/buttons/b_exit.png", 60, 60);
	loadimage(&b_rank, "images/buttons/b_rank.png", 60, 60);
	loadimage(&settingBack, "images/buttons/b_settingBack.png", 200, 60);
	loadimage(&singleModel, "images/buttons/b_single.png", 200, 60);
	loadimage(&doubleModel, "images/buttons/b_double.png", 200, 60);
	loadimage(&computerModel, "images/buttons/b_computer.png", 200, 60);
	loadimage(&simpleModel, "images/buttons/b_simple.png", 200, 60);
	loadimage(&difficultModel, "images/buttons/b_difficult.png", 200, 60);
	loadimage(&impossibleModel, "images/buttons/b_impossible.png", 200, 60);
	loadimage(&choose, "images/buttons/choose.png", 70, 70);
	loadimage(&brain, "images/foods/brain.png");
	loadimage(&poiWeed, "images/foods/poi_weed.png");
	loadimage(&landmine, "images/foods/landmine.png");
	loadimage(&tone, "images/foods/tone.png");
	loadimage(foods, "images/foods/food_1.png");
	loadimage(foods + 1, "images/foods/food_2.png");
	loadimage(foods + 2, "images/foods/food_3.png");
	loadimage(foods + 3, "images/foods/food_4.png");
	loadimage(foods + 4, "images/foods/food_5.png");
	loadimage(foods + 5, "images/foods/food_6.png");
	loadimage(foods + 6, "images/foods/food_7.png");
	loadimage(foods + 7, "images/foods/food_8.png");
	loadimage(foods + 8, "images/foods/food_9.png");
	loadimage(foods + 9, "images/foods/food_10.png");
	loadimage(foods + 10, "images/foods/food_11.png");
	loadimage(foods + 11, "images/foods/food_12.png");
	loadimage(&singleLoss, "images/background/single_loss.png");
	loadimage(&singleWin, "images/background/single_win.png");
	loadimage(&P1Win, "images/background/double_p1_win.png");
	loadimage(&P2Win, "images/background/double_p2_win.png");
	loadimage(&allWin, "images/background/double_all_win.png");
	loadimage(&b_menu, "images/buttons/b_menu.png");
	loadimage(&b_pause, "images/buttons/b_pause.png");
	loadimage(&snakeBrain[0], "images/snake/head_up_brain.png");
	loadimage(&snakeBrain[1], "images/snake/head_right_brain.png");
	loadimage(&snakeBrain[2], "images/snake/head_down_brain.png");
	loadimage(&snakeBrain[3], "images/snake/head_left_brain.png");

	//up right down left
	//snake_1 init
	//init head_1
	loadimage(&snake_1[0][0], "images/snake/head_up_1.png");
	loadimage(&snake_1[0][1], "images/snake/head_right_1.png");
	loadimage(&snake_1[0][2], "images/snake/head_down_1.png");
	loadimage(&snake_1[0][3], "images/snake/head_left_1.png");
	//init body
	loadimage(&snake_1[1][0], "images/snake/body_up_1.png");
	loadimage(&snake_1[1][1], "images/snake/body_right_1.png");
	loadimage(&snake_1[1][2], "images/snake/body_down_1.png");
	loadimage(&snake_1[1][3], "images/snake/body_left_1.png");
	//init body_r
	//utol rtou dtor ltod
	loadimage(&snake_1[2][0], "images/snake/body_utol_1.png");
	loadimage(&snake_1[2][1], "images/snake/body_rtou_1.png");
	loadimage(&snake_1[2][2], "images/snake/body_dtor_1.png");
	loadimage(&snake_1[2][3], "images/snake/body_ltod_1.png");
	//utor rtod dtol ltou
	loadimage(&snake_1[3][0], "images/snake/body_utor_1.png");
	loadimage(&snake_1[3][1], "images/snake/body_rtod_1.png");
	loadimage(&snake_1[3][2], "images/snake/body_dtol_1.png");
	loadimage(&snake_1[3][3], "images/snake/body_ltou_1.png");
	//init tail_1
	loadimage(&snake_1[4][0], "images/snake/tail_up_1.png");
	loadimage(&snake_1[4][1], "images/snake/tail_right_1.png");
	loadimage(&snake_1[4][2], "images/snake/tail_down_1.png");
	loadimage(&snake_1[4][3], "images/snake/tail_left_1.png");
	
	//snake_2 init
	//init head_1
	loadimage(&snake_2[0][0], "images/snake/head_up_2.png");
	loadimage(&snake_2[0][1], "images/snake/head_right_2.png");
	loadimage(&snake_2[0][2], "images/snake/head_down_2.png");
	loadimage(&snake_2[0][3], "images/snake/head_left_2.png");
	//init body
	loadimage(&snake_2[1][0], "images/snake/body_up_2.png");
	loadimage(&snake_2[1][1], "images/snake/body_right_2.png");
	loadimage(&snake_2[1][2], "images/snake/body_down_2.png");
	loadimage(&snake_2[1][3], "images/snake/body_left_2.png");
	//init body_r
	//utol rtou dtor ltod
	loadimage(&snake_2[2][0], "images/snake/body_utol_2.png");
	loadimage(&snake_2[2][1], "images/snake/body_rtou_2.png");
	loadimage(&snake_2[2][2], "images/snake/body_dtor_2.png");
	loadimage(&snake_2[2][3], "images/snake/body_ltod_2.png");
	//utor rtod dtol ltou
	loadimage(&snake_2[3][0], "images/snake/body_utor_2.png");
	loadimage(&snake_2[3][1], "images/snake/body_rtod_2.png");
	loadimage(&snake_2[3][2], "images/snake/body_dtol_2.png");
	loadimage(&snake_2[3][3], "images/snake/body_ltou_2.png");
	//init tail_1
	loadimage(&snake_2[4][0], "images/snake/tail_up_2.png");
	loadimage(&snake_2[4][1], "images/snake/tail_right_2.png");
	loadimage(&snake_2[4][2], "images/snake/tail_down_2.png");
	loadimage(&snake_2[4][3], "images/snake/tail_left_2.png");
}

void loadAllMusic() {
	mciSendString(_T("open music\\backgroundMusic.mp3 alias backgroundMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\menuMusic.mp3 alias menuMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\readyGo.mp3 alias readyGoMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\eatFood.wav alias eatFoodMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\eatLandmine.wav alias eatLandmineMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\eatPoiWeed.wav alias eatPoiWeedMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\eatBrain.wav alias eatBrainMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\BrainOver.wav alias brainOverMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\loss.wav alias lossMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\win.wav alias winMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\gameOver.wav alias gameOverMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\pause.wav alias pauseMusic"), NULL, 0, NULL);
	mciSendString(_T("open music\\button.wav alias buttonMusic"), NULL, 0, NULL);
}

void *playEatFoodMusic(void*p) {
	mciSendString(_T("open music\\eatFood.wav alias eatFoodMusic"), NULL, 0, NULL);
	mciSendString(_T("play eatFoodMusic from 0"), NULL, 0, NULL);
	return NULL;
}

void *playEatPoiWeedMusic(void*p) {
	mciSendString(_T("open music\\eatPoiWeed.wav alias eatPoiWeedMusic"), NULL, 0, NULL);
	mciSendString(_T("play eatPoiWeedMusic from 0"), NULL, 0, NULL);
	return NULL;
}

void *playEatLandmineMusic(void*p) {
	mciSendString(_T("open music\\eatLandmine.wav alias eatLandmineMusic"), NULL, 0, NULL);
	mciSendString(_T("play eatLandmineMusic from 0"), NULL, 0, NULL);
	return NULL;
}

void *playEatBrainMusic(void*p) {
	mciSendString(_T("open music\\eatBrain.wav alias eatBrainMusic"), NULL, 0, NULL);
	mciSendString(_T("play eatBrainMusic from 0"), NULL, 0, NULL);
	return NULL;
}

void *playBrainOverMusic(void*) {
	mciSendString(_T("open music\\BrainOver.wav alias brainOverMusic"), NULL, 0, NULL);
	mciSendString(_T("play brainOverMusic from 0"), NULL, 0, NULL);
	return NULL;
}

void gameStart_single() {
	mciSendString(_T("play readyGoMusic from 0"), NULL, 0, NULL);
	int score;
	int poiWeedsTime = 0;
	Direction d_temp;
	int keyASC;
	int poi_times = FLASH_FREQ;
	int brainTime = 0;
	int autoTime = 0;
	Bool start = false;
	Bool hasBrain_1 = false;
	Bool brainOvering = false;
	putimage(0, 200, &readyImage);
	Sleep(1500);
	cleardevice();
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	FlushMouseMsgBuffer();
	keyUsed_1 = false;
	isWin = false;
	while (true)
	{
		paintPoiWeeds = !paintPoiWeeds;
		cleardevice();
		
		putimage(0, 0, &gameBackground);
		if (FLASH_FREQ == poi_times)
		{
			
			
			updateMap();
			score = 10 * (snakeLength_1 - SNAKE_INIT_LENGTH);
			
			
		}
		
		paintSingleScore(score);
		paintMap(hasBrain_1, false);
		if (!start)
		{
			putimage(0, 200, &readyImage);
			Sleep(1000);
			start = true;
			mciSendString(_T("stop readyGoMusic"), NULL, 0, NULL);
			mciSendString(_T("play backgroundMusic repeat"), NULL, 0, NULL);
		}
		Sleep(sleepTime / FLASH_FREQ);
		MOUSEMSG msg;
		int x, y;
		while (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 635 && x < 785 && y>480 && y < 530)
				{
					mciSendString(_T("play pauseMusic from 0"), NULL, 0, NULL);
					isPause = !isPause;
				}
				else if (x > 635 && x < 785 && y > 550 && y < 600)
				{
					setIsMainGUI(true);
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto start;
				}
			}
		}
		while (kbhit()) {
			keyASC = getch();
			switch (keyASC)
			{
			case ' ':
				mciSendString(_T("play pauseMusic from 0"), NULL, 0, NULL);
				isPause = !isPause;
			}
			if (!keyUsed_1 && !isPause)
			{
				d_temp = listenDirection(keyASC, P1);
				if (d_temp != ERR)
				{
					direction_1 = d_temp;
					keyUsed_1 = true;
				}
			}
		}

		if (FLASH_FREQ != poi_times)
		{
			if (!isPause)
			{
				poi_times++;
				if (poiWeedsTime <= POI_WEEDS_TIME)
				{
					poiWeedsTime += (int)(sleepTime / FLASH_FREQ);
				}
				else
				{
					poiWeedsTime = 0;
					hasPoiWeeds = false;
					poi_weedOff = !poi_weedOff;
				}

				if (brainTime <= BRAIN_TIME)
				{
					brainTime += (int)(sleepTime / FLASH_FREQ);
				}
				else
				{
					brainTime = 0;
					hasBrain = false;
					brainOff = !brainOff;
				}
			}
			continue;
		}
		else
		{
			poi_times = 0;
			writeFile();
			if (autoTime > 0)
			{
				autoMove_1 = true;
				autoTime -= sleepTime;
				if (autoTime < 2300)
				{
					if (!brainOvering)
					{
						if (pthread_create(&newThread_2, NULL, playBrainOverMusic, NULL) == -1)
							mciSendString(_T("play brainOverMusic from 0"), NULL, 0, NULL);
					}
					brainOvering = true;
					hasBrain_1 = !hasBrain_1;
				}
				else {
					hasBrain_1 = true;
					brainOvering = false;
				}
			}
			else {
				autoMove_1 = false;
				hasBrain_1 = false;
			}
			if (!(keyUsed_1 && autoTime < 2300 && autoTime > 0) && autoMove_1)
			{
				direction_1 = autoGetDirection(P1, single_Model);
			}
			keyUsed_1 = false;
		}

		if (eatFood(P1))
		{
			if(pthread_create(&newThread_1, NULL, playEatFoodMusic, NULL) == -1)
				mciSendString(_T("play eatFoodMusic from 0"), NULL, 0, NULL);
			growUp(P1);
			snakeLength_1++;
		}
		else if (eatPoiWeed(P1))
		{
			if (pthread_create(&newThread_3, NULL, playEatPoiWeedMusic, NULL) == -1)
				mciSendString(_T("play eatPoiWeedMusic from 0"), NULL, 0, NULL);
			if (snakeLength_1 <= 1)
			{
				isOver = true;
			}
			else {
				reduceCell(P1);
				snakeLength_1--;
				move(P1);
			}
		}
		else if (eatLandmine(P1)) {
			if (pthread_create(&newThread_1, NULL, playEatLandmineMusic, NULL) == -1)
				mciSendString(_T("play eatLandmineMusic from 0"), NULL, 0, NULL);
			if (snakeLength_1 <= 1)
			{
				isOver = true;
			}
			else {
				reduceHalf(P1);
				snakeLength_1 = snakeLength_1 - snakeLength_1 / 2;
				move(P1);
			}
		}
		else if (eatTone(P1)) {
			isOver = true;
		}
		else if (eatBrain(P1)) {
			if (pthread_create(&newThread_3, NULL, playEatBrainMusic, NULL) == -1)
				mciSendString(_T("play eatBrainMusic from 0"), NULL, 0, NULL);
			autoTime = BRAIN_MAINTAIN_TIME;
			move(P1);
		}
		else
		{
			move(P1);
		}

		if (outOfBoundary(P1) || isOver || map[head_1->point.x][head_1->point.y] == BODY_1)
		{
			break;
		}
		else if (score >= scoreThresholdValue)
		{
			isWin = true;
		}
	}
	isOver = true;
	mciSendString(_T("stop backgroundMusic"), NULL, 0, NULL);
	if (isWin)
	{
		mciSendString(_T("play winMusic from 0"), NULL, 0, NULL);
	}
	else {
		mciSendString(_T("play lossMusic from 0"), NULL, 0, NULL);
	}
	paintGameOver(isWin);
start:
	highestScore = score;
	writeTop3();
	mciSendString(_T("stop backgroundMusic"), NULL, 0, NULL);
}

void gameStart_double() {
	mciSendString(_T("play readyGoMusic from 0"), NULL, 0, NULL);
	int score_1 = 0, score_2 = 0;
	int poiWeedsTime = 0;
	int brainTime = 0;
	int autoTime_1 = 0, autoTime_2 = 0;
	Direction d_temp_1, d_temp_2;
	int keyASC;
	int poi_times = 0;
	Bool start = false;
	Player winner = none;
	int nextX_1, nextY_1;
	int nextX_2, nextY_2;
	Bool hasBrain_1 = false, hasBrain_2 = false;
	Bool brainOvering_1 = false, brainOvering_2 = false;
	putimage(0, 200, &readyImage);
	Sleep(1500);
	cleardevice();
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	FlushMouseMsgBuffer();
	while (true)
	{
		score_1 = 10 * (snakeLength_1 - SNAKE_INIT_LENGTH);
		score_2 = 10 * (snakeLength_2 - SNAKE_INIT_LENGTH);
		
		paintPoiWeeds = !paintPoiWeeds;
		keyUsed_1 = false;
		keyUsed_2 = false;
		cleardevice();
		putimage(0, 0, &gameBackground);
		updateMap();
		paintMap(hasBrain_1, hasBrain_2);
		paintDoubleScore(score_1, score_2);
		if (!start)
		{
			putimage(0, 200, &readyImage);
			Sleep(1000);
			start = true;
			mciSendString(_T("stop readyGoMusic"), NULL, 0, NULL);
			mciSendString(_T("play backgroundMusic repeat"), NULL, 0, NULL);
		}
		Sleep(sleepTime / FLASH_FREQ);
		MOUSEMSG msg;
		int x, y;
		while (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 635 && x < 785 && y>480 && y < 530)
				{
					mciSendString(_T("play pauseMusic from 0"), NULL, 0, NULL);
					isPause = !isPause;
				}
				else if (x > 635 && x < 785 && y > 550 && y < 600)
				{
					setIsMainGUI(true);
					mciSendString(_T("play buttonMusic from 0"), NULL, 0, NULL);
					goto start;
				}
			}
		}
		while (_kbhit()) {
			keyASC = _getch();
			switch (keyASC)
			{
			case ' ':
				mciSendString(_T("play pauseMusic from 0"), NULL, 0, NULL);
				isPause = !isPause;
			}
			if (!keyUsed_1 && !autoMove_1 && !isPause)
			{
				d_temp_1 = listenDirection(keyASC, P1);
				if (d_temp_1 != ERR)
				{
					direction_1 = d_temp_1;
					keyUsed_1 = true;
				}
			}
			if (!keyUsed_2 && !autoMove_2 && !isPause)
			{
				d_temp_2 = listenDirection(keyASC, P2);
				if (d_temp_2 != ERR)
				{
					direction_2 = d_temp_2;
					keyUsed_2 = true;
				}
			}
		}
		if (FLASH_FREQ != poi_times)
		{
			if (!isPause)
			{
				poi_times++;
				if (poiWeedsTime <= POI_WEEDS_TIME)
				{
					poiWeedsTime += (int)(sleepTime / FLASH_FREQ);
				}
				else
				{
					poiWeedsTime = 0;
					hasPoiWeeds = false;
					poi_weedOff = !poi_weedOff;
				}

				if (brainTime <= BRAIN_TIME)
				{
					brainTime += (int)(sleepTime / FLASH_FREQ);
				}
				else
				{
					brainTime = 0;
					hasBrain = false;
					brainOff = !brainOff;
				}
			}
			continue;
		}
		else
		{
			poi_times = 0;
			writeFile();
			if (autoTime_1 > 0)
			{
				autoMove_1 = true;
				autoTime_1 -= sleepTime;
				if (autoTime_1 < 2300)
				{
					if (!brainOvering_1)
					{
						if (pthread_create(&newThread_3, NULL, playBrainOverMusic, NULL) == -1)
							mciSendString(_T("play brainOverMusic from 0"), NULL, 0, NULL);
					}
					brainOvering_1 = true;
					hasBrain_1 = !hasBrain_1;
				}
				else {
					brainOvering_1 = false;
					hasBrain_1 = true;
				}
			}
			else {
				autoMove_1 = false;
				hasBrain_1 = false;
			}
			
			if (currentModel == computer_Model)
			{
				autoMove_2 = true;
				hasBrain_2 = false;
			}
			else
			{
				if (autoTime_2 > 0)
				{
					autoMove_2 = true;
					autoTime_2 -= sleepTime;
					if (autoTime_2 < 2300)
					{
						if (!brainOvering_2)
						{
							if (pthread_create(&newThread_3, NULL, playBrainOverMusic, NULL) == -1)
								mciSendString(_T("play brainOverMusic from 0"), NULL, 0, NULL);
						}
						brainOvering_2 = true;
						hasBrain_2 = !hasBrain_2;
					}
					else
					{
						brainOvering_2 = false;
						hasBrain_2 = true;
					}
				}
				else {
					autoMove_2 = false;
					hasBrain_2 = false;
				}
			}
			if (autoMove_1)
			{
				direction_1 = autoGetDirection(P1, double_Model);
			}
			if (autoMove_2)
			{
				direction_2 = autoGetDirection(P2, double_Model);
			}
		}

		getNextXY(direction_1, P1, &nextX_1, &nextY_1);
		getNextXY(direction_2, P2, &nextX_2, &nextY_2);

		if (map[nextX_1][nextY_1] == BODY_2 && map[nextX_2][nextY_2] == BODY_1)
		{
			isOver = true;
			if (snakeLength_1 > snakeLength_2)
			{
				winner = P1;
			}
			else if (snakeLength_1 < snakeLength_2)
			{
				winner = P2;
			}
			else
			{
				winner = none;
			}
		}
		else if (map[nextX_1][nextY_1] == BODY_2)
		{
			isOver = true;
			winner = P2;
		}
		else if (map[nextX_2][nextY_2] == BODY_1)
		{
			isOver = true;
			winner = P1;
		}
		else if (map[nextX_1][nextY_1] == HEAD_2 || map[nextX_2][nextY_2] == HEAD_1)
		{
			if (snakeLength_1 > snakeLength_2)
			{
				isOver = true;
				winner = P1;
			}
			else if (snakeLength_1 < snakeLength_2)
			{
				isOver = true;
				winner = P2;
			}
			else
			{
				isOver = true;
				winner = none;
			}
		}

		if (isOver)
		{
			break;
		}

		if (eatFood(P1))
		{
			if (pthread_create(&newThread_1, NULL, playEatFoodMusic, NULL) == -1)
				mciSendString(_T("play eatFoodMusic from 0"), NULL, 0, NULL);
			growUp(P1);
			snakeLength_1++;
		}
		else if (eatPoiWeed(P1))
		{
			if (pthread_create(&newThread_1, NULL, playEatPoiWeedMusic, NULL) == -1)
				mciSendString(_T("play eatPoiWeedMusic from 0"), NULL, 0, NULL);
			if (snakeLength_1 <= 1)
			{
				isOver = true;
				winner = P2;
			}
			else {
				snakeLength_1--;
				reduceCell(P1);
				move(P1);
			}
		}
		else if (eatLandmine(P1)) {
			if (pthread_create(&newThread_1, NULL, playEatLandmineMusic, NULL) == -1)
				mciSendString(_T("play eatLandmineMusic from 0"), NULL, 0, NULL);
			if (snakeLength_1 <= 1)
			{
				isOver = true;
				winner = P2;
			}
			else {
				reduceHalf(P1);
				snakeLength_1 = snakeLength_1 - snakeLength_1 / 2;
				move(P1);
			}
		}
		else if (eatTone(P1)) {
			isOver = true;
			winner = P2;
		}
		else if (eatBrain(P1)) {
			if (pthread_create(&newThread_1, NULL, playEatBrainMusic, NULL) == -1)
				mciSendString(_T("play eatBrainMusic from 0"), NULL, 0, NULL);
			autoTime_1 = BRAIN_MAINTAIN_TIME;
			move(P1);
		}
		else
		{
			move(P1);
		}

		if (eatFood(P2))
		{
			if (pthread_create(&newThread_2, NULL, playEatFoodMusic, NULL) == -1)
				mciSendString(_T("play eatFoodMusic from 0"), NULL, 0, NULL);
			growUp(P2);
			snakeLength_2++;
		}
		else if (eatPoiWeed(P2))
		{
			if (pthread_create(&newThread_2, NULL, playEatPoiWeedMusic, NULL) == -1)
				mciSendString(_T("play eatPoiWeedMusic from 0"), NULL, 0, NULL);
			if (snakeLength_2 <= 1)
			{
				isOver = true;
				if (winner == P2)
				{
					winner = none;
				}
				else {
					winner = P1;
				}
			}
			else {
				snakeLength_2--;
				reduceCell(P2);
				move(P2);
			}
		}
		else if (eatLandmine(P2)) {
			if (pthread_create(&newThread_2, NULL, playEatLandmineMusic, NULL) == -1)
				mciSendString(_T("play eatLandmineMusic from 0"), NULL, 0, NULL);
			if (snakeLength_2 <= 1)
			{
				isOver = true;
				if (winner == P2)
				{
					winner = none;
				}
				else {
					winner = P1;
				}
			}
			else {
				reduceHalf(P2);
				snakeLength_2 = snakeLength_2 - snakeLength_2 / 2;
				move(P2);
			}
		}
		else if (eatTone(P2)) {
			isOver = true;
			if (winner == P2)
			{
				winner = none;
			}
			else {
				winner = P1;
			}
		}
		else if (eatBrain(P2)) {
			if (pthread_create(&newThread_2, NULL, playEatBrainMusic, NULL) == -1)
				mciSendString(_T("play eatBrainMusic from 0"), NULL, 0, NULL);
			autoTime_2 = BRAIN_MAINTAIN_TIME;
			move(P2);
		}
		else
		{
			move(P2);
		}

		if (map[head_1->point.x][head_1->point.y] == BODY_2)
		{
			isOver = true;
			winner = P2;
		}
		
		if (head_1->point.x == head_2->point.x && head_1->point.y == head_2->point.y)
		{
			isOver = true;
			if (snakeLength_1 > snakeLength_2)
			{
				winner = P1;
			}
			else if (snakeLength_1 < snakeLength_2)
			{
				winner = P2;
			}
			else
			{
				winner = none;
			}
		}

		if (outOfBoundary(P2) && outOfBoundary(P1))
		{
			isOver = true;
			if (snakeLength_1 > snakeLength_2)
			{
				winner = P1;
			}
			else if (snakeLength_1 < snakeLength_2)
			{
				winner = P2;
			}
			else
			{
				winner = none;
			}
		}
		else if (outOfBoundary(P1))
		{
			isOver = true;
			winner = P2;
		}
		else if (outOfBoundary(P2))
		{
			isOver = true;
			winner = P1;
		}

		if (isOver)
		{
			break;
		}
	}
	mciSendString(_T("stop backgroundMusic"), NULL, 0, NULL);
	mciSendString(_T("play gameOverMusic from 0"), NULL, 0, NULL);
	paintWinner(winner);
start:
	switch (currentModel)
	{
	case double_Model:
		highestScore = max(score_1, score_2);
		break;
	case computer_Model:
		highestScore = score_1;
		break;
	}
	writeTop3();
	mciSendString(_T("stop backgroundMusic"), NULL, 0, NULL);
}

Bool outOfBoundary(Player player) {
	int x, y;
	switch (player)
	{
	case P1:
		x = head_1->point.x;
		y = head_1->point.y;
		break;
	case P2:
		x = head_2->point.x;
		y = head_2->point.y;
		break;
	}
	if (x < 0 || x > MAP_X - 1 || y < 0 || y > MAP_Y - 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool eatLandmine(Player player) {
	Snake *head = NULL;
	Direction direction = ERR;
	switch (player)
	{
	case P1:
		head = head_1;
		direction = direction_1;
		break;
	case P2:
		head = head_2;
		direction = direction_2;
		break;
	}
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] && head->point.y == landmine_y[i] + 1)
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] && head->point.y == landmine_y[i] - 1)
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] + 1 && head->point.y == landmine_y[i])
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] - 1 && head->point.y == landmine_y[i])
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

Bool eatTone(Player player) {
	Snake *head = NULL;
	Direction direction = ERR;
	switch (player)
	{
	case P1:
		head = head_1;
		direction = direction_1;
		break;
	case P2:
		head = head_2;
		direction = direction_2;
		break;
	}
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
		{
			if (head->point.x == tone_x[i] && head->point.y == tone_y[i] + 1)
			{
				tone_x[i] = -2;
				tone_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
		{
			if (head->point.x == tone_x[i] && head->point.y == tone_y[i] - 1)
			{
				tone_x[i] = -2;
				tone_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
		{
			if (head->point.x == tone_x[i] + 1 && head->point.y == tone_y[i])
			{
				tone_x[i] = -2;
				tone_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
		{
			if (head->point.x == tone_x[i] - 1 && head->point.y == tone_y[i])
			{
				tone_x[i] = -2;
				tone_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

Bool eatBrain(Player player) {
	if (brainOff)
	{
		return false;
	}
	Snake *head = NULL;
	Direction direction = ERR;
	switch (player)
	{
	case P1:
		head = head_1;
		direction = direction_1;
		break;
	case P2:
		head = head_2;
		direction = direction_2;
		break;
	}
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfBrain; i++)
		{
			if (head->point.x == brain_x[i] && head->point.y == brain_y[i] + 1)
			{
				brain_x[i] = -2;
				brain_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfBrain; i++)
		{
			if (head->point.x == brain_x[i] && head->point.y == brain_y[i] - 1)
			{
				brain_x[i] = -2;
				brain_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfBrain; i++)
		{
			if (head->point.x == brain_x[i] + 1 && head->point.y == brain_y[i])
			{
				brain_x[i] = -2;
				brain_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfBrain; i++)
		{
			if (head->point.x == brain_x[i] - 1 && head->point.y == brain_y[i])
			{
				brain_x[i] = -2;
				brain_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

void reduceHalf(Player player) {
	int t;
	Snake *tail = NULL;
	switch (player)
	{
	case P1:
		t = snakeLength_1 / 2;
		for (int i = 0; i < t; i++)
		{
			tail_1 = tail_1->previous;
			//free(tail_1->next);
		}
		tail_1->isTail = true;
		tail_1->next = NULL;
		break;
	case P2:
		t = snakeLength_2 / 2;
		for (int i = 0; i < t; i++)
		{
			tail_2 = tail_2->previous;
			//free(tail_2->next);
		}
		tail_2->isTail = true;
		tail_2->next = NULL;
		break;
	}
}

void createLandmine(int x0, int y0, int dx, int dy) {
	if (!hasLandmine)
	{
		setfillcolor(BLACK);
		for (int i = 0; i < numOfLandmine; i++)
		{
			landmine_x[i] = rand() % 30;
			landmine_y[i] = rand() % 30;
			while (map[landmine_x[i]][landmine_y[i]] != NOTHING) {
				landmine_x[i] = rand() % 30;
				landmine_y[i] = rand() % 30;
			}
			map[landmine_x[i]][landmine_y[i]] = LANDMINE;
			putimage(x0 + landmine_x[i] * dx, y0 + landmine_y[i] * dy, &landmine);
			//paintCell(x0, y0, dx, dy, landmine_x[i], landmine_y[i]);
		}
		hasLandmine = true;
	}
	else
	{
		setfillcolor(BLACK);
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (landmine_x[i] != -2 && landmine_y[i] != -2)
			{
				map[landmine_x[i]][landmine_y[i]] = LANDMINE;
				putimage(x0 + landmine_x[i] * dx, y0 + landmine_y[i] * dy, &landmine);
				//paintCell(x0, y0, dx, dy, landmine_x[i], landmine_y[i]);
			}
		}
	}
}

void createBrain(int x0, int y0, int dx, int dy) {
	if (brainOff)
	{
		return;
	}
	if (!hasBrain)
	{
		for (int i = 0; i < numOfBrain; i++)
		{
			brain_x[i] = rand() % 30;
			brain_y[i] = rand() % 30;
			while (map[brain_x[i]][brain_y[i]] != NOTHING) {
				brain_x[i] = rand() % 30;
				brain_y[i] = rand() % 30;
			}
			map[brain_x[i]][brain_y[i]] = BRAIN;
			putimage(x0 + brain_x[i] * dx, y0 + brain_y[i] * dy, &brain);
		}
		hasBrain = true;
	}
	else
	{
		for (int i = 0; i < numOfBrain; i++)
		{
			if (brain_x[i] != -2 && brain_y[i] != -2)
			{
				map[brain_x[i]][brain_y[i]] = BRAIN;
				putimage(x0 + brain_x[i] * dx, y0 + brain_y[i] * dy, &brain);
			}
		}
	}
}

void createTone(int x0, int y0, int dx, int dy) {
	if (!hasTone)
	{
		setfillcolor(BLACK);
		int numOfTonePerBarrier;
		int index = 0;
		Bool newBarrier;
		int safeDis = 5;
		int r;
		int rIndex;
		int times;
		if (currentModel == single_Model)
		{
			for (int i = 1; i <= safeDis; i++)
			{
				map[head_1->point.x - i][head_1->point.y] = NO_ANYTHING;
			}
		}
		else {
			for (int i = 1; i <= safeDis; i++)
			{
				map[head_1->point.x][head_1->point.y - i] = NO_ANYTHING;
				map[head_2->point.x][head_2->point.y - i] = NO_ANYTHING;
			}
		}
		
		for (int i = 0; i < numOfBarrier; i++)
		{
			numOfTonePerBarrier = rand() % (MAX_NUM_OF_TONE_PER_BARRIER - MIN_NUM_OF_TONE_PER_BARRIER + 1) + MIN_NUM_OF_TONE_PER_BARRIER;
			newBarrier = true;
			for (int j = 0; j < numOfTonePerBarrier; j++, index++)
			{
				if (newBarrier)
				{
					do
					{
						tone_x[index] = rand() % 30;
						tone_y[index] = rand() % 30;
					} while (map[tone_x[index]][tone_y[index]] != NOTHING);
					newBarrier = false;
				}
				else
				{
					times = 0;
					do
					{
						r = rand() % 4;
						rIndex = rand() % j + 1;
						switch (r)
						{
						case 0:
							tone_x[index] = tone_x[index - rIndex] - 1;
							tone_y[index] = tone_y[index - rIndex];
							break;
						case 1:
							tone_x[index] = tone_x[index - rIndex] + 1;
							tone_y[index] = tone_y[index - rIndex];
							break;
						case 2:
							tone_x[index] = tone_x[index - rIndex];
							tone_y[index] = tone_y[index - rIndex] - 1;
							break;
						case 3:
							tone_x[index] = tone_x[index - rIndex];
							tone_y[index] = tone_y[index - rIndex] + 1;
							break;
						}
						times++;
					} while ((tone_x[index] > 29 || tone_x[index] < 0 || tone_y[index] > 29 || tone_y[index] < 0 || map[tone_x[index]][tone_y[index]] != NOTHING) && times < 50);
					if (times >= 50)
					{
						do
						{
							tone_x[index] = rand() % 30;
							tone_y[index] = rand() % 30;
						} while (map[tone_x[index]][tone_y[index]] != NOTHING);
					}
				}
				map[tone_x[index]][tone_y[index]] = TONE;
				putimage(x0 + tone_x[index] * dx, y0 + tone_y[index] * dy, &tone);
			}
		}
		hasTone = true;
	}
	else
	{
		setfillcolor(BLACK);
		for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
		{
			if (tone_x[i] != -2 && tone_y[i] != -2)
			{
				map[tone_x[i]][tone_y[i]] = TONE;
				putimage(x0 + tone_x[i] * dx, y0 + tone_y[i] * dy, &tone);
				//paintCell(x0, y0, dx, dy, landmine_x[i], landmine_y[i]);
			}
		}
	}
}

Bool eatPoiWeed(Player player) {
	if (poi_weedOff)
	{
		return false;
	}
	Direction direction = ERR;
	Snake *head = NULL;
	switch (player)
	{
	case P1:
		direction = direction_1;
		head = head_1;
		break;
	case P2:
		direction = direction_2;
		head = head_2;
		break;
	}
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] && head->point.y == poiWeeds_y[i] + 1)
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] && head->point.y == poiWeeds_y[i] - 1)
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] + 1 && head->point.y == poiWeeds_y[i])
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] - 1 && head->point.y == poiWeeds_y[i])
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

void reduceCell(Player player) {
	switch (player)
	{
	case P1:
		tail_1 = tail_1->previous;
		tail_1->isTail = true;
		//free(tail_1->next);
		tail_1->next = NULL;
		break;
	case P2:
		tail_2 = tail_2->previous;
		tail_2->isTail = true;
		//free(tail_2->next);
		tail_2->next = NULL;
		break;
	}
}

void createPoiWeeds(int x0, int y0, int dx, int dy) {
	if (poi_weedOff) {
		return;
	}
	if (!hasPoiWeeds)
	{
		setfillcolor(YELLOW);
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			poiWeeds_x[i] = rand() % 30;
			poiWeeds_y[i] = rand() % 30;
			while (map[poiWeeds_x[i]][poiWeeds_y[i]] != NOTHING) {
				poiWeeds_x[i] = rand() % 30;
				poiWeeds_y[i] = rand() % 30;
			}
			map[poiWeeds_x[i]][poiWeeds_y[i]] = POI_WEED;
			putimage(x0 + poiWeeds_x[i] * dx, y0 + poiWeeds_y[i] * dy, &poiWeed);
			//paintCell(x0, y0, dx, dy, poiWeeds_x[i], poiWeeds_y[i]);
		}
		hasPoiWeeds = true;
	}
	else
	{
		setfillcolor(YELLOW);
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (poiWeeds_x[i] != -2 && poiWeeds_y[i] != -2)
			{
				map[poiWeeds_x[i]][poiWeeds_y[i]] = POI_WEED;
				if (paintPoiWeeds)
				{
					putimage(x0 + poiWeeds_x[i] * dx, y0 + poiWeeds_y[i] * dy, &poiWeed);
					//paintCell(x0, y0, dx, dy, poiWeeds_x[i], poiWeeds_y[i]);
				}
			}
		}
	}
}

Bool eatFood(Player player) {
	Direction direction = ERR;
	Snake *head = NULL;
	switch (player)
	{
	case P1:
		direction = direction_1;
		head = head_1;
		break;
	case P2:
		direction = direction_2;
		head = head_2;
		break;
	}
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] && head->point.y == food_y[i] + 1)
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] && head->point.y == food_y[i] - 1)
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] + 1 && head->point.y == food_y[i])
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] - 1 && head->point.y == food_y[i])
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

void growUp(Player player) {
	Snake *head = NULL;
	Direction direction = ERR;
	Snake* newCell = NULL;
	switch (player)
	{
	case P1:
		head = head_1;
		direction = direction_1;
		newCell = (Snake*)malloc(sizeof(Snake));
		newCell->next = head;
		head->previous = newCell;
		head->isHead = false;
		newCell->isHead = true;
		head = head_1 = newCell;
		break;
	case P2:
		head = head_2;
		direction = direction_2;
		newCell = (Snake*)malloc(sizeof(Snake));
		newCell->next = head;
		head->previous = newCell;
		head->isHead = false;
		newCell->isHead = true;
		head = head_2 = newCell;
		break;
	}

	switch (direction)
	{
	case UP:
		head->point.x = head->next->point.x;
		head->point.y = head->next->point.y - 1;
		break;
	case DOWN:
		head->point.x = head->next->point.x;
		head->point.y = head->next->point.y + 1;
		break;
	case LEFT:
		head->point.x = head->next->point.x - 1;
		head->point.y = head->next->point.y;
		break;
	case RIGHT:
		head->point.x = head->next->point.x + 1;
		head->point.y = head->next->point.y;
		break;
	default:
		break;
	}
}

void paintCell(int x0, int y0, int dx, int dy, int x, int y) {
	solidrectangle(x0 + x * dx, y0 + y * dy, x0 + x * dx + dx, y0 + y * dy + dy);
}

void createFood(int x0, int y0, int dx, int dy) {
	if (!hasFood)
	{
		for (int i = 0; i < numOfFood; i++) {
			food_x[i] = -2;
			food_y[i] = -2;
			foodIndex[i] = rand() % KINDS_OF_FOODS;
		}
		hasFood = true;
	}
	for (int i = 0; i < numOfFood; i++)
	{
		if (food_x[i] == -2 || food_y[i] == -2)
		{
			food_x[i] = rand() % 30;
			food_y[i] = rand() % 30;
			while (map[food_x[i]][food_y[i]] != NOTHING)
			{
				food_x[i] = rand() % 30;
				food_y[i] = rand() % 30;
			}
			foodIndex[i] = rand() % KINDS_OF_FOODS;
		}
		map[food_x[i]][food_y[i]] = FOOD;
		setfillcolor(BLUE);
		putimage(x0 + food_x[i] * dx, y0 + food_y[i] * dy, &foods[foodIndex[i]]);
		//paintCell(x0, y0, dx, dy, food_x[i], food_y[i]);
	}
}

Direction listenDirection(int key, Player player) {
	switch (player)
	{
	case P1:
		switch (key)
		{
		case KEY_UP_1:
			if (direction_1 == DOWN)
			{
				return ERR;
			}
			return UP;
		case KEY_DOWN_1:
			if (direction_1 == UP)
			{
				return ERR;
			}
			return DOWN;
		case KEY_RIGHT_1:
			if (direction_1 == LEFT)
			{
				return ERR;
			}
			return RIGHT;
		case KEY_LEFT_1:
			if (direction_1 == RIGHT)
			{
				return ERR;
			}
			return LEFT;
		default:
			return ERR;
		}
		break;
	case P2:
		switch (key)
		{
		case KEY_UP_2_0:
			if (direction_2 == DOWN)
			{
				return ERR;
			}
			return UP;
		case KEY_DOWN_2_0:
			if (direction_2 == UP)
			{
				return ERR;
			}
			return DOWN;
		case KEY_RIGHT_2_0:
			if (direction_2 == LEFT)
			{
				return ERR;
			}
			return RIGHT;
		case KEY_LEFT_2_0:
			if (direction_2 == RIGHT)
			{
				return ERR;
			}
			return LEFT;
		case KEY_UP_2_1:
			if (direction_2 == DOWN)
			{
				return ERR;
			}
			return UP;
		case KEY_DOWN_2_1:
			if (direction_2 == UP)
			{
				return ERR;
			}
			return DOWN;
		case KEY_RIGHT_2_1:
			if (direction_2 == LEFT)
			{
				return ERR;
			}
			return RIGHT;
		case KEY_LEFT_2_1:
			if (direction_2 == RIGHT)
			{
				return ERR;
			}
			return LEFT;
		default:
			return ERR;
		}
		break;
	}
	return ERR;
}

void move(Player player) {
	Snake *temp;
	Snake *head = NULL, *tail = NULL;
	Direction direction = ERR;
	switch (player)
	{
	case P1:
		direction = direction_1;
		head = head_1;
		tail = tail_1;
		break;
	case P2:
		direction = direction_2;
		head = head_2;
		tail = tail_2;
		break;
	}

	if (head->next != NULL)
	{
		if (head->next->point.x == head->point.x + 1 && direction == RIGHT)
		{
			direction = LEFT;
		}
		if (head->next->point.x == head->point.x - 1 && direction == LEFT)
		{
			direction = RIGHT;
		}
		if (head->next->point.y == head->point.y + 1 && direction == DOWN)
		{
			direction = UP;
		}
		if (head->next->point.y == head->point.y - 1 && direction == UP)
		{
			direction = DOWN;
		}
	}
	
	switch (direction)
	{
	case LEFT:
		tail->point.x = head->point.x - 1;
		tail->point.y = head->point.y;
		break;
	case RIGHT:
		tail->point.x = head->point.x + 1;
		tail->point.y = head->point.y;
		break;
	case UP:
		tail->point.x = head->point.x;
		tail->point.y = head->point.y - 1;
		break;
	case DOWN:
		tail->point.x = head->point.x;
		tail->point.y = head->point.y + 1;
		break;
	default:
		goto done;
	}
	if (tail == head)
	{
		head->next = NULL;
		head->previous = NULL;
	}
	else {
		switch (player)
		{
		case P1:
			temp = head_1;
			head_1 = tail_1;
			tail_1 = tail_1->previous;
			temp->previous = head_1;
			head_1->next = temp;
			tail_1->next = NULL;
			head_1->previous = NULL;
			break;
		case P2:
			temp = head_2;
			head_2 = tail_2;
			tail_2 = tail_2->previous;
			temp->previous = head_2;
			head_2->next = temp;
			tail_2->next = NULL;
			head_2->previous = NULL;
			break;
		}
	}
done:;
}

void initMap() {
	for (int i = 0; i < MAP_Y; i++)
	{
		for (int j = 0; j < MAP_X; j++)
		{
			map[j][i] = NOTHING;
		}
	}
}

void paintMap(Bool hasBrain_1, Bool hasBrain_2) {
	int x0 = 10, y0 = 10;
	int dx = DX, dy = DY;
	setcolor(BLUE);
	//rectangle(10, 10, 10 + 30 * DX, 10 + 30 * DY);
	paintSnake(x0, y0, dx, dy, P1, hasBrain_1);
	if (currentModel == double_Model || currentModel == computer_Model)
	{
		paintSnake(x0, y0, dx, dy, P2, hasBrain_2);
	}
	createTone(x0, y0, dx, dy);
	createPoiWeeds(x0, y0, dx, dy);
	createLandmine(x0, y0, dx, dy);
	createBrain(x0, y0, dx, dy);
	createFood(x0, y0, dx, dy);
	paintButtons();
}

void paintSnake(int x0, int y0, int dx, int dy, Player player, Bool hasBrain) {
	IMAGE* cell;
	Snake* snakeCell = NULL;
	switch (player)
	{
	case P1:
		snakeCell = tail_1;
		head_1->previous = NULL;
		break;
	case P2:
		snakeCell = tail_2;
		head_2->previous = NULL;
		break;
	}
	int x, y;
	while (snakeCell != NULL)
	{
		cell = getSnakeImage(snakeCell, player, hasBrain);
		x = snakeCell->point.x;
		y = snakeCell->point.y;
		putimage(x0 + dx * x, y0 + dy * y, cell);
		snakeCell = snakeCell->previous;
	}
}

void initSnake(int length, Player player) {
	switch (player)
	{
	case P1:
		head_1 = (Snake*)malloc(sizeof(Snake)*length);
		tail_1 = &head_1[length - 1];
		for (int i = 0; i < length - 1; i++)
		{
			head_1[i].next = &head_1[i + 1];
			head_1[i + 1].previous = &head_1[i];
		}
		head_1->isHead = true;
		tail_1->isTail = true;
		tail_1->next = NULL;
		head_1->previous = NULL;
		break;
	case P2:
		head_2 = (Snake*)malloc(sizeof(Snake)*length);
		tail_2 = &head_2[length - 1];
		for (int i = 0; i < length - 1; i++)
		{
			head_2[i].next = &head_2[i + 1];
			head_2[i + 1].previous = &head_2[i];
		}
		head_2->isHead = true;
		tail_2->isTail = true;
		tail_2->next = NULL;
		head_2->previous = NULL;
		break;
	}
}

void updateMap() {
	initMap();
	Snake* temp;
	int x, y;
	temp = head_1;
	while (temp != NULL) {
		if (temp == head_1)
		{
			x = temp->point.x;
			y = temp->point.y;
			map[x][y] = HEAD_1;
		}
		else
		{
			x = temp->point.x;
			y = temp->point.y;
			map[x][y] = BODY_1;
		}
		temp = temp->next;
	}

	if (currentModel == double_Model || currentModel == computer_Model)
	{
		temp = head_2;
		while (temp != NULL) {
			if (temp == head_2)
			{
				x = temp->point.x;
				y = temp->point.y;
				map[x][y] = HEAD_2;
			}
			else
			{
				x = temp->point.x;
				y = temp->point.y;
				map[x][y] = BODY_2;
			}
			temp = temp->next;
		}
	}

	for (int i = 0; i < numOfFood; i++)
	{
		if (food_x[i] != -2 && food_y[i] != -2) {
			map[food_x[i]][food_y[i]] = FOOD;
		}
	}

	for (int i = 0; i < numOfPoiWeeds; i++)
	{
		if (poiWeeds_x[i] != -2 && poiWeeds_y[i] != -2) {
			map[poiWeeds_x[i]][poiWeeds_y[i]] = POI_WEED;
		}
	}

	for (int i = 0; i < numOfLandmine; i++)
	{
		if (landmine_x[i] != -2 && landmine_y[i] != -2) {
			map[landmine_x[i]][landmine_y[i]] = LANDMINE;
		}
	}

	for (int i = 0; i < numOfBarrier*MAX_NUM_OF_TONE_PER_BARRIER; i++)
	{
		if (tone_x[i] != -2 && tone_y[i] != -2) {
			map[tone_x[i]][tone_y[i]] = TONE;
		}
	}

	for (int i = 0; i < numOfBrain; i++)
	{
		if (brain_x[i] != -2 && brain_y[i] != -2) {
			map[brain_x[i]][brain_y[i]] = BRAIN;
		}
	}
}

void initArr() {
	for (int i = 0; i < NUM_OF_FOODS_SIMPLE; i++)
	{
		food_x[i] = -2;
		food_y[i] = -2;
	}
	for (int i = 0; i < NUM_OF_POI_WEEDS_IMPOSSIBLE; i++)
	{
		poiWeeds_x[i] = -2;
		poiWeeds_y[i] = -2;
	}
	for (int i = 0; i < NUM_OF_BRAIN_SIMPLE; i++)
	{
		brain_x[i] = -2;
		brain_y[i] = -2;
	}
	for (int i = 0; i < NUM_OF_LANDMINE_IMPOSSIBLE; i++)
	{
		landmine_x[i] = -2;
		landmine_y[i] = -2;
	}
	for (int i = 0; i < MAX_NUM_OF_TONE_PER_BARRIER*NUM_OF_BARRIER_IMPOSSIBLE; i++)
	{
		tone_x[i] = -2;
		tone_y[i] = -2;
	}
}