
#include <cxxopts.hpp>
#include <iostream>
#include <program/program.hpp>
#include <string>
#include <ts/bat.hpp>
#include <ts/nit.hpp>
#include <ts_filter/ts_packet.hpp>
#include <unordered_map>
#if 0
const std::unordered_map<std::string, TSAnalyse::LanguageCode> languages{
    {"en", TSAnalyse::LanguageCode::EN},
    {"de", TSAnalyse::LanguageCode::DE},
    {"es", TSAnalyse::LanguageCode::ES},
    {"fr", TSAnalyse::LanguageCode::FR},
};
#endif

struct A {
  int a;
  int b;
  virtual int getVal(void) { return 1; }
  virtual A &operator[](int i) const {
    return *((A *)((uint8_t *)this) + i * sizeof(A));
  }
  virtual ~A(){};
};
struct B : public A {
  int c;
  int d;
  B() {
    a = 1;
    b = 2;
    c = 3;
    d = 4;
  }
  virtual int getVal(void) { return 2; };
  virtual A &operator[](int i) const {
    printf("hello world.");
    return *((A *)((uint8_t *)this) + i * sizeof(B));
  }
};
int main(int argc, char **argv) {
#if 0
  cxxopts::Options options(argv[0], "A program to welcome the world!");

  std::string language;
  std::string name;

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("n,name", "Name to greet", cxxopts::value(name)->default_value("World"))
    ("l,lang", "Language code to use", cxxopts::value(language)->default_value("en"))
  ;
  // clang-format on

  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help() << std::endl;
    return 0;
  } else if (result["version"].as<bool>()) {
    return 0;
  }

  data_base_t db;
  db.refresh();
  std::cout << db.lookup_node({1, 1}) << std::endl;
  NIT nit(102, 20);
  nit.parse(nullptr, 0, nullptr);
  LOG_INFO("error");
  A *ap = nullptr;
  B *bp = new B[10];
  ap = bp;
  for (int i = 0; i < 10; i++) {
    printf("%d: ap[%d]. a = %d, .b = %d\n", i, i, ap[i].a,
           ap[i].b);  // 果然时不行的
    printf("%d: bp[%d]. a = %d, .b = %d\n", i, i, bp[i].a, bp[i].b);
  }
#endif
  ts_packet_t *ts = new ts_packet_t("./build/app/abc.ts");
  NIT *nit_ts = new NIT(0x10, 4);
  BAT *bat_ts = new BAT(0x11, 64);
  bat_ts->set_filter_table_id(0x4A);
  ts->open_filter(nit_ts);
  ts->open_filter(bat_ts);
  ts->run();
  delete (nit_ts);
  return 0;
}
