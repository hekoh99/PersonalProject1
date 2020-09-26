#include <bangtal.h>
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

ScenePtr scene1;
ScenePtr endscene;
ObjectPtr thing;
int thingX;
int thingY = 20;
int hight[6];
int fin = 0;

ObjectPtr Bar[6];
int fallingBarY;

int checkPass(int num) {

	if (num == 0 && ((thingX >= 40 && thingX <= 80) ||(thingX >=430 && thingX <=460))) {
		return 1;
	}
	else if (num == 1 && ((thingX >= 490 && thingX <= 530))) {
		return 1;
	}
	else if (num == 2 && ((thingX >= 630 && thingX <= 670) || (thingX >= 1050 && thingX <= 1090))) {
		return 1;
	}
	else if (num == 3 && ((thingX >= 250 && thingX <= 290) || (thingX >= 1130 && thingX <= 1160))) {
		return 1;
	}
	else if (num == 4 && ((thingX >= 770 && thingX <= 810))) {
		return 1;
	}
	else if (num == 5 && ((thingX >= 0 && thingX <= 20) || (thingX >= 1030 && thingX <= 1050))) {
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
	auto timer = Timer::create(0.01f * level);
	timer->setOnTimerCallback([=](TimerPtr t)->bool {

		if (hight[barN] >= -20) {
			if (hight[barN] >= 20 && hight[barN] <= 90) {
				if (checkPass(barN) == 0) {
					endscene->enter();
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
				t->set(0.01f * level);
				t->start();
			}
		}
		if (hight[barN] <= -30) {
			bar->hide();
			fallingBar(Bar[(barN+1)%6], level, (barN+1)%6);
		}
		return true;
	});
	timer->start();
}

void keyboardCallback(KeyCode x, KeyState) {
	if (x == LEFT) {
		thingX -= 10;
		if (thingX <= 0) {
			thingX = 0;
		}
		thing->locate(scene1, thingX, thingY);
		cout << "here : " <<thingX << endl;
	}
	else if (x == RIGHT) {
		thingX += 10;
		if (thingX >= 1196) {
			thingX = 1196;
		}
		thing->locate(scene1, thingX, thingY);
		cout << "here : " << thingX << endl;
	}
}

int main(void) {
    
	setKeyboardCallback(keyboardCallback);
	
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	thingX = 550;

	scene1 = Scene::create("1", "images/바탕.png");
	endscene = Scene::create("1", "images/gameover.png");
	thing = Object::create("images/별.png", scene1, thingX, thingY);
	auto endbtn = Object::create("images/end.png", endscene, 600, 100);

	endbtn->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		endGame();
		return true;
		});

	for (int i = 0; i < 6; i++) {
		string filename = "images/바" + to_string(i + 1) + ".png";

		Bar[i] = Object::create(filename, scene1, 10, 720);
	}
	
	int n;
	srand((unsigned int)time(0));
	n = rand() % 6;
	cout << n << endl;
	fallingBar(Bar[n], 10, n);

	startGame(scene1);

    return 0;
}