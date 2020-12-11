
#ifndef PMT_HPP__
#define PMT_HPP__
#include "ts.hpp"
struct PMT : public TS {
  virtual bool parse(uint8_t *data, uint16_t len, void *priv){};
};
#endif  // PMT_HPP__