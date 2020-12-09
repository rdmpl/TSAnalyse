#include <TSAnalyse/TSAnalyse.h>
#include <TSAnalyse/version.h>
#include <doctest/doctest.h>

#include <string>

TEST_CASE("TSAnalyse") {
  using namespace TSAnalyse;

  TSAnalyse TSAnalyse("Tests");

  CHECK(TSAnalyse.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(TSAnalyse.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(TSAnalyse.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(TSAnalyse.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("TSAnalyse version") {
  static_assert(std::string_view(TSAnalyse_VERSION) == std::string_view("1.0"));
  CHECK(std::string(TSAnalyse_VERSION) == std::string("1.0"));
}