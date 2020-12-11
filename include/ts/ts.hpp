#ifndef TS_HPP__
#define TS_HPP__
#include <stdint.h>
struct TS {
  uint8_t version;
  uint8_t current_section;
  uint8_t last_section;
  uint32_t crc32;
  virtual parse(uint8_t *data, uint16_t len, void *priv);
  virtual ~TS(){};
};
#endif  // end of TS_HPP__