#include <TSAnalyse/TSAnalyse.h>

using namespace TSAnalyse;

TSAnalyse::TSAnalyse(std::string _name) : name(_name) {}

std::string TSAnalyse::greet(LanguageCode lang) const {
  switch (lang) {
    default:
    case LanguageCode::EN:
      return "Hello, " + name + "!";
    case LanguageCode::DE:
      return "Hallo " + name + "!";
    case LanguageCode::ES:
      return "¡Hola " + name + "!";
    case LanguageCode::FR:
      return "Bonjour " + name + "!";
  }
}
