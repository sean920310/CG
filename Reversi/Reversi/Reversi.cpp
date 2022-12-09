#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "GameManager.h"

int main()
{
    GameManager game;
    game.run();
    return 0;
}