#ifndef TS_HPP__
#define TS_HPP__
#include <stdint.h>
#include <stdio.h>
#define LOG_INFO(fmt, args...) printf("%s(%d)" fmt, __FUNCTION__, __LINE__, ##args)
struct TS {
  uint32_t filer_id;
  uint8_t table_id;
  uint16_t section_length;
  uint8_t version_number;
  uint8_t current_section;
  uint8_t last_section;
  uint32_t crc32;
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual ~TS(){};
};
#endif  // end of TS_HPP__