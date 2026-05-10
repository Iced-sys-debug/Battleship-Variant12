#include <iostream>
#include <vector>
#include <algorithm>

#ifndef NOMINMAX
#define NOMINMAX 
#endif

#include <windows.h>

using namespace std;

class BattleshipSolver {
private:
    static const int N = 10; 
    int grid[N][N];
    
    int top_w[N]  = {8, 3, 7, 6, 4, 7, 5, 9, 5, 10};
    int left_w[N] = {3, 10, 6, 10, 4, 8, 6, 5, 8, 5};
    int right_b[N]  = {4, 0, 3, 0, 1, 1, 1, 1, 1, 2};
    int bottom_b[N] = {1, 2, 1, 1, 1, 1, 1, 1, 3, 0};

    int ships[10] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    struct Pos { int r, c; } last_pos[10]; 

    bool canPlace(int r, int c, int len, bool horiz) {
        if (horiz) {
            if (c + len > N) return false;
            for (int i = 0; i < len; ++i) {
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + dr, nc = c + i + dc;
                        if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                            if (grid[nr][nc] == 1) return false; 
                        }
                    }
                }
            }
        } else {
            if (r + len > N) return false;
            for (int i = 0; i < len; ++i) {
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + i + dr, nc = c + dc;
                        if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                            if (grid[nr][nc] == 1) return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    void placeShip(int r, int c, int len, bool horiz, int val) {
        for (int i = 0; i < len; ++i) {
            if (horiz) grid[r][c + i] = val;
            else grid[r + i][c] = val;
        }
    }

    bool earlyPrune(int next_ship_idx) {
        int rows_to_fix = 0;
        for (int i = 0; i < N; ++i) {
            int max_b = 0, cur_b = 0, max_w = 0, cur_w = 0; 
            for (int j = 0; j < N; ++j) {
                if (grid[i][j] == 1) { cur_b++; max_b = max(max_b, cur_b); } else { cur_b = 0; }
                if (grid[i][j] == 0) { cur_w++; max_w = max(max_w, cur_w); } else { cur_w = 0; }
            }
            if (right_b[i] != -1 && max_b > right_b[i]) return false;
            if (left_w[i] != -1 && max_w < left_w[i]) return false;
            if (left_w[i] != -1 && max_w > left_w[i]) rows_to_fix++;
        }

        int cols_to_fix = 0;
        for (int j = 0; j < N; ++j) {
            int max_b = 0, cur_b = 0, max_w = 0, cur_w = 0;
            for (int i = 0; i < N; ++i) {
                if (grid[i][j] == 1) { cur_b++; max_b = max(max_b, cur_b); } else { cur_b = 0; }
                if (grid[i][j] == 0) { cur_w++; max_w = max(max_w, cur_w); } else { cur_w = 0; }
            }
            if (bottom_b[j] != -1 && max_b > bottom_b[j]) return false;
            if (top_w[j] != -1 && max_w < top_w[j]) return false;
            if (top_w[j] != -1 && max_w > top_w[j]) cols_to_fix++;
        }

        int cells_left = 0;
        for (int i = next_ship_idx; i < 10; ++i) cells_left += ships[i];
        if (cells_left < max(rows_to_fix, cols_to_fix)) return false;

        return true;
    }

    bool checkFinalConstraints() {
        for (int i = 0; i < N; ++i) {
            int max_w = 0, cur_w = 0, max_b = 0, cur_b = 0;
            for (int j = 0; j < N; ++j) {
                if (grid[i][j] == 0) { cur_w++; max_w = max(max_w, cur_w); cur_b = 0; } 
                else { cur_b++; max_b = max(max_b, cur_b); cur_w = 0; }
            }
            if (left_w[i] != -1 && max_w != left_w[i]) return false;
            if (right_b[i] != -1 && max_b != right_b[i]) return false;
        }
        for (int j = 0; j < N; ++j) {
            int max_w = 0, cur_w = 0, max_b = 0, cur_b = 0;
            for (int i = 0; i < N; ++i) {
                if (grid[i][j] == 0) { cur_w++; max_w = max(max_w, cur_w); cur_b = 0; } 
                else { cur_b++; max_b = max(max_b, cur_b); cur_w = 0; }
            }
            if (top_w[j] != -1 && max_w != top_w[j]) return false;
            if (bottom_b[j] != -1 && max_b != bottom_b[j]) return false;
        }
        return true;
    }

    bool solveRecursive(int ship_idx) {
        if (ship_idx == 10) return checkFinalConstraints();

        int len = ships[ship_idx];
        int start_idx = 0;
        
        if (ship_idx > 0 && ships[ship_idx] == ships[ship_idx - 1]) {
            start_idx = last_pos[ship_idx - 1].r * N + last_pos[ship_idx - 1].c + 1;
        }

        for (int idx = start_idx; idx < N * N; ++idx) {
            int r = idx / N;
            int c = idx % N;

            if (canPlace(r, c, len, true)) {
                placeShip(r, c, len, true, 1);
                last_pos[ship_idx] = {r, c};
                if (earlyPrune(ship_idx + 1) && solveRecursive(ship_idx + 1)) return true;
                placeShip(r, c, len, true, 0); 
            }
            
            if (len > 1 && canPlace(r, c, len, false)) {
                placeShip(r, c, len, false, 1);
                last_pos[ship_idx] = {r, c};
                if (earlyPrune(ship_idx + 1) && solveRecursive(ship_idx + 1)) return true;
                placeShip(r, c, len, false, 0); 
            }
        }
        return false;
    }

public:
    BattleshipSolver() {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                grid[i][j] = 0; 
    }

    void solve() {
        cout << "Аналіз варіанту 10х10..." << endl;
        if (solveRecursive(0)) {
            printGrid();
        } else {
            cout << "Рішення не знайдено." << endl;
        }
    }

    void printGrid() {
        cout << "\nЗнайдене розташування флотилії:\n";
        cout << "    0 1 2 3 4 5 6 7 8 9\n";
        cout << "  +" << string(N * 2 + 1, '-') << "+\n";
        
        for (int i = 0; i < N; ++i) {
            cout << i << " | ";
            for (int j = 0; j < N; ++j) {
                if (grid[i][j] == 1) cout << "■ "; 
                else cout << ". ";               
            }
            cout << "|\n";
        }
        cout << "  +" << string(N * 2 + 1, '-') << "+\n";
        cout << "\nАлгоритм Backtracking успішно розв'язав задачу!\n";
    }
};

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    BattleshipSolver game;
    game.solve();

    return 0;
}