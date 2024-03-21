#include <stdint.h>
#include <stdio.h>
#include <time.h>

/* Enhance tic-tac-toe game performance through a strategic approach.
 * Rather than exclusively focusing on achieving three consecutive marks on a
 * 3x3 board, reimagine the game as aiming for three in a row across any of the
 * eight possible lines. In this variation, a single move can influence multiple
 * lines simultaneously.
 */
static const uint32_t move_masks[9] = {
    0x40040040, 0x20004000, 0x10000404, 0x04020000, 0x02002022,
    0x01000200, 0x00410001, 0x00201000, 0x00100110,
};

/* Determine if the tic-tac-toe board is in a winning state. */
static inline uint32_t is_win(uint32_t player_board)
{
    return (player_board + 0x11111111) & 0x88888888;
}

static inline int mod3(unsigned n)
{
    static char table[33] = {
        2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0,
        1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1,
    };

    n = __builtin_popcount(n ^ 0xAAAAAAAA);
    return table[n];
}

static inline int mod7(uint32_t x)
{
    x = (x >> 15) + (x & UINT32_C(0x7FFF));
    /* Take reminder as (mod 8) by mul/shift. Since the multiplier
     * was calculated using ceil() instead of floor(), it skips the
     * value '7' properly.
     *    M <- ceil(ldexp(8/7, 29))
     */
    return (int) ((x * UINT32_C(0x24924925)) >> 29);
}

/* specialized */
static inline uint32_t fastmod(uint32_t x, uint32_t n)
{
    switch (n) {
    case 2:
        return x & 1;
    case 3:
        return mod3(x);
    case 4:
        return x & 3;
    case 5:
        return x % 5;
    case 6:
        return x % 6;
    case 7:
        return mod7(x);
    case 8:
        return x & 7;
    case 9:
        return x % 9;
    default:
        return 0;
    }
}

/* Fast pseudo-random number generator */
uint32_t xorshift32()
{
    static uint32_t x = 0x12345678;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

/* Simulate a random game and display the sequence of moves made. */
uint32_t play_random_game(uint32_t player, uint32_t *moves)
{
    uint32_t boards[2] = {0, 0};
    uint32_t available_moves[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    for (uint32_t n_moves = 9; n_moves > 0; n_moves--) {
        /* Get board of player */
        uint32_t board = boards[player - 1];
        /* Choose random move */
        uint32_t i = fastmod(xorshift32(), n_moves);
        uint32_t move = available_moves[i];
        /* Delete move from available moves */
        available_moves[i] = available_moves[n_moves - 1];
        /* Apply move to board */
        board |= move_masks[move];
        /* Remember move */
        *moves++ = move;
        /* Check if current player won the game and return the winner */
        if (is_win(board))
            return player;
        /* Update board of player */
        boards[player - 1] = board;
        /* Next player, 1 -> 2, 2 -> 1 */
        player = 3 - player;
    }
    /* Mark end of game */
    *moves++ = -1;
    return 0;
}

int main()
{
    /* Run multiple iterations to verify the consistency of probabilities. */
    for (int k = 0; k < 10; k++) {
        double start_time = clock() / (double) CLOCKS_PER_SEC;
        /* Count wins by player (tie, player 1, player 2) */
        uint32_t wins[3] = {0, 0, 0};
        /* Count wins by first move */
        uint32_t wins_by_move[9] = {0};
        /* Simulate a million random games */
        int n_games = 1000 * 1000;
        for (int i = 0; i < n_games; i++) {
            uint32_t player = 1;
            /* Record which moves were played, last move is -1. */
            uint32_t moves[10] = {0};
            uint32_t winner = play_random_game(player, moves);
            /* Count wins */
            wins[winner]++;
            if (winner == player)
                wins_by_move[moves[0]]++;
        }

        double delta_time = clock() / (double) CLOCKS_PER_SEC - start_time;
        /* Print statistics */
        printf("Win probability for first move with random agents:\n");
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++)
                printf("%.3f ", wins_by_move[x + y * 3] * 1.0 / wins[1]);
            printf("\n");
        }
        printf("Player 1 won %u times\n", wins[1]);
        printf("Player 2 won %u times\n", wins[2]);
        printf("%u ties\n", wins[0]);
        printf("%f seconds\n", delta_time);
        printf("%f million games/sec\n", n_games * 1e-6 / delta_time);
        printf("\n");
    }

    return 0;
}