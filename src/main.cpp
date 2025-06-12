#include <raylib.h>
#include <iostream>
#include "game.h"
#include <cstdlib>
#include <ctime>
#include "textureloader.h"


using namespace std;

int main()
{
    srand((unsigned)time(NULL));
    //const Color darkGreen = {20, 160, 133, 255};

    int screenWidth = 1488;
    int screenHeight = 912;
    

    InitWindow(screenWidth, screenHeight, "CG~~Defense");


    Game game(screenWidth,screenHeight);

    CloseWindow();
}