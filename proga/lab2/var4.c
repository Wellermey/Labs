#include <math.h>
#include <ncurses.h>

#define EPS 0.000001
#define func4(x) ((sin(x) + cos(x)) / ((x) * (x) - M_PI * M_PI))
#define my_func4(x) ((my_sin(x) + my_cos(x)) / ((x) * (x) - M_PI * M_PI))

void print(char c) {
  printw("%c", c);
  refresh();
}

double read_double() {
  double a = 0;
  int m = 1;
  bool start = true;
  bool dot = true;
  double after_dot = 10;
  char c = getch();

  while (start || (c != ' ' && c != '\n')) {
    if (start && c == '-') {
      m = -1;
      start = 0;
      print(c);
    } else if (!start && dot && c == '.') {
      dot = 0;
      print(c);
    } else if ('0' <= c && c <= '9') {
      if (dot) {
        start = 0;
        a = 10 * a + m * (c - '0');
      } else {
        start = 0;
        a = a + m * (c - '0') / after_dot;
        after_dot *= 10;
      }
      print(c);
    }
    c = getch();
  }
  print(c);
  return a;
}

double my_sin(double x) {
  x -= floor(x / (2 * M_PI) + 0.5) * 2 * M_PI;
  double res = x;
  double last = x;

  for (int i = 0; i < 6; i++) {
    last = -last * x * x / ((2 * i + 2) * (2 * i + 3));
    res += last;
  }
  return res;
}

double my_cos(double x) {
  x -= floor(x / (2 * M_PI) + 0.5) * 2 * M_PI;
  double res = 1;
  double last = 1;

  for (int i = 0; i < 6; i++) {
    last = -last * x * x / ((2 * i + 1) * (2 * i + 2));
    res += last;
  }
  return res;
}

void integral(double a, double b, int n, double *integral_value,
              double *my_integral_value) {
  double w = (b - a) / n;
  double sum = 0;
  double my_sum = 0;

  for (int i = 0; i < n; i++) {
    sum += func4(a + i * w + 0.5 * w);
    my_sum += my_func4(a + i * w + 0.5 * w);
  }
  sum *= w;
  *integral_value += sum;
  my_sum *= w;
  *my_integral_value += my_sum;
}

int main() {
  initscr();
  cbreak();
  noecho();

  double a, b;
  a = read_double();
  b = read_double();
  if (b < a) {
    a = a + b;
    b = a - b;
    a = a - b;
  }
  double integral_value = 0;
  double my_integral_value = 0;

  double bad[2] = {-M_PI, M_PI};
  double left = a;

  for (int i = 0; i < 2; i++) {
    if (a < bad[i] && bad[i] < b) {
      integral(left, bad[i] - EPS, 100, &integral_value, &my_integral_value);
      left = bad[i] + EPS;
    }
  }
  integral(left, b, 100, &integral_value, &my_integral_value);

  printw("Integral value: %0.2f\n", integral_value);
  printw("My integral value: %0.2f\n", my_integral_value);
  printw("Integral value: %0.10f\n", integral_value);
  printw("My integral value: %0.10f\n", my_integral_value);

  getch();
  endwin();
  return 0;
}
