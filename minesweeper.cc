#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>

class Grid {
  bool over = false, won = false, mines_created = false, first_display = true,
       Reveal(int x, int y);
  int rows = 16, cols = 16, num_flags = 0, num_revealed = 0, num_mines = 40,
      c_x = 0, c_y = 0, Prompt(), IntPrompt(int limit), p_int;
  std::vector<std::vector<int>> board;
  std::string rst = "\033[0m", bld = "\033[1m", inv = "\033[7m", p_line;
  void GameSequence(), DisplayBoard(), Mines(bool overflow);
  inline void AddDigit(int &value, int digit, int add_value) {
    value += (pow(10, digit) * add_value);
  };
  inline int GetDigit(const int &value, int digit) {
    return ((int)(value / pow(10, digit)) % (int)10);
  };

 public:
  Grid(int r, int c, int m)
      : rows(r), cols(c), num_mines(m), board(cols, std::vector<int>(rows, 0)) {
    while (!over) DisplayBoard(), GameSequence();
    DisplayBoard();  // display once more at end
  }
};

int main(int argc, char *argv[]) {
  int r = 16, c = 16, m = 40;  // default values (medium difficulty)
  if (argc == 2) {             // handle preset difficulty values
    if (!strcmp(argv[1], "easy") || !strcmp(argv[1], "e")) r = c = 9, m = 10;
    if (!strcmp(argv[1], "hard") || !strcmp(argv[1], "h")) c = 30, m = 99;
  } else if (argc == 4)  // handle custom dimensions
    try {
      r = std::stoi(argv[1]), c = std::stoi(argv[2]), m = std::stoi(argv[3]);
      if (r <= 0 || c <= 0) throw std::domain_error("Rows & Cols must be > 0");
      if (m >= (r * c)) throw std::domain_error("Mines must be < Rows * Cols");
      if (m < 0) throw std::domain_error("Mines must be >= 0");
    } catch (const std::exception &e) {  // will also catch stoi errors
      std::cerr << e.what() << "\n";
      return -1;  // exit program due to malformed input
    }
  Grid game(r, c, m);
}

void Grid::GameSequence() {
  int act = Prompt();
  if (act == 2) return;       // canceled action
  if (!mines_created && act)  // on reveal Mines(true) if requires overflow locs
    (num_mines > (rows * cols - 9)) ? Mines(true) : Mines(false);
  if (!act) num_flags += (GetDigit(board[c_x][c_y], 3) ? -1 : 1);
  if (!act) AddDigit(board[c_x][c_y], 3, GetDigit(board[c_x][c_y], 3) ? -1 : 1);
  if (act && !GetDigit(board[c_x][c_y], 3) && !GetDigit(board[c_x][c_y], 2))
    over = Reveal(c_x, c_y);
  if (num_revealed == (cols * rows) - num_mines) over = won = true;
}

int Grid::Prompt() {
  for (int i = 1; i != -1; i--) {  // process cursor move
    std::cout << bld << (i ? "X" : "Y") << " -> ";
    int response = (i ? IntPrompt(cols) : IntPrompt(rows));
    if (response == -1) return 2;  // cancel return
    if (response >= 1) (i ? c_x : c_y) = response - 1;
    DisplayBoard();
  }
  std::cout << bld << "(f)lag / (c)ancel / (*)reveal -> ";
  std::getline(std::cin, p_line);  // 2 cancel / 1 reveal / 0 flag
  return (p_line == "c" ? 2 : (p_line == "f" ? 0 : 1));
}

int Grid::IntPrompt(int limit) {
  do {
    std::getline(std::cin, p_line);
    if (p_line == "c" || p_line.empty()) return (p_line == "c" ? -1 : -2);
    try {
      p_int = std::stoi(p_line);
    } catch (...) {
      p_int = -1;
    }
    if (p_int < 1 || p_int > limit) std::cout << "\033[F\033[5C\033[K";
  } while (p_int < 1 || p_int > limit);
  return p_int;
}

void Grid::Mines(bool overflow) {
  mines_created = true;
  std::vector<std::pair<int, int>> locs;
  for (int x = 0; x != cols; ++x)
    for (int y = 0; y != rows; ++y)
      if ((overflow && x != c_x && y != c_y) ||
          (x < c_x - 1 || x > c_x + 1 || y < c_y - 1 || y > c_y + 1))
        locs.emplace_back(x, y);
  std::shuffle(locs.begin(), locs.end(), std::random_device());
  for (int i = 0; i != num_mines; ++i) {
    const int &x = locs[i].first, &y = locs[i].second;
    AddDigit(board[x][y], 1, 1);
    for (int o_x = -1; o_x != 2; ++o_x)
      for (int o_y = -1; o_y != 2; ++o_y)
        if (x + o_x < cols && x + o_x >= 0 && y + o_y < rows && y + o_y >= 0)
          AddDigit(board[x + o_x][y + o_y], 0, 1);
  }
}

bool Grid::Reveal(int x, int y) {              // returns game over status
  if (GetDigit(board[x][y], 2)) return false;  // if revealed don't (recursive)
  if (GetDigit(board[x][y], 1)) return true;   // game over if mine
  AddDigit(board[x][y], 2, 1);                 // increment revealed digit
  ++num_revealed;                              // increment revealed count
  if (GetDigit(board[x][y], 0)) return false;  // if adjacent mines dont recurse
  for (int o_x = -1; o_x != 2; ++o_x)
    for (int o_y = -1; o_y != 2; ++o_y)
      if (x + o_x < cols && x + o_x >= 0 && y + o_y < rows && y + o_y >= 0)
        Reveal(x + o_x, y + o_y);
  return false;
}

void Grid::DisplayBoard() {
  int col_height = log10(cols) + 1, row_len = log10(rows) + 1;
  if (!first_display)  // Redraw (2 is header + prompt line)
    std::cout << "\033[" + std::to_string(rows + col_height + 2) + "F\033[J";
  if (first_display) first_display = false;
  std::cout << bld << num_mines - num_flags << " / "
            << (won ? ":)" : (over ? ":(" : ":|")) << "\n";
  for (int s = col_height - 1; s != -1; --s) {  // Col Numbers
    for (int i = 0; i != row_len + 1; ++i) std::cout << " ";
    for (int x = 1; x != cols + 1; ++x)
      std::cout << (x == c_x + 1 ? inv : "")
                << (x >= pow(10, s) ? std::to_string(GetDigit(x, s))
                                    : (x == c_x + 1 ? rst + " " : " "))
                << (x == c_x + 1 ? rst + bld : "") << " ";
    std::cout << "\n";
  }
  for (int y = 0; y != rows; ++y) {  // Row Numbers + Board
    std::string ws(row_len - (int)log10(y + 1) - 1, ' ');
    std::cout << ws << (y == c_y ? inv : "") << bld << y + 1 << rst << " ";
    for (int x = 0; x != cols; ++x) {  // cells
      int flag = GetDigit(board[x][y], 3), revealed = GetDigit(board[x][y], 2),
          mine = GetDigit(board[x][y], 1), nzero = GetDigit(board[x][y], 0);
      std::string cell = nzero ? std::to_string(nzero) : " ";
      if (!revealed) cell = (over ? (mine ? (won ? "+" : "*") : "-") : "-");
      if (flag) cell = (over ? (mine ? "+" : "x") : "+");
      std::cout << ((y == c_y && x == c_x) ? inv + cell + rst : cell) << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n\033[F";
}
