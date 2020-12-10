
#include <cxxopts.hpp>
#include <iostream>
#include <program/program.hpp>
#include <string>
#include <unordered_map>

#if 0
const std::unordered_map<std::string, TSAnalyse::LanguageCode> languages{
    {"en", TSAnalyse::LanguageCode::EN},
    {"de", TSAnalyse::LanguageCode::DE},
    {"es", TSAnalyse::LanguageCode::ES},
    {"fr", TSAnalyse::LanguageCode::FR},
};
#endif

int main(int argc, char** argv) {
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
#endif
  data_base_t db;
  db.refresh();
  std::cout << db.lookup_node({1, 1}) << std::endl;
  return 0;
}
