#include <bangtal.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#pragma comment (lib, "Bangtal.lib")
using namespace std;

namespace easy {
    SceneID scene1;
    ObjectID startButton;
    ObjectID piece[9];
    ObjectID menu2, start;
    TimerID timer1;

    int arr[3][3];
    // currentX[i] : 현재 i번쨰 조각의 X좌표가 어딘지
    int currentX[9], currentY[9];
    int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };

    // 이미지 파일의 위치
    const char* imgFileLocate[9] = { "images\\1\\1-1.png", "images\\1\\1-2.png", "images\\1\\1-3.png", "images\\1\\1-4.png", "images\\1\\1-5.png", "images\\1\\1-6.png",
            "images\\1\\1-7.png", "images\\1\\1-8.png" , "images\\1\\1-9.png" };

    // hideX,Y,Num.  현재 hide된 piece의 X,Y좌표와 몇번째인지
    int hX = 0, hY = 0, hNum = 0;

    // i 번째 조각이 현재 맞는 위치에 있는지.
    bool correct[9];

    // playing : 게임진행중, finish : 게임끝(클리어)
    bool playing = false;
    bool finish = false;


    // for testing bug
    void test() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                printf("%d ", arr[j][i]);
            }
            printf("\n");
        }
    }

    // 0 ~ n-1 중 random한 숫자 생성
    int myRand(int n) {
        int k = rand() % (n+1);
        if (n > 0) {
            return k;
        }
        return 0;

    }

    // num 번째 조각에 배열에서의 원래 위치의 X좌표
    int arrayX(int num) {
        return num % 3;
    }
    // num 번째 조각에 배열에서의 원래 위치의 X좌표
    int arrayY(int num) {
        return (int)(num / 3);
    }
    // 배열에서 x,y 좌표는 원래 number번째 조각.
    int number(int x, int y) {
        return (3 * y + x);
    }
    // 반댓방향 번호
    int oppositeDirection(int d) {
        return ((d + 2) % 3);
    }

    // (x,y)좌표로부터 이미지상 X좌표.
    // 사실 여기서 y는 안쓰이는 파라미터지만, coolX2 이런식으로 명명하면 훨씬 헷갈리니까
    // 좌표값을 받는다는 형식이란걸 일단 이렇게 표기합시다.
    int coolX(int x, int y) {
        return 398 + 200 * x;
    }
    int coolY(int x, int y) {
        return 461 - 200 * y;
    }
    // num 번쨰 조각의 원래 위치의 이미지상 X좌표
    int coolX(int num) {
        return coolX(arrayX(num), 0);
    }
    // num 번쨰 조각의 원래 위치의 이미지상 Y좌표.
    // 주의! 이미지 좌표는 자체정의한 배열의 Y좌표 방향과 반대.
    int coolY(int num) {
        return coolY(0, arrayY(num));
    }

    // 마우스 입력을 받았을때 쓰일 함수.
    // 클릭된 object가 몇번째 piece인지 return
    int findNum(ObjectID object) {
        for (int i = 0; i < 9; i++) {
            if (piece[i] == object) {
                return i;
            }
        }
        return -1; // piece가 아닌 object가 들어오면 -1.
    }

    // x+dx[direction], y+dy가 배열 안인지 확인
    bool checkIn(int x, int y, int direction) {
        int tx = x + dx[direction], ty = y + dy[direction];
        return (tx > -1 && tx < 3 && ty > -1 && ty < 3);
    }

    // arr[4][4]를 초기화 + initObject가 true면 piece 위치도 초기화
    void initializeArr(bool initObject) {
        int num = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                arr[j][i] = num;
                if (initObject) {
                    locateObject(piece[num], scene1, coolX(num), coolY(num));
                }
                num++;
            }
        }
    }
    // currentX,Y 초기화
    void initializeCurrentXY() {
        for (int i = 0; i < 9; i++) {
            currentX[i] = arrayX(i);
            currentY[i] = arrayY(i);
        }
    }
    // correct 초기화
    void initializeCorrect() {
        for (int i = 0; i < 9; i++) {
            correct[i] = true;
        }
    }
    bool checkFin() {
        for (int i = 0; i < 9; i++) {
            if (!correct[i]) {
                return false;
            }
        }
        return true;
    }

    void updateCorrect(int x, int y) {
        int num = number(x, y);
        if (arr[x][y] == num) {
            correct[num] = true;
        }
        else {
            correct[num] = false;
        }
    }

    // hPiece와 붙어있는지 확인하고, 근처에 있으면 방향 번호 리턴
    int checkHPieceDirection(int x, int y) {
        int tx = 0, ty = 0;
        for (int i = 0; i < 4; i++) {
            tx = x + dx[i];
            ty = y + dy[i];
            if (tx == hX && ty == hY) {
                return i;  // hidePiece가 근처에 있으면 방향 번호 리턴
            }
        }
        return -1; // hidePiece가 근처에 없으면 -1
    }


    void shuffle() {

        hNum = myRand(16);
        hX = arrayX(hNum);
        hY = arrayY(hNum);
        hideObject(piece[hNum]);


        // 128번 섞기
        for (int j = 0; j < 128; j++) {
            // directionIn[i] : 갈수있는 방향 중 i번째 방향
            int directionIn[4] = { 0, 0, 0, 0 };
            int n = 0, r = -1;


            // 갈수있는 방향 갯수 n 구하기 + 갈수있는 방향들에 번호부여
            for (int i = 0; i < 4; i++) {
                if (checkIn(hX, hY, i)) {
                    n++;
                    directionIn[n - 1] = i;
                }
            }

            r = myRand(n);
            //directionIn[r] 번째 방향으로 갈거임 ( = 그 방향 조각과 맞바꿀거임)

            int dNum = directionIn[r];

            int sx = hX + dx[dNum], sy = hY + dy[dNum];

            if (sx < 3) {
                int sNum = arr[sx][sy];

                int tx = hX, ty = hY;

                hX = sx, hY = sy;
                currentX[hNum] = sx, currentY[hNum] = sy;
                arr[sx][sy] = hNum;

                currentX[sNum] = tx, currentY[sNum] = ty;
                arr[tx][ty] = sNum;

                locateObject(piece[hNum], scene1, coolX(sx, 0), coolY(0, sy));
                locateObject(piece[sNum], scene1, coolX(tx, 0), coolY(0, ty));

                updateCorrect(sx, sy);
                updateCorrect(tx, ty);
            }

        }
    }

    void mouseCallback(ObjectID object, int x, int y, MouseAction) {
        if (object == startButton) {
            hideObject(startButton);
            shuffle();
            finish = false;
            playing = true;

            hideTimer();
            setTimer(timer1, 3600.0f);
            startTimer(timer1);

        }

        else if (object == menu2) {
            startGame(scene1);
        }
        else if (playing) {
            // num : 클릭한 조각 번호
            int num = findNum(object);

            // piece가 아닌걸 클릭하면 -1 반환했으니, piece중 하나 클릭한 경우만
            if (num >= 0) {
                int cx = currentX[num], cy = currentY[num];
                int direction = checkHPieceDirection(cx, cy);
                // 만약 인접해있다면, 자리 맞바꾸기
                if (direction > -1) {

                    int tx = hX, ty = hY;

                    //hPiece 옮기기
                    hX = cx, hY = cy;
                    currentX[hNum] = cx, currentY[hNum] = cy;
                    arr[cx][cy] = hNum;

                    //클릭한 piece 옮기기
                    currentX[num] = tx, currentY[num] = ty;
                    arr[tx][ty] = num;

                    //이미지 갱신
                    locateObject(piece[hNum], scene1, coolX(cx, 0), coolY(0, cy));
                    locateObject(piece[num], scene1, coolX(tx, 0), coolY(0, ty));

                    updateCorrect(cx, cy);
                    updateCorrect(tx, ty);

                    finish = checkFin();
                    if (finish) {
                        playing = false;
                        setObjectImage(startButton, "Images\\2\\restart.png");
                        showObject(startButton);
                        showObject(piece[hNum]);
                        stopTimer(timer1);
                        float time = 3600 - getTimer(timer1);
                        setTimer(timer1, time);
                        showTimer(timer1);

                    }
                }
            }

        }
    }
}

int main()
{
    setMouseCallback(easy::mouseCallback);

    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);



    easy::scene1 = createScene("퍼즐", "Images\\1\\배경1.png");
    easy::start = createScene("퍼즐", "Images\\바탕.png");
    easy::menu2 = createObject("Images\\메뉴1.png");
    locateObject(easy::menu2, easy::start, 200, 300);
    showObject(easy::menu2);

    // 조각 생성
    for (int i = 0; i < 9; i++) {
        easy::piece[i] = createObject(easy::imgFileLocate[i]);
        locateObject(easy::piece[i], easy::scene1, easy::coolX(i), easy::coolY(i));
        showObject(easy::piece[i]);
    }

    easy::startButton = createObject("Images\\2\\start.png");
    locateObject(easy::startButton, easy::scene1, 590, 80);
    showObject(easy::startButton);

    easy::initializeArr(false);
    easy::initializeCurrentXY();
    easy::initializeCorrect();

    easy::timer1 = createTimer(3600.0f);


    startGame(easy::start);

}