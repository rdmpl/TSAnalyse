
#ifndef SDT_HPP__
#define SDT_HPP__
#include "ts.hpp"
struct SDT : public TS {
  virtual bool parse(uint8_t *data, uint16_t len, void *priv){};
};
#endif  // SDT_HPP__