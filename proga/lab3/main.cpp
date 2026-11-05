#include <cmath>
#include <ncurses.h>
#include <random>

void print(char c) {
  printw("%c", c);
  refresh();
}

void del_matrix(int **&a, int n) {
  for (int i = 0; i < n; i++)
    delete[] a[i];
  delete[] a;
}

void print_matrix(int **&matrix, int n) {
  int maxs = std::log10(n * n) + 1;
  for (int i = 0; i < 2 * n + 1; i++) {
    mvprintw(1 + i, 29, "|");
    mvprintw(1 + i, 29 + (maxs + 1) * n, "|");
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      mvprintw(2 + 2 * i, 30 + (maxs + 1) * j, "%d", matrix[i][j]);
    }
  }
}

double read2(int i, int j) {
  int a = 0;
  int m = 1;
  int l = 0;
  bool start = 1;
  attron(COLOR_PAIR(4));
  mvprintw(i, j, "%*s", 3, "");
  refresh();
  char c = getch();
  while (start || (c != '\n')) {
    if (start && c == '-') {
      m = -1;
      start = 0;
      mvprintw(i, j + l++, "%c", c);
    } else if ('0' <= c && c <= '9' &&
               (-100 < (10 * abs(a) + (c - '0')) &&
                (10 * a + m * (c - '0') < 1000))) {
      start = 0;
      a = 10 * a + m * (c - '0');
      mvprintw(i, j + l++, "%c", c);
    }
    refresh();
    c = getch();
  }
  attroff(COLOR_PAIR(4));

  attron(COLOR_PAIR(5));
  mvprintw(i, j, "%*d", 3, a);
  refresh();
  attroff(COLOR_PAIR(5));

  return a;
}

void read_matrix(int **&matrix, int n) {
  erase();
  matrix = new int *[n];
  for (int i = 0; i < n; matrix[i++] = new int[n]{0}) {
  }
  int maxs = 3;
  for (int i = 0; i < 2 * n + 1; i++) {
    mvprintw(1 + i, 2, "|");
    mvprintw(1 + i, 2 + (maxs + 1) * n, "|");
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      attron(COLOR_PAIR(3));
      mvprintw(2 + 2 * i, 3 + (maxs + 1) * j, "%*s", maxs, "");
      attroff(COLOR_PAIR(3));
    }
  }
  refresh();

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      matrix[i][j] = read2(2 + 2 * i, 3 + (maxs + 1) * j);
    }
}

void generate(int **&matrix, int n) {
  matrix = new int *[n];
  for (int i = 0; i < n; matrix[i++] = new int[n]{0}) {
  }
  int k = 1, s = 1;
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
      matrix[i][j] = k + s * i;
    }
    s = -s;
    k += (j % 2) ? 1 : 2 * n - 1;
  }
}

void print_determinant(int a) { mvprintw(10, 2, "Determinant = %d", a); }

double read(const char *message) {
  erase();
  int a = 0;
  int m = 1;
  bool start = 0;

  printw("\n  %s", message);

  char c = getch();

  while (start || (c != '\n')) {
    if (start && c == '-') {
      m = -1;
      start = 0;
      print(c);
    } else if ('0' <= c && c <= '9' && (10 * abs(a) + (c - '0') <= 20)) {
      start = 0;
      a = 10 * a + m * (c - '0');
      print(c);
    }
    c = getch();
  }
  print(c);
  return a;
}

void render(int selected) {
  erase();
  const char messages[4][100] = {"1) Generate", "2) Determinant", "3) Enter",
                                 "4) - Random"};
  for (int i = 0; i < 4; i++)
    if (i == selected) {
      attron(COLOR_PAIR(2));
      mvprintw(1 + 2 * i, 2, "%s", messages[i]);
      attroff(COLOR_PAIR(2));
    } else {

      mvprintw(1 + 2 * i, 2, "%s", messages[i]);
    }
}

int determinant(int **matrix, int n) {
  int **a = new int *[n];
  for (int i = 0; i < n; a[i++] = new int[n]{0}) {
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++)
      a[i][j] = matrix[i][j];
  }
  long long m = 1, s = 1, res = 1;
  for (int k = 0; k < n - 1; k++) {
    if (!a[k][k]) {
      int p = k + 1;
      while (p < n && !a[p][k])
        p++;
      if (p == n) {
        res = 0;
        break;
      }
      std::swap(a[k], a[p]);
      s = -s;
    }
    for (int i = k + 1; i < n; i++)
      for (int j = k + 1; j < n; j++)
        a[i][j] = (a[k][k] * a[i][j] - a[i][k] * a[k][j]) / m;
    m = a[k][k];
  }
  if (res)
    res = s * a[n - 1][n - 1];
  del_matrix(a, n);
  return res;
}

int main() {
  initscr();
  curs_set(0);
  noecho();
  // nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_BLACK, COLOR_BLUE);
  init_pair(4, COLOR_WHITE, COLOR_RED);
  init_pair(5, COLOR_BLACK, COLOR_GREEN);

  // random
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 10);

  int a = 4;
  int **matrix;

  generate(matrix, a);
  int ch = 'a';
  int selected = 0;
  bool det = 0;
  long long _determinant = 0;

  while (ch != 'q') {
    render(selected);
    print_matrix(matrix, a);
    if (det)
      print_determinant(_determinant);

    ch = getch();
    switch (ch) {
    case KEY_UP:
      selected = (selected + 3) % 4;
      break;
    case KEY_DOWN:
      selected = (selected + 1) % 4;
      break;
    case '\n':
      switch (selected) {
      case 0:
        del_matrix(matrix, a);
        det = 0;
        a = 0;
        while (a < 4)
          a = read("Enter the matrix size: ");
        generate(matrix, a);
        break;
      case 1:
        det = 1;
        _determinant = determinant(matrix, a);
        break;
      case 2:
        del_matrix(matrix, a);
        det = 0;
        a = 0;
        while (a < 4)
          a = read("Enter the matrix size: ");
        read_matrix(matrix, a);
        break;
      case 3:
        for (int i = 0; i < a; i++)
          for (int j = 0; j < a; j++)
            matrix[i][j] -= dist(gen);
        break;
      }
    }
  }
  del_matrix(matrix, a);
  endwin();
  return 0;
}
