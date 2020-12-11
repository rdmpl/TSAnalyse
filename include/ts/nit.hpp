
#ifndef NIT_HPP__
#define NIT_HPP__
#include "ts.hpp"
struct NIT : public TS {
  virtual bool parse(uint8_t *data, uint16_t len, void *priv){};
};
#endif  // NIT_HPP__