#include <cmath>
#include <math.h>
#include <ncurses.h>

void print(char c) {
  printw("%c", c);
  refresh();
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

void generate(int **&matrix, int n) {
  matrix = new int *[n];
  for (int i = 0; i < n; matrix[i++] = new int[n]{0}) {
  }

  int i = 0, j = 0, k = 1, ri = 0, rj = 1;
  while (k <= n * n) {
    matrix[i][j] = k;
    if (i + ri < 0 || i + ri >= n || j + rj < 0 || j + rj >= n ||
        matrix[i + ri][j + rj]) {
      rj += ri;
      ri = rj - ri;
      rj = ri - rj;
    }
    i += ri;
    j += rj;
    k++;
  }
}

void print_determinant(int a) { mvprintw(6, 2, "Determinant = %d", a); }

double read(const char *message) {
  erase();
  int a = 0;
  int m = 1;
  bool start = true;

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
  const char messages[2][100] = {"1) Generate", "2) Determinant"};
  for (int i = 0; i < 2; i++)
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
  int m = 1, s = 1, res = 1;
  for (int k = 0; k < n - 1; k++) {
    if (a[k][k]) {
      for (int i = k + 1; i < n; i++)
        for (int j = k + 1; j < n; j++)
          a[i][j] = (a[k][k] * a[i][j] - a[i][k] * a[k][j]) / m;
      m = a[k][k];
    } else {
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
  }
  if (res)
    res = s * a[n - 1][n - 1];
  for (int i = 0; i < n; i++)
    delete[] a[i];
  delete[] a;
  return res;
}

int main() {
  initscr();
  curs_set(0);
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  init_pair(2, COLOR_RED, COLOR_BLACK);

  int a = 4;
  int **matrix;

  generate(matrix, a);
  int ch;
  int selected = 0;
  bool det = 0;
  int _determinant = 0;

  while (ch != 'q') {
    render(selected);
    print_matrix(matrix, a);
    if (det)
      print_determinant(_determinant);

    ch = getch();
    switch (ch) {
    case KEY_UP:
      selected = abs(selected - 1) % 2;
      break;
    case KEY_DOWN:
      selected = abs(selected + 1) % 2;
      break;
    case '\n':
      switch (selected) {
      case 0:
        det = 0;
        a = read("Enter the matrix size: ");
        generate(matrix, a);
        break;
      case 1:
        det = 1;
        _determinant = determinant(matrix, a);
        break;
      }
    }
  }

  getch();
  endwin();
  return 0;
}
