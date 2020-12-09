#include <TSAnalyse/TSAnalyse.h>
#include <TSAnalyse/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

const std::unordered_map<std::string, TSAnalyse::LanguageCode> languages{
    {"en", TSAnalyse::LanguageCode::EN},
    {"de", TSAnalyse::LanguageCode::DE},
    {"es", TSAnalyse::LanguageCode::ES},
    {"fr", TSAnalyse::LanguageCode::FR},
};

int main(int argc, char** argv) {
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
    std::cout << "TSAnalyse, version " << TSAnalyse_VERSION << std::endl;
    return 0;
  }

  auto langIt = languages.find(language);
  if (langIt == languages.end()) {
    std::cerr << "unknown language code: " << language << std::endl;
    return 1;
  }

  TSAnalyse::TSAnalyse TSAnalyse(name);
  std::cout << TSAnalyse.greet(langIt->second) << std::endl;

  return 0;
}
