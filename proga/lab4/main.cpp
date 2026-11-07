#include "structures.h"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <math.h>
#include <ncurses.h>

#define int long long

void render(int selected, const char messages[6][100]) {
  erase();
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
  char s[30];
  mvinnstr(1, 17, data.name, am[0]);
  data.name[am[0]] = '\0';
  data.weight = 0;
  mvinnstr(3, 17, s, am[1]);
  for (int i = 0; i < am[1]; data.weight = data.weight * 10 + (s[i++] - '0')) {
  }
  data.price = 0;
  mvinnstr(5, 17, s, am[2]);
  for (int i = 0; i < am[2]; data.price = data.price * 10 + (s[i++] - '0')) {
  }
  data.amount = 0;
  mvinnstr(7, 17, s, am[3]);
  for (int i = 0; i < am[3]; data.amount = data.amount * 10 + (s[i++] - '0')) {
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
  data.date = {d, m, y - 2000};
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
  int32_t ch = 'f';
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
  int32_t ch;
  while (a < 1 || a > c) {
    erase();
    a = 0;
    printw("\n  Enter the number from 1 to %lld:", c);
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
    head = item->next;

  if (item->next)
    item->next->last = item->last;
  else
    tail = item->last;

  delete item;
}

void render_all(Item *head) {
  erase();
  printw("\n  %-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s", 5, "Id", 10, "Name", 10,
         "Weight", 10, "Price", 10, "Amount", 9, "Date", 9, "Discount", 5,
         "Key");
  Item *item = head;
  int i = 3;
  while (item) {
    mvprintw(i, 2,
             "\n  %-*lld%-*s%-*lld%-*lld%-*lld%02lld.%02lld.%02lld %-*s%-*lld",
             5, i / 2, 10, item->data.name, 10, item->data.weight, 10,
             item->data.price, 10, item->data.amount, item->data.date.d,
             item->data.date.m, item->data.date.y, 9,
             (item->data.discount) ? "YES" : "NO", 5, item->data.id);
    i += 2;
    item = item->next;
  }

  getch();
}

void search(Item *head) {
  erase();
  const char messages[6][100] = {"Name",   "Weight", "Price",
                                 "Amount", "Date",   "Discount"};
  int32_t ch = 'f';
  int selected = 0;

  while (ch != 'q') {
    render(selected, messages);

    ch = getch();
    switch (ch) {
    case KEY_UP:
      selected = (selected + 5) % 6;
      break;
    case KEY_DOWN:
      selected = (selected + 1) % 6;
      break;
    case '\n': {
      char s[256];
      int i = 0;
      erase();
      printw("\n  %s: ", messages[selected]);
      echo();
      ch = getch();
      while (ch != '\n') {
        s[i++] = ch;
        ch = getch();
      }
      noecho();
      erase();

      bool ans[1000];
      for (int i = 0; i < 1000; ans[i++] = 0) {
      }
      for (Item *item = head; item; item = item->next) {
        char s2[256];
        switch (selected) {
        case 0:
          snprintf(s2, 256, "%s", item->data.name);
          break;
        case 1:

          snprintf(s2, 256, "%lld", item->data.weight);
          break;
        case 2:

          snprintf(s2, 256, "%lld", item->data.price);
          break;
        case 3:

          snprintf(s2, 256, "%lld", item->data.amount);
          break;
        case 4: {
          Date date = item->data.date;
          snprintf(s2, 256, "%lld%lld.%lld%lld.%lld%lld", date.d / 10,
                   date.d % 10, date.m / 10, date.m % 10, date.y / 10,
                   date.y % 10);
          break;
        }
        case 5:

          snprintf(s2, 256, "%s", (item->data.discount) ? "YES" : "NO");
          break;
        }
        i = 0;
        while (s[i] == s2[i] && i < 250)
          i++;
        if (i > 200 || s[i] == '\0' || s2[i] == '\0')
          ans[item->data.id] = 1;
      }
      erase();
      printw("\n  %-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s", 5, "Id", 10, "Name", 10,
             "Weight", 10, "Price", 10, "Amount", 9, "Date", 9, "Discount", 5,
             "Key");
      Item *item = head;
      i = 3;
      while (item) {
        if (ans[item->data.id]) {
          mvprintw(
              i, 2,
              "\n  %-*lld%-*s%-*lld%-*lld%-*lld%02lld.%02lld.%02lld %-*s%-*lld",
              5, i / 2, 10, item->data.name, 10, item->data.weight, 10,
              item->data.price, 10, item->data.amount, item->data.date.d,
              item->data.date.m, item->data.date.y, 9,
              (item->data.discount) ? "YES" : "NO", 5, item->data.id);
          i += 2;
        }
        item = item->next;
      }
      getch();
      return;
    }
    }
  }
}

void file_save(Item *head) {
  int32_t ch;
  char s[256];
  int i = 0;
  erase();
  printw("\n  Enter the file name: ");
  ch = getch();
  while (ch != '\n') {
    if ('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z') {
      s[i++] = ch;
      printw("%c", ch);
    }
    ch = getch();
  }
  s[i] = '.';
  s[i + 1] = 'b';
  s[i + 2] = 'i';
  s[i + 3] = 'n';
  s[i + 4] = '\0';

  erase();
  {
    std::ofstream out(s, std::ios::binary);
    for (Item *item = head; item; item = item->next)
      out.write(reinterpret_cast<const char *>(&item->data), sizeof(Item_data));
  }
}

int file_load(Item *&head, Item *&tail, int id) {
  int newid = 0;
  int32_t ch;
  char s[256];
  int i = 0;
  erase();
  printw("\n  Enter the file name: ");
  ch = getch();
  while (ch != '\n') {
    if ('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z') {
      s[i++] = ch;
      printw("%c", ch);
    }
    ch = getch();
  }
  s[i] = '.';
  s[i + 1] = 'b';
  s[i + 2] = 'i';
  s[i + 3] = 'n';
  s[i + 4] = '\0';

  erase();

  {
    std::ifstream in(s, std::ios::binary);
    if (!in.is_open()) {
      return id;
    }

    while (head) {
      Item *item = head->next;
      delete head;
      head = item;
    }
    tail = nullptr;

    Item_data data;

    while (in.read(reinterpret_cast<char *>(&data), sizeof(Item_data))) {
      push_back(head, tail, data);
      newid = std::max(newid, data.id);
    }
  }
  return newid + 1;
}

int32_t main() {
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

  int32_t ch = 'a';
  int selected = 0;
  Item *head = nullptr;
  Item *tail = nullptr;
  int _id = 1;

  const char messages[6][100] = {"1) Add",    "2) Remove", "3) Display",
                                 "4) Search", "5) Save",   "6) Load"};

  while (ch != 'q') {
    render(selected, messages);

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
        render_all(head);
        break;
      case 3:
        search(head);
        break;
      case 4:
        file_save(head);
        break;
      case 5:
        _id = file_load(head, tail, _id);
        break;
      }
    }
  }

  endwin();
  return 0;
}
