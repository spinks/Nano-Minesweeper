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
  Grid(int r, int c, int m);

 private:
  bool over = false, won = false, mines_created = false, first_display = true;
  int rows = 16, cols = 16, num_flags = 0, num_revealed = 0, num_mines = 40,
      cursor_x = 0, cursor_y = 0;
  std::vector<std::vector<int>> board;
  void GameSequence();
  int Prompt();
  int IntPrompt(int limit);
  void Mines();
  bool Reveal(int x, int y);
  inline void AddDigit(int &value, int digit, int add_value);
  inline int GetDigit(const int &value, int digit);
  void DisplayBoard();
};

int main(int argc, char *argv[]) {
  int r = 16, c = 16, m = 40;
  if (argc == 2) {
    if (!strcmp(argv[1], "easy") || !strcmp(argv[1], "e")) r = c = 9, m = 10;
    if (!strcmp(argv[1], "hard") || !strcmp(argv[1], "h")) c = 30, m = 99;
  }
  if (argc == 4) {
    try {
      r = std::stoi(argv[1]), c = std::stoi(argv[2]), m = std::stoi(argv[3]);
      if ((r <= 0) || (c <= 0))
        throw std::domain_error(std::string("Rows + Cols must be > 0"));
      if (m >= (r * c - 9))
        throw std::domain_error(std::string("Mines must be <= board size -9"));
    } catch (const std::exception &e) {
      std::cerr << e.what() << "\n";
      return 0;
    }
  }
  Grid game(r, c, m);
  return 0;
}

Grid::Grid(int r, int c, int m)
    : rows(r), cols(c), num_mines(m), board(cols, std::vector<int>(rows, 0)) {
  while (!over) GameSequence();
}

void Grid::GameSequence() {
  DisplayBoard();
  int action_reveal = Prompt();
  if (action_reveal == 2) return;
  if (!mines_created && action_reveal) Mines();
  if (!action_reveal) {  // if flag
    int flag_change = (GetDigit(board[cursor_x][cursor_y], 3) ? -1 : 1);
    AddDigit(board[cursor_x][cursor_y], 3, flag_change);
    num_flags += flag_change;
  } else if (!(GetDigit(board[cursor_x][cursor_y], 3)) &&
             !(GetDigit(board[cursor_x][cursor_y], 2))) {
    over = Reveal(cursor_x, cursor_y);
  }
  if (num_revealed == (cols * rows) - num_mines) over = won = true;
  if (over) DisplayBoard();
}

int Grid::Prompt() {
  for (int i = 1; i != -1; i--) {  // process cursor mode
    cout << (i ? "X" : "Y") << " -> ";
    int response = (i ? IntPrompt(cols) : IntPrompt(rows)) - 1;
    if (response == -2) return 2;  // cancel return
    if (response >= 0) (i ? cursor_x : cursor_y) = response;
    DisplayBoard();
  }
  cout << "Action (f = flag, !f = reveal)-> ";
  std::string action;
  std::getline(std::cin, action);
  if (action == "c") return 2;
  return (action == "f" ? 0 : 1);  // 0 - flag 1 - reveal
}

int Grid::IntPrompt(int limit) {
  std::string line;
  int value;
  do {
    std::getline(std::cin, line);
    if (line == "c") return -1;   // cancelled int input
    if (line.empty()) return -2;  // repeated number
    try {
      value = std::stoi(line);
    } catch (...) {
      value = -1;
    }
    if ((value < 1) || (value > limit)) cout << "\033[F\033[5C\033[K";
  } while ((value < 1) || (value > limit));
  return value;
}

void Grid::Mines() {
  if (mines_created) return;
  mines_created = true;
  std::vector<std::pair<int, int>> p_locs;
  for (int x = 0; x != cols; x++) {
    for (int y = 0; y != rows; y++) {
      if (!((x >= cursor_x - 1) && (x <= cursor_x + 1) && (y >= cursor_y - 1) &&
            (y <= cursor_y + 1)))
        p_locs.emplace_back(x, y);
    }
  }
  shuffle(p_locs.begin(), p_locs.end(), std::default_random_engine(time(0)));
  for (int i = 0; i != num_mines; i++) {
    const int &x = p_locs.back().first, &y = p_locs.back().second;
    AddDigit(board[x][y], 1, 1);
    for (int x_off = -1; x_off != 2; x_off++) {
      for (int y_off = -1; y_off != 2; y_off++) {
        if ((x + x_off < cols) && (x + x_off >= 0) && (y + y_off < rows) &&
            (y + y_off >= 0))
          AddDigit(board[x + x_off][y + y_off], 0, 1);
      }
    }
    p_locs.pop_back();  // done after as &x &y are pointers
  }
}

bool Grid::Reveal(int x, int y) {  // returns game over status
  if (GetDigit(board[x][y], 1)) return true;
  if (GetDigit(board[x][y], 2)) return false;
  AddDigit(board[x][y], 2, 1);
  num_revealed++;
  if (((GetDigit(board[x][y], 0)) == 0)) {
    for (int x_off = -1; x_off != 2; x_off++) {
      for (int y_off = -1; y_off != 2; y_off++) {
        if ((x + x_off < cols) && (x + x_off >= 0) && (y + y_off < rows) &&
            (y + y_off >= 0)) {
          Reveal(x + x_off, y + y_off);
        }
      }
    }
  }
  return false;
}

inline void Grid::AddDigit(int &value, int digit, int add_value) {
  value += (pow(10, digit) * add_value);
}

inline int Grid::GetDigit(const int &value, int digit) {
  return ((int)(value / pow(10, digit)) % (int)10);  // 0 is ones, 1 is 10s ect
}

void Grid::DisplayBoard() {
  int col_height = std::to_string(cols).length();
  if (!first_display)  // Redraw
    cout << "\033[" + std::to_string(rows + col_height + 2) + "F\033[J";
  if (first_display) first_display = false;
  cout << "\033[1m" << num_mines - num_flags << " / ";  // Header
  cout << (won ? ":)" : (over ? ":(" : ":|")) << "\n";
  for (int s = col_height - 1; s != -1; s--) {  // Col Numbers
    cout << "\t";
    for (int x = 1; x != cols + 1; x++) {
      if (x == cursor_x + 1) cout << "\033[7m";
      cout << (x >= pow(10, s) ? std::to_string(GetDigit(x, s))
                               : (x == cursor_x + 1 ? ("\033[0m ") : (" ")));
      cout << (x == cursor_x + 1 ? "\033[0m\033[1m " : " ");
    }
    cout << "\n";
  }
  for (int y = 0; y != rows; y++) {  // Row Numbers + Board
    if (y == cursor_y) cout << "\033[7m";
    cout << "\033[1m" << y + 1 << "\033[0m\t";
    for (int x = 0; x != cols; x++) {
      if ((y == cursor_y) && (x == cursor_x)) cout << "\033[7m";  // highlight
      bool flag = GetDigit(board[x][y], 3), revealed = GetDigit(board[x][y], 2),
           mine = GetDigit(board[x][y], 1), non_zero = GetDigit(board[x][y], 0);
      char cell = (non_zero ? (GetDigit(board[x][y], 0) + '0') : ' ');
      if (!revealed) cell = (over ? (mine ? (won ? '+' : '*') : '-') : '-');
      if (flag) cell = (over ? (mine ? '+' : 'x') : '+');
      cout << cell << (((y == cursor_y) && (x == cursor_x)) ? "\033[0m " : " ");
    }
    cout << "\n";
  }
}
