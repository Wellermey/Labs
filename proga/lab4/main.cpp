#include "structures.h"
#include <fstream>
#include <math.h>
#include <ncurses.h>

void render(int selected) {
  erase();
  const char messages[6][100] = {"1) Add",    "2) Remove", "3) Display",
                                 "4) Search", "5) Save",   "6) Load"};
  for (int i = 0; i < 6; i++)
    if (i == selected) {
      attron(COLOR_PAIR(2));
      mvprintw(1 + 2 * i, 2, "%s", messages[i]);
      attroff(COLOR_PAIR(2));
    } else {

      mvprintw(1 + 2 * i, 2, "%s", messages[i]);
    }
}
void push_back(Item *&head, Item *&tail, Item_data data) {
  Item *item = new Item(data, tail);
  if (tail)
    tail->next = item;
  else
    head = item;
  tail = item;
}

Item_data read_data(int &err, const char *&message, int am[6]) {
  Item_data data;
  err = 0;

  for (int i = 0; i < 6; i++)
    if (!am[i]) {
      err = i + 1;
      message = "Empty";
    }
  char s[20];
  mvinnstr(1, 17, data.name, am[0]);
  data.name[am[0]] = '\0';
  data.weight = 0;
  mvinnstr(3, 17, s, am[1]);
  for (int i = 0; i < am[1]; data.weight = data.weight * 10 + (s[i++] - '0')) {
  }
  data.price = 0;
  mvinnstr(5, 17, s, am[2]);
  for (int i = 0; i < am[2]; data.price += data.price * 10 + (s[i++] - '0')) {
  }
  data.amount = 0;
  mvinnstr(7, 17, s, am[3]);
  for (int i = 0; i < am[3]; data.amount += data.amount * 10 + (s[i++] - '0')) {
  }
  mvinnstr(9, 17, s, am[4]);
  if (am[4] != 8) {
    err = 5;
    message = "Must be dd.mm.yy";
  }
  int d = (s[0] - '0') * 10 + s[1] - '0';
  int m = (s[3] - '0') * 10 + s[4] - '0';
  int y = 2000 + (s[6] - '0') * 10 + s[7] - '0';
  int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
    days[1] = 29;
  if (m < 1 || m > 12 || d < 1 || d > days[m - 1]) {
    err = 5;
    message = "Must be dd.mm.yy";
  }
  data.date = {d, m, y};
  mvinnstr(11, 17, s, am[5]);
  data.discount = s[0] == 'y';

  return data;
}

void read_item(Item *&head, Item *&tail, int &id) {
  erase();
  printw("\n  Name: \n\n  Weight: \n\n  Price: \n\n  Amount: \n\n  Date "
         "dd.mm.yy: \n\n  "
         "Discount y/n:");
  attron(COLOR_PAIR(3));
  for (int i = 1; i < 12; i += 2)
    mvprintw(i, 17, "%*s", 20, "");
  attroff(COLOR_PAIR(3));
  refresh();
  int selected = 0;
  int ch = 'f';
  int am[6] = {0, 0, 0, 0, 0, 0};
  for (;;) {
    for (int i = 0; i < 6; i++)
      mvchgat(1 + 2 * i, 17, 20, A_NORMAL, 3, NULL);
    mvchgat(1 + 2 * selected, 17, 20, A_NORMAL, 4, NULL);
    attron(COLOR_PAIR(3));
    ch = getch();
    switch (ch) {
    case '\n': {
      int err;
      const char *message;
      Item_data data = read_data(err, message, am);
      if (!err) {
        data.id = id++;
        push_back(head, tail, data);
        return;
      }
      attroff(COLOR_PAIR(3));
      attron(COLOR_PAIR(4));

      mvchgat(1 + 2 * (err - 1), 17, 20, A_NORMAL, 5, NULL);

      attroff(COLOR_PAIR(4));
      mvprintw(2 * err, 17, "%s", message);

      refresh();
      while (getch() != '\n') {
      }

      mvprintw(2 * err, 17, "%*s", 20, "");
      attron(COLOR_PAIR(3));

      break;
    }
    case KEY_UP:
      selected = (selected + 5) % 6;
      break;
    case KEY_DOWN:
      selected = (selected + 1) % 6;
      break;
    case KEY_BACKSPACE:
      if (am[selected])
        mvprintw(1 + 2 * selected, 17 + --am[selected], " ");
      break;
    default:
      if (am[selected] > 19)
        break;
      switch (selected) {
      case 0:
        if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
          mvprintw(1 + 2 * selected, 17 + am[selected]++, "%c", ch);
        break;
      case 4:
        if (am[selected] < 8 &&
            ((am[selected] % 3 == 2 && ch == '.') ||
             (am[selected] % 3 < 2 && '0' <= ch && ch <= '9')))
          mvprintw(1 + 2 * selected, 17 + am[selected]++, "%c", ch);
        break;
      case 5:
        if (!am[selected] && (ch == 'y' || ch == 'n'))
          mvprintw(1 + 2 * selected, 17 + am[selected]++, "%c", ch);
        break;
      case 1:
      case 2:
      case 3:
        if ('0' <= ch && ch <= '9')
          mvprintw(1 + 2 * selected, 17 + am[selected]++, "%c", ch);
        break;
      }
    }
    attroff(COLOR_PAIR(3));
    refresh();
  }
}

void remove_item(Item *&head, Item *&tail) {
  erase();
  int c = 0;
  Item *item = head;
  for (; item; item = item->next) {
    c++;
  }
  if (!c) {
    printw("\n  Nothing to remove.");
    getch();
    return;
  }
  int a = 0;
  int ch;
  while (a < 1 || a > c) {
    erase();
    a = 0;
    printw("\n  Enter the number from 1 to %d:", c);
    refresh();
    ch = getch();

    while (ch != '\n') {
      if ('0' <= ch && ch <= '9' && (10 * a + (ch - '0') <= c)) {
        a = 10 * a + (ch - '0');
        mvprintw(1, 32 + std::log10(c) + std::log10(a), "%c", ch);
      }
      ch = getch();
    }
  }

  item = head;
  while (--a)
    item = item->next;

  if (item->last)
    item->last->next = item->next;
  else
    head = item;

  if (item->next)
    item->next->last = item->last;
  else
    tail = item->last;

  delete item;
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
  init_pair(4, COLOR_BLACK, COLOR_CYAN);
  init_pair(5, COLOR_BLACK, COLOR_RED);

  int ch = 'a';
  int selected = 0;
  Item *head = nullptr;
  Item *tail = nullptr;
  int _id = 1;

  while (ch != 'q') {
    render(selected);

    ch = getch();
    switch (ch) {
    case KEY_UP:
      selected = (selected + 5) % 6;
      break;
    case KEY_DOWN:
      selected = (selected + 1) % 6;
      break;
    case '\n':
      switch (selected) {
      case 0:
        read_item(head, tail, _id);
        break;
      case 1:
        remove_item(head, tail);
        break;
      case 2:

        break;
      case 3:

        break;
      case 4:

        break;
      case 5:

        break;
      }
    }
  }

  endwin();
  return 0;
}
