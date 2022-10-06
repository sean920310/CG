#include <iostream>
#include "GameHeader.h"

int main()
{
    GameManager game;
    while (1)
    {
        switch (game.menu())
        {
        case 0:
            return 0;
            break;
        case 1:
            game.inGame(InGameState::start);
            break;
        case 2:
            try
            {
                game.readFile();
            }
            catch (const char* e)
            {
                std::cout << e << std::endl;
                break;
            }
            game.inGame(InGameState::inputFile);
            break;
        default:
            break;
        }
    }
    //end of application
    return 0;
}