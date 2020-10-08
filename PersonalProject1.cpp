#include <bangtal>
#include <iostream>
#include <conio.h>
#include <string.h>
#include  <cstdlib>
#include<ctime>

using namespace bangtal;
using namespace std;

#define UP 84
#define DOWN 85
#define LEFT 82
#define RIGHT 83

int isPlaying = 0;
float highScore = 0;

ScenePtr scene1, startscene;
ScenePtr endscene;
ObjectPtr thing, startbtn;
int thingX;
int thingY = 20;
int hight[6];
int fin = 0;
TimerID Gametime;


ObjectPtr Bar[6];
int fallingBarY;

int makeRandom() {
	int n;
	srand((unsigned int)time(NULL));
	n = rand() % 6;

	return n;
}

int checkPass(int num) {

	if (num == 0 && ((thingX >= 40 && thingX <= 80) || (thingX >= 430 && thingX <= 460) || (thingX >= 1060 && thingX <= 1180))) {
		return 1;
	}
	else if (num == 1 && ((thingX >= 490 && thingX <= 530) || (thingX >= 30 && thingX <= 100) || (thingX >= 1050 && thingX <= 1110))) {
		return 1;
	}
	else if (num == 2 && ((thingX >= 630 && thingX <= 670) || (thingX >= 1050 && thingX <= 1090))) {
		return 1;
	}
	else if (num == 3 && ((thingX >= 250 && thingX <= 290) || (thingX >= 1130 && thingX <= 1160) || (thingX >= 910 && thingX <= 1000))) {
		return 1;
	}
	else if (num == 4 && ((thingX >= 770 && thingX <= 810) || (thingX >= 110 && thingX <= 230) || (thingX >= 1150))) {
		return 1;
	}
	else if (num == 5 && ((thingX >= 0 && thingX <= 20) || (thingX >= 1030 && thingX <= 1050) || (thingX >= 520 && thingX <= 540))) {
		return 1;
	}
	else {
		return 0;
	}
}

void fallingBar(ObjectPtr bar, int level, int barN) {
	hight[barN] = 720;
	bar->locate(scene1, 0, 720);
	bar->show();
	auto timer = Timer::create(0.01 * level);
	timer->setOnTimerCallback([=](TimerPtr t)->bool {

		if (isPlaying == 1) {
			if (hight[barN] >= -20) {
				if (hight[barN] >= 20 && hight[barN] <= 90) {
					if (checkPass(barN) == 0) {
						bar->hide();
						isPlaying = 2;
						stopTimer(Gametime);
						timer->stop();
						float time = 3600 - getTimer(Gametime);
						cout << "time : " << time << endl;
						endscene->enter();
						setTimer(Gametime, time);
						showTimer(Gametime);
						if (highScore < time) {
							highScore = time;
							showMessage("최고 기록 갱신!!");
						}
						startbtn->locate(endscene, 500, 200);
						startbtn->setImage("images/restart.png");
					}
					else {
						bar->locate(scene1, 0, hight[barN]);
						hight[barN] -= 10;
						t->set(0.01f * level);
						t->start();
					}
				}
				else {
					bar->locate(scene1, 0, hight[barN]);
					hight[barN] -= 10;
					t->set(0.01 * level);
					t->start();
				}
			}
			if (hight[barN] <= -30) {
				bar->hide();
				fallingBar(Bar[(barN + 1) % 6], level, (barN + 1) % 6);
			}
		}
		return true;
		});
	timer->start();
}


void keyboardCallback(KeyCode x, KeyState) {
	if (isPlaying == 1) {
		if (x == LEFT) {
			thingX -= 10;
			if (thingX <= 0) {
				thingX = 0;
			}
			thing->locate(scene1, thingX, thingY);
		}
		else if (x == RIGHT) {
			thingX += 10;
			if (thingX >= 1196) {
				thingX = 1196;
			}
			thing->locate(scene1, thingX, thingY);
		}
	}

}


int main(void) {

	setKeyboardCallback(keyboardCallback);

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	thingX = 550;

	scene1 = Scene::create("1", "images/바탕.png");
	startscene = Scene::create("1", "images/시작화면.png");
	startbtn = Object::create("images/start.png", startscene, 600, 300);

	endscene = Scene::create("1", "images/gameover.png");
	thing = Object::create("images/별.png", scene1, thingX, thingY);

	auto endbtn = Object::create("images/end.png", endscene, 700, 200);

	endbtn->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		endGame();
		return true;
		});



	startbtn->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {

		isPlaying = 1;
		thingX = 550;
		thingY = 20;
		for (int i = 0; i < 6; i++) {
			string filename = "images/바" + to_string(i + 1) + ".png";

			Bar[i] = Object::create(filename, scene1, 10, 720);
		}

		int n = makeRandom();

		fallingBar(Bar[n], 10, n);
		thing->locate(scene1, thingX, thingY);

		hideTimer();
		Gametime = createTimer(3600.0f);
		startTimer(Gametime);

		startGame(scene1);
		return true;
		});

	startGame(startscene);

	return 0;
}