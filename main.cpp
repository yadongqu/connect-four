#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <assert.h>
#include <array>
const int COL = 7;
const int ROW = 6;

const int map[6][7] = {
    {5, 12, 19, 26, 33, 40, 47},
    {4, 11, 18, 25, 32, 39, 46},
    {3, 10, 17, 24, 31, 38, 45},
    {2, 9, 16, 23, 30, 37, 44},
    {1, 8, 15, 22, 29, 36, 43},
    {0, 7, 14, 21, 28, 35, 42}};

int COLUMN_ORDER[7] = {5, 4, 3, 2, 1, 0, 6};

struct Board
{
    uint64_t masks[2] = {0, 0};
    int height[COL] = {
        0, 7, 14, 21, 28, 35, 42};
    std::vector<int> moves{};
};

void make_move(Board &board, int col)
{
    uint64_t s = 1;
    uint64_t move = s << board.height[col]++;
    board.masks[board.moves.size() & 1] ^= move;
    board.moves.push_back(col);
}

Board copy_board(const Board &src)
{
    Board dst;
    dst.masks[0] = src.masks[0];
    dst.masks[1] = src.masks[1];
    for (int i = 0; i < COL; i++)
    {
        dst.height[i] = src.height[i];
    }
    dst.moves = src.moves;
    return dst;
}
bool is_game_over(const Board &board)
{
    return board.moves.size() == 42;
}
int is_win(const Board &board)
{
    auto check_win = [](uint64_t mask)
    {
        int directions[] = {1, 7, 6, 8};
        for (int i = 0; i < 4; i++)
        {
            auto second = mask >> directions[i];
            auto third = mask >> (directions[i] * 2);
            auto fourth = mask >> (directions[i] * 3);
            if ((mask & second & third & fourth) != 0)
            {
                return true;
            }
        }
        return false;
    };
    for (int i = 0; i < 2; i++)
    {
        if (check_win(board.masks[i]))
        {
            return i;
        }
    }
    return -1;
}

std::vector<int> get_valid_moves(const Board &board)
{
    std::vector<int> valid_moves{};
    for (int i = 0; i < COL; i++)
    {
        int col = COLUMN_ORDER[i];
        if (board.height[col] <= map[0][col])
        {
            valid_moves.push_back(col);
        }
    }
    return valid_moves;
}

std::array<int, 2> minimax(const Board &board, int depth, bool is_maximizing, int alpha, int beta)
{

    if (is_game_over(board))
    {
        return {0, 0};
    }
    if (is_win(board) != -1)
    {
        if (is_win(board) == 0)
        {
            return {(int)22 - ((int)board.moves.size() + 1) / 2, 0};
        }
        else
        {
            return {
                -22 + (int)board.moves.size() / 2, 0};
        }
    }

    if (depth == 4)
    {
        if (is_maximizing)
        {
            return {(int)22 - ((int)board.moves.size() + 1) / 2, 0};
        }
        else
        {
            return {-22 + (int)board.moves.size() / 2, 0};
        }
    }
    if (is_maximizing)
    {
        int best = -22;
        int best_move = 0;
        for (auto move : get_valid_moves(board))
        {
            Board new_board = copy_board(board);
            make_move(new_board, move);
            auto new_value = minimax(new_board, depth + 1, false, alpha, beta)[0];
            if (new_value > best)
            {
                best = new_value;
                best_move = move;
            }
            // if (best > beta)
            // {
            //     break;
            // }
            alpha = std::max(alpha, best);
        }
        return {best, best_move};
    }
    else
    {
        int best = 22;
        int best_move = 0;
        for (auto move : get_valid_moves(board))
        {
            Board new_board = copy_board(board);
            make_move(new_board, move);
            auto new_value = minimax(new_board, depth + 1, true, alpha, beta)[0];
            if (new_value < best)
            {
                best = new_value;
                best_move = move;
            }
            // if (best < alpha)
            // {
            //     break;
            // }
            beta = std::min(beta, best);
        }
        return {best, best_move};
    }
}

int get_best_move(const Board &board)
{
    auto [best, best_move] = minimax(board, 0, false, -22, 22);
    return best_move;
}

void render(const Board &board)
{
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            if ((board.masks[0] >> map[i][j]) & 1)
            {
                printf("X ");
            }

            else if ((board.masks[1] >> map[i][j]) & 1)
            {
                printf("O ");
            }
            else
            {
                printf(". ");
            }
        }
        printf("\n");
    }
}

void render_test(const Board &board)
{
    auto b = board.masks[0] & (board.masks[0] >> 1) & (board.masks[0] >> 2) & (board.masks[0] >> 3);
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            if ((b >> map[i][j]) & 1)
            {
                printf("X ");
            }

            // else if ((board.masks[1] >> map[i][j]) & 1)
            // {
            //     printf("O ");
            // }
            else
            {
                printf(". ");
            }
        }
        printf("\n");
    }
}

int main()
{
    Board board;
    int move;
    while (!is_game_over(board) && is_win(board) == -1)
    {
        std::cout << "Enter move: ";
        std::cin >> move;
        auto valid_moves = get_valid_moves(board);
        if (move < 0 || move > 6 || std::find(valid_moves.begin(), valid_moves.end(), move) == valid_moves.end())
        {
            printf("Invalid move\n");
            continue;
        }
        make_move(board, move);
        if (is_game_over(board) || is_win(board) != -1)
            break;
        move = get_best_move(board);
        make_move(board, move);
        render(board);
    }
    render(board);

    if (is_win(board) == 0)
    {
        printf("X wins");
    }

    if (is_win(board) == 1)
    {
        printf("O wins");
    }

    if (is_game_over(board))
    {
        printf("Draw");
    }
}