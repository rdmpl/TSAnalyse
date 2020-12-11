#ifndef BAT_HPP__
#define BAT_HPP__
#include "ts.hpp"
struct BAT : public TS {
  virtual bool parse(uint8_t *data, uint16_t len, void *priv){};
};
#endif  // BAT_HPP__