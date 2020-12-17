
#ifndef SDT_HPP__
#define SDT_HPP__
#include "ts.hpp"
namespace ts {
struct sdt : public abstract_ts {
  virtual bool parse(uint8_t *data, uint16_t len, void *priv){};
};
}  // namespace ts
#endif  // SDT_HPP__