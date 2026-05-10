/*
Name:    main.cpp
Title:   Battleship Solver (Variant 12)
Group:   TV-52
Student: Opanasenko Yu. Yu.
Written: 2026-05-10
Revised: 2026-05-10

<Header>-
Description: This program solves the Battleship puzzle for a 10x10 grid 
using a backtracking algorithm with heuristic pruning. It ensures that 
ships do not touch each other and satisfy row/column constraints.
</Header>-*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace std;

const int GRID_SIZE = 10;

class battleship_solver {
private:
    int grid[GRID_SIZE][GRID_SIZE];

    int top_w[GRID_SIZE] = {8, 3, 7, 6, 4, 7, 5, 9, 5, 10};
    int left_w[GRID_SIZE] = {3, 10, 6, 10, 4, 8, 6, 5, 8, 5};
    int right_b[GRID_SIZE] = {4, 0, 3, 0, 1, 1, 1, 1, 1, 2};
    int bottom_b[GRID_SIZE] = {1, 2, 1, 1, 1, 1, 1, 1, 3, 0};

    int ships[10] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};

    struct ship_position {
        int r;
        int c;
    } last_pos[10];

    /*
    Function: can_place
    Synopsis: Checks if a ship of a given length can be placed at (r, c) 
    without violating grid boundaries or touching other ships.
    */
    bool can_place(int r, int c, int len, bool horiz) {
        if (horiz) {
            if (c + len > GRID_SIZE) {
                return false;
            }
            for (int i = 0; i < len; ++i) {
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + dr;
                        int nc = c + i + dc;
                        if (nr >= 0 && nr < GRID_SIZE && nc >= 0 && nc < GRID_SIZE && grid[nr][nc] == 1) {
                            return false;
                        }
                    }
                }
            }
        } else {
            if (r + len > GRID_SIZE) {
                return false;
            }
            for (int i = 0; i < len; ++i) {
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + i + dr;
                        int nc = c + dc;
                        if (nr >= 0 && nr < GRID_SIZE && nc >= 0 && nc < GRID_SIZE && grid[nr][nc] == 1) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    /*
    Function: place_ship
    Synopsis: Updates the grid by placing (val=1) or removing (val=0) 
    a ship of a specific length and orientation.
    */
    void place_ship(int r, int c, int len, bool horiz, int val) {
        for (int i = 0; i < len; ++i) {
            if (horiz) {
                grid[r][c + i] = val;
            } else {
                grid[r + i][c] = val;
            }
        }
    }

    /*
    Function: early_prune
    Synopsis: Analyzes the partial state of the grid to detect if 
    any row or column constraints are already violated.
    */
    bool early_prune(int next_ship_idx) {
        int rows_to_fix = 0;
        for (int i = 0; i < GRID_SIZE; ++i) {
            int max_b = 0;
            int cur_b = 0;
            int max_w = 0;
            int cur_w = 0;
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (grid[i][j] == 1) {
                    cur_b++;
                    max_b = max(max_b, cur_b);
                    cur_w = 0;
                } else {
                    cur_w++;
                    max_w = max(max_w, cur_w);
                    cur_b = 0;
                }
            }
            
            if (right_b[i] != -1 && max_b > right_b[i]) {
                return false;
            }
            
            if (left_w[i] != -1 && max_w < left_w[i]) {
                return false; 
            } else if (left_w[i] != -1 && max_w > left_w[i]) {
                rows_to_fix++; 
            } else {
            }
        }

        int cols_to_fix = 0;
        for (int j = 0; j < GRID_SIZE; ++j) {
            int max_b = 0;
            int cur_b = 0;
            int max_w = 0;
            int cur_w = 0;
            for (int i = 0; i < GRID_SIZE; ++i) {
                if (grid[i][j] == 1) {
                    cur_b++;
                    max_b = max(max_b, cur_b);
                    cur_w = 0;
                } else {
                    cur_w++;
                    max_w = max(max_w, cur_w);
                    cur_b = 0;
                }
            }
            
            if (bottom_b[j] != -1 && max_b > bottom_b[j]) {
                return false;
            }
            
            if (top_w[j] != -1 && max_w < top_w[j]) {
                return false;
            } else if (top_w[j] != -1 && max_w > top_w[j]) {
                cols_to_fix++;
            } else {
            }
        }

        int cells_left = 0;
        for (int i = next_ship_idx; i < 10; ++i) {
            cells_left += ships[i];
        }
        if (cells_left < max(rows_to_fix, cols_to_fix)) {
            return false;
        }
        return true;
    }

    /*
    Function: check_final_constraints
    Synopsis: Performs a strict validation of the final grid 
    against all row and column constraints.
    */
    bool check_final_constraints() {
        for (int i = 0; i < GRID_SIZE; ++i) {
            int max_w = 0;
            int cur_w = 0;
            int max_b = 0;
            int cur_b = 0;
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (grid[i][j] == 0) {
                    cur_w++;
                    max_w = max(max_w, cur_w);
                    cur_b = 0;
                } else {
                    cur_b++;
                    max_b = max(max_b, cur_b);
                    cur_w = 0;
                }
            }
            if (left_w[i] != -1 && max_w != left_w[i]) {
                return false;
            }
            if (right_b[i] != -1 && max_b != right_b[i]) {
                return false;
            }
        }
        
        for (int j = 0; j < GRID_SIZE; ++j) {
            int max_w = 0;
            int cur_w = 0;
            int max_b = 0;
            int cur_b = 0;
            for (int i = 0; i < GRID_SIZE; ++i) {
                if (grid[i][j] == 0) {
                    cur_w++;
                    max_w = max(max_w, cur_w);
                    cur_b = 0;
                } else {
                    cur_b++;
                    max_b = max(max_b, cur_b);
                    cur_w = 0;
                }
            }
            if (top_w[j] != -1 && max_w != top_w[j]) {
                return false;
            }
            if (bottom_b[j] != -1 && max_b != bottom_b[j]) {
                return false;
            }
        }
        return true;
    }

    /*
    Function: solve_recursive
    Synopsis: The core backtracking method that attempts to place 
    ships one by one using recursion.
    */
    bool solve_recursive(int ship_idx) {
        if (ship_idx == 10) {
            return check_final_constraints();
        }

        int len = ships[ship_idx];
        int start_idx = 0;

        if (ship_idx > 0 && ships[ship_idx] == ships[ship_idx - 1]) {
            start_idx = last_pos[ship_idx - 1].r * GRID_SIZE + last_pos[ship_idx - 1].c + 1;
        }

        for (int idx = start_idx; idx < GRID_SIZE * GRID_SIZE; ++idx) {
            int r = idx / GRID_SIZE;
            int c = idx % GRID_SIZE;

            if (can_place(r, c, len, true)) {
                place_ship(r, c, len, true, 1);
                last_pos[ship_idx].r = r;
                last_pos[ship_idx].c = c;
                
                if (early_prune(ship_idx + 1)) {
                    if (solve_recursive(ship_idx + 1)) {
                        return true;
                    }
                }
                place_ship(r, c, len, true, 0);
            }

            if (len > 1) {
                if (can_place(r, c, len, false)) {
                    place_ship(r, c, len, false, 1);
                    last_pos[ship_idx].r = r;
                    last_pos[ship_idx].c = c;
                    
                    if (early_prune(ship_idx + 1)) {
                        if (solve_recursive(ship_idx + 1)) {
                            return true;
                        }
                    }
                    place_ship(r, c, len, false, 0);
                }
            }
        }
        return false;
    }

public:
    /*
    Function: battleship_solver (Constructor)
    Synopsis: Initializes the 10x10 grid with zeros (empty cells).
    */
    battleship_solver() {
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                grid[i][j] = 0;
            }
        }
    }

    /*
    Function: solve
    Synopsis: Public entry point to start the solving process and print results.
    */
    void solve() {
        cout << "Analysis of 10x10 variant..." << endl;
        if (solve_recursive(0)) {
            print_grid();
        } else {
            cout << "Solution not found." << endl;
        }
    }

    /*
    Function: print_grid
    Synopsis: Outputs the final fleet arrangement to the console.
    */
    void print_grid() {
        cout << "\nFound fleet arrangement:\n";
        cout << "    0 1 2 3 4 5 6 7 8 9\n";
        cout << "  +" << string(GRID_SIZE * 2 + 1, '-') << "+\n";
        for (int i = 0; i < GRID_SIZE; ++i) {
            cout << i << " | ";
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (grid[i][j] == 1) {
                    cout << "\u25a0 ";
                } else {
                    cout << ". ";
                }
            }
            cout << "|\n";
        }
        cout << "  +" << string(GRID_SIZE * 2 + 1, '-') << "+\n";
        cout << "\nBacktracking algorithm successfully solved the task!\n";
    }
};

/*
Function: main
Synopsis: Sets console encoding and initializes the battleship solver.
*/
int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    battleship_solver game;
    game.solve();
    
    return 0;
}
