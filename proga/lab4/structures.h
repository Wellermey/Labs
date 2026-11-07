
struct Date {
  long long d;
  long long m;
  long long y;
};

struct Item_data {
  long long id;
  char name[256];
  long long weight;
  long long price;
  long long amount;
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
