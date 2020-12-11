
#ifndef PAT_HPP__
#define PAT_HPP__
#include "ts.hpp"
struct PAT : public TS {
  virtual bool parse(uint8_t *data, uint16_t len, void *priv){};
};
#endif  // PAT_HPP__