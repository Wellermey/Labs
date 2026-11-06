
struct Date {
  int d;
  int m;
  int y;
};

struct Item_data {
  int id;
  char name[256];
  int weight;
  int price;
  int amount;
  Date date;
  bool discount;
};

struct Item {
  Item_data data;
  Item *next;
  Item *last;

  Item() {
    next = nullptr;
    last = nullptr;
  }
  Item(Item_data d, Item *l) {
    data = d;
    next = nullptr;
    last = l;
  }
};
