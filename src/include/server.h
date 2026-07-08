#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

/*
 * You may need to define some global variables for the information of the game map here.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

// Internal state tracking
bool mine_map[35][35];       // true if mine, false otherwise
int visited[35][35];        // 0: unvisited, 1: visited, 2: marked
int mine_counts[35][35];     // precalculated mine counts for each cell

int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

/**
 * @brief The definition of function InitMap()
 */
void InitMap() {
  if (!(std::cin >> rows >> columns)) return;
  total_mines = 0;
  for (int i = 0; i < rows; ++i) {
    std::string row_str;
    std::cin >> row_str;
    for (int j = 0; j < columns; ++j) {
      mine_map[i][j] = (row_str[j] == 'X');
      visited[i][j] = 0;
      if (mine_map[i][j]) total_mines++;
    }
  }

  // Precalculate mine counts
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      int count = 0;
      for (int k = 0; k < 8; ++k) {
        int ni = i + dr[k];
        int nj = j + dc[k];
        if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && mine_map[ni][nj]) {
          count++;
        }
      }
      mine_counts[i][j] = count;
    }
  }
  game_state = 0;
}

/**
 * @brief The definition of function VisitBlock(int, int)
 */
void VisitBlock(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (game_state != 0 || visited[r][c] != 0) return;

  if (mine_map[r][c]) {
    game_state = -1;
    visited[r][c] = 1; // Mark as visited to show 'X' in PrintMap
    return;
  }

  visited[r][c] = 1;
  if (mine_counts[r][c] == 0) {
    for (int k = 0; k < 8; ++k) {
      int ni = r + dr[k];
      int nj = c + dc[k];
      VisitBlock(ni, nj);
    }
  }

  // Check for victory: all non-mine blocks visited
  int non_mine_visited = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (!mine_map[i][j] && visited[i][j] == 1) {
        non_mine_visited++;
      }
    }
  }
  if (non_mine_visited == (rows * columns - total_mines)) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function MarkMine(int, int)
 */
void MarkMine(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (game_state != 0 || visited[r][c] != 0) return;

  if (mine_map[r][c]) {
    visited[r][c] = 2;
  } else {
    game_state = -1;
    visited[r][c] = 2; // Mark as marked to show 'X' in PrintMap
  }
}

/**
 * @brief The definition of function AutoExplore(int, int)
 */
void AutoExplore(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (game_state != 0) return;
  if (visited[r][c] != 1 || mine_map[r][c]) return;

  int marked_around = 0;
  for (int k = 0; k < 8; ++k) {
    int ni = r + dr[k];
    int nj = c + dc[k];
    if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && visited[ni][nj] == 2) {
      marked_around++;
    }
  }

  if (marked_around == mine_counts[r][c]) {
    for (int k = 0; k < 8; ++k) {
      int ni = r + dr[k];
      int nj = c + dc[k];
      if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
        if (!mine_map[ni][nj]) {
          VisitBlock(ni, nj);
        }
      }
    }
  }
}

/**
 * @brief The definition of function ExitGame()
 */
void ExitGame() {
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
  } else {
    std::cout << "GAME OVER!" << std::endl;
  }

  int visit_count = 0;
  int marked_mine_count = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (!mine_map[i][j] && visited[i][j] == 1) visit_count++;
      if (mine_map[i][j] && visited[i][j] == 2) marked_mine_count++;
    }
  }
  
  if (game_state == 1) {
    marked_mine_count = total_mines;
  }

  std::cout << visit_count << " " << marked_mine_count << std::endl;
  exit(0);
}

/**
 * @brief The definition of function PrintMap()
 */
void PrintMap() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (game_state == 1) {
        // Victory: all mines are '@'
        if (mine_map[i][j]) std::cout << '@';
        else std::cout << mine_counts[i][j];
      } else if (visited[i][j] == 0) {
        std::cout << '?';
      } else if (visited[i][j] == 1) {
        if (mine_map[i][j]) std::cout << 'X';
        else std::cout << mine_counts[i][j];
      } else if (visited[i][j] == 2) {
        if (mine_map[i][j]) std::cout << '@';
        else std::cout << 'X';
      }
    }
    std::cout << std::endl;
  }
}

#endif
