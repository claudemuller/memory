#include "game.h"
#include "utils.h"

int main(void)
{
    if (!game_init()) {
        util_error("Failed to start game");
        return EXIT_FAILURE;
    }

    if (!game_run()) {
        util_error("Failed to start game");
        return EXIT_FAILURE;
    }

    game_destroy();

    return EXIT_SUCCESS;
}
