#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using std::cout;

class Grid {
 public:
  Grid();

 private:
  bool game_over = false, game_won = false, mines_created = false,
       first_display = true;
  int board_size = 16, num_flags = 0, num_revealed = 0, num_mines = 40,
      cursor_x = 0, cursor_y = 0;
  std::vector<std::vector<int>> board;
  void GameSequence();
  int Prompt();
  int IntPrompt();
  void Mines(bool protect);
  void Reveal(int x, int y, bool flood);
  inline void AddDigit(int &value, int digit, int add_value);
  inline int GetDigit(const int &value, int digit);
  void DisplayBoard();
};

int main() {
  Grid game;
  return 0;
}

Grid::Grid() : board(board_size, std::vector<int>(board_size, 0)) {
  do {
    GameSequence();
  } while (!game_over);
}

void Grid::GameSequence() {
  DisplayBoard();
  int reveal = Prompt();
  if (reveal != 2) {  // if not cancel
    if (!mines_created) (reveal ? Mines(true) : Mines(false));
    if (!reveal) {  // if flag
      int flag_change = (GetDigit(board[cursor_x][cursor_y], 3) ? -1 : 1);
      AddDigit(board[cursor_x][cursor_y], 3, flag_change);
      num_flags += flag_change;
    } else if (!(GetDigit(board[cursor_x][cursor_y], 3)) &&
               !(GetDigit(board[cursor_x][cursor_y], 2))) {
      Reveal(cursor_x, cursor_y, true);
    }
    if (num_revealed == (board_size * board_size) - num_mines) {
      game_over = game_won = true;
    }
  }
  if (game_over) DisplayBoard();
}

int Grid::Prompt() {
  // Process cursor move and action
  // Returns 0 - Flag, 1 - Reveal, 2 - Cancel
  cout << "x -> ";
  int response = IntPrompt();
  if (response == -1) return 2;
  cursor_x = response - 1;
  DisplayBoard();

  cout << "y -> ";
  response = IntPrompt();
  if (response == -1) return 2;
  cursor_y = response - 1;
  DisplayBoard();

  cout << "Action (f = flag, !f = reveal)-> ";
  std::string action;
  std::getline(std::cin, action);
  if (action == "c") return 2;
  return (action == "f" ? 0 : 1);
}

int Grid::IntPrompt() {
  std::string line;
  int value;
  do {
    std::getline(std::cin, line);
    if (line == "c") return -1;  // cancelled int input
    try {
      value = stoi(line);
    } catch (...) {
      value = -1;
    }
    if ((value < 1) || (value > board_size)) {
      cout << "\033[F\033[5C\033[K";
    }
  } while ((value < 1) || (value > board_size));
  return value;
}

void Grid::Mines(bool protect) {
  mines_created = true;
  std::vector<std::pair<int, int>> locations;
  for (int x = 0; x != board_size; x++) {
    for (int y = 0; y != board_size; y++) {
      if (protect) {
        if (!((x >= cursor_x - 1) && (x <= cursor_x + 1) &&
              (y >= cursor_y - 1) && (y <= cursor_y + 1))) {
          locations.emplace_back(x, y);
        }
      } else {
        locations.emplace_back(x, y);
      }
    }
  }
  shuffle(locations.begin(), locations.end(),
          std::default_random_engine(time(0)));
  for (int i = 0; i != num_mines; i++) {
    const int &x = locations.back().first, &y = locations.back().second;
    AddDigit(board[x][y], 1, 1);
    for (int x_off = -1; x_off != 2; x_off++) {
      for (int y_off = -1; y_off != 2; y_off++) {
        if ((x + x_off < board_size) && (x + x_off >= 0) &&
            (y + y_off < board_size) && (y + y_off >= 0)) {
          AddDigit(board[x + x_off][y + y_off], 0, 1);
        }
      }
    }
    locations.pop_back();
  }
}

void Grid::Reveal(int x, int y, bool flood) {
  if (GetDigit(board[x][y], 2)) return;
  AddDigit(board[x][y], 2, 1);
  num_revealed++;
  if ((GetDigit(board[x][y], 1)) && flood) {  // if pressed on mine
    game_over = true;
    for (int i = 0; i != board_size; i++) {
      for (int j = 0; j != board_size; j++) {
        Reveal(i, j, false);
      }
    }
  }
  if (((GetDigit(board[x][y], 0)) == 0) && flood) {
    for (int x_off = -1; x_off != 2; x_off++) {
      for (int y_off = -1; y_off != 2; y_off++) {
        if ((x + x_off < board_size) && (x + x_off >= 0) &&
            (y + y_off < board_size) && (y + y_off >= 0)) {
          Reveal(x + x_off, y + y_off, true);
        }
      }
    }
  }
}

inline void Grid::AddDigit(int &value, int digit, int add_value) {
  value += (pow(10, digit) * add_value);
}

inline int Grid::GetDigit(const int &value, int digit) {
  // 0 is ones digit, 1 is 10s ect
  return ((int)(value / pow(10, digit)) % (int)10);
}

void Grid::DisplayBoard() {
  int stack_height = std::to_string(board_size).length();
  if (!first_display) {  // Redraw
    int rows_up = board_size + stack_height + 3;
    for (int i = 0; i != rows_up; i++) {
      cout << "\033[F\33[2K";
    }
  }
  first_display = false;

  // Header
  cout << "Mines: " << num_mines - num_flags;
  if (game_won) {
    cout << "\t :)\n";
  } else if (game_over) {
    cout << "\t :(\n";
  } else {
    cout << "\t :|\n";
  }

  // Col headers
  for (int s = stack_height; s != 0; s--) {
    int magnitude = pow(10, (s - 1));
    if (magnitude == 1) cout << "Ix.";
    cout << "\t";
    for (int x = 1; x != board_size + 1; x++) {
      if (x == cursor_x + 1) cout << "\033[7m";
      if (x >= magnitude) {
        if (magnitude == 1) {
          cout << x % 10;
        } else {
          cout << (x / magnitude) % magnitude;
        }
      } else {
        if (x == cursor_x + 1) cout << "\033[0m";
        cout << " ";  // empty space if below magnitude
      }
      if (x == cursor_x + 1) cout << "\033[0m";
      cout << " ";  // col seperator
    }
    cout << "\n";
  }

  for (int y = 0; y != board_size; y++) {
    if (y == cursor_y) cout << "\033[7m";
    cout << y + 1 << "\t";
    if (y == cursor_y) cout << "\033[0m";
    for (int x = 0; x != board_size; x++) {
      if ((y == cursor_y) && (x == cursor_x)) cout << "\033[7m";  // highlight
      if (GetDigit(board[x][y], 3)) {
        cout << "+";
      } else if (!(GetDigit(board[x][y], 2))) {
        cout << "-";
      } else if (GetDigit(board[x][y], 1)) {
        cout << "*";
      } else if (GetDigit(board[x][y], 0)) {
        cout << GetDigit(board[x][y], 0);
      } else {
        cout << " ";
      }
      if ((y == cursor_y) && (x == cursor_x)) cout << "\033[0m";  // clear
      cout << " ";
    }
    cout << "\n";
  }
  cout << "\n";
}
