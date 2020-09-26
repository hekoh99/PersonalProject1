#include <bangtal.h>
#include <iostream>
#include <conio.h>
using namespace bangtal;

#define UP 84
#define DOWN 85
#define LEFT 82
#define RIGHT 83

ScenePtr scene1;
ObjectPtr thing;
int thingX;

void keyboardCallback(KeyCode x, KeyState y) {
	if (x == LEFT) {
		thingX -= 10;
		if (thingX <= 0) {
			thingX = 0;
		}
		thing->locate(scene1, thingX, 250);
	}
	else if (x == RIGHT) {
		thingX += 10;
		if (thingX >= 1280) {
			thingX = 1280;
		}
		thing->locate(scene1, thingX, 250);
	}
}

int main(void) {
    
	setKeyboardCallback(keyboardCallback);
	
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	thingX = 550;

	scene1 = Scene::create("1", "images/바탕.png");
	thing = Object::create("images/별.png", scene1, thingX, 250);

	startGame(scene1);

    return 0;
}