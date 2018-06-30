#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using std::cin;
using std::cout;
using std::default_random_engine;
using std::pair;
using std::string;
using std::vector;

class Grid {
public:
  Grid();

private:
  bool game_over = false, game_won = false, mines_created = false,
       first_display = true;
  int board_size = 16, num_flags = 0, num_revealed = 0, num_mines = 40;
  int cursor_x = 0, cursor_y = 0;
  vector<vector<int>> board;
  void GameSequence();
  bool Prompt();
  int IntPrompt();
  void Mines(bool protect);
  void Reveal(int x, int y);
  inline void AddDigit(int &value, int digit, int add_value);
  inline int GetDigit(const int &value, int digit);
  void DisplayBoard();
};

int main() {
  Grid game;
  return 0;
}

Grid::Grid() : board(board_size, vector<int>(board_size, 0)) { GameSequence(); }

void Grid::GameSequence() {
  DisplayBoard();
  bool reveal = Prompt();
  // create mines on first turn
  if (!mines_created) {
    if (reveal) {
      Mines(true);
    } else {
      Mines(false);
    }
  }
  if (!reveal) {
    if (!(GetDigit(board[cursor_x][cursor_y], 3))) {
      AddDigit(board[cursor_x][cursor_y], 3, 1);
      num_flags++;
    } else {
      AddDigit(board[cursor_x][cursor_y], 3, -1);
      num_flags--;
    }
  } else if (!(GetDigit(board[cursor_x][cursor_y], 3)) &&
             !(GetDigit(board[cursor_x][cursor_y], 2))) {
    Reveal(cursor_x, cursor_y);
  }
  if (num_revealed == (board_size * board_size) - num_mines) {
    game_over = true;
    game_won = true;
  }
  DisplayBoard();
  if (game_over) {
    return;
  } else {
    GameSequence();
  }
}

bool Grid::Prompt() {
  // change x cursor location and redraw
  cout << "x -> ";
  cursor_x = IntPrompt() - 1;
  DisplayBoard();

  // change y cursor location and redraw
  cout << "y -> ";
  cursor_y = IntPrompt() - 1;
  DisplayBoard();

  // get action and return to gamesequence
  cout << "Action (f = flag, !f = reveal)-> ";
  char action;
  cin >> action;
  cout << "\n";
  if (action == 'f') {
    return false;
  } else {
    return true;
  }
}

int Grid::IntPrompt() {
  int value;
  do {
    while (!(cin >> value)) {
      cin.clear();
      cin.ignore(256, '\n');
    }
  } while ((value < 0) || (value > 16));
  return value;
}

void Grid::Mines(bool protect) {
  mines_created = true;
  vector<pair<int, int>> possible_locations;
  for (int x = 0; x != board_size; x++) {
    for (int y = 0; y != board_size; y++) {
      if (protect) {
        if (!((x >= cursor_x - 1) && (x <= cursor_x + 1) &&
              (y >= cursor_y - 1) && (y <= cursor_y + 1))) {
          possible_locations.emplace_back(x, y);
        }
      } else {
        possible_locations.emplace_back(x, y);
      }
    }
  }
  auto rng = default_random_engine(time(0));
  shuffle(possible_locations.begin(), possible_locations.end(), rng);
  for (int i = 0; i != num_mines; i++) {
    const int &x = possible_locations.back().first,
              &y = possible_locations.back().second;
    AddDigit(board[x][y], 1, 1);
    for (int x_off = -1; x_off != 2; x_off++) {
      for (int y_off = -1; y_off != 2; y_off++) {
        if ((x + x_off < board_size) && (x + x_off >= 0) &&
            (y + y_off < board_size) && (y + y_off >= 0)) {
          AddDigit(board[x + x_off][y + y_off], 0, 1);
        }
      }
    }
    possible_locations.pop_back();
  }
}

void Grid::Reveal(int x, int y) {
  if (!(GetDigit(board[x][y], 2))) {
    AddDigit(board[x][y], 2, 1);
    num_revealed++;
    if (!(GetDigit(board[x][y], 0))) {
      for (int x_off = -1; x_off != 2; x_off++) {
        for (int y_off = -1; y_off != 2; y_off++) {
          if ((x + x_off < board_size) && (x + x_off >= 0) &&
              (y + y_off < board_size) && (y + y_off >= 0)) {
            Reveal(x + x_off, y + y_off);
          }
        }
      }
    }
    if (GetDigit(board[x][y], 1)) {
      game_over = true;
      for (int i = 0; i != board_size; i++) {
        for (int j = 0; j != board_size; j++) {
          Reveal(i, j);
        }
      }
    }
  }
}

inline void Grid::AddDigit(int &value, int digit, int add_value) {
  value += (pow(10, (digit)) * add_value);
}

inline int Grid::GetDigit(const int &value, int digit) {
  // 0 is ones digit, 1 is 10s ect
  return ((int)(value / pow(10, digit)) % (int)10);
}

void Grid::DisplayBoard() {
  // Redraw
  int stack_height = std::to_string(board_size).length();
  int rows_up = board_size + stack_height + 3;
  if (!first_display) {
    for (int i = 0; i != rows_up; i++) {
      cout << "\033[F\33[2K";
    }
  }
  first_display = false;

  // Header
  cout << "Mines: " << num_mines - num_flags;
  if (game_won) {
    cout << "\t :)";
  } else if (game_over) {
    cout << "\t :(";
  } else {
    cout << "\t :|";
  }
  cout << "\n";

  // Col headers
  for (int s = stack_height; s != 0; s--) {
    int magnitude = pow(10, (s - 1));
    if (magnitude == 1) {
      cout << "Ix.";
    }
    cout << "\t";
    for (int x = 1; x != board_size + 1; x++) {
      if (x == cursor_x + 1) {
        cout << "\033[7m";
      }
      if ((x) >= magnitude) {
        if (magnitude == 1) {
          cout << x % 10;
        } else {
          cout << (x / magnitude) % magnitude;
        }
      } else {
        cout << "\033[0m"
             << " ";
      }
      if (x == cursor_x + 1) {
        cout << "\033[0m";
      }
      // col seperator
      cout << " ";
    }
    cout << "\n";
  }

  for (int y = 1; y != board_size + 1; y++) {
    if (y == cursor_y + 1) {
      cout << "\033[7m" << y << "\033[0m\t";
    } else {
      cout << y << "\t";
    }
    for (int x = 1; x != board_size + 1; x++) {
      if ((y == cursor_y + 1) && (x == cursor_x + 1)) {
        cout << "\033[7m";
      }
      if (GetDigit(board[x - 1][y - 1], 3)) {
        cout << "+";
      } else if (!(GetDigit(board[x - 1][y - 1], 2))) {
        cout << "-";
      } else if (GetDigit(board[x - 1][y - 1], 1)) {
        cout << "*";
      } else if (GetDigit(board[x - 1][y - 1], 0)) {
        cout << GetDigit(board[x - 1][y - 1], 0);
      } else {
        cout << " ";
      }
      if ((y == cursor_y + 1) && (x == cursor_x + 1)) {
        cout << "\033[0m";
      }
      cout << " ";
    }
    cout << "\n";
  }
  cout << "\n";
}
