#ifndef TS_HPP__
#define TS_HPP__
#include <stdint.h>
#include <stdio.h>

#include <cstring>
#include <ultilities/ultilities.hpp>
#define LOG_INFO(fmt, args...) \
  printf("%s(%d)" fmt, __FUNCTION__, __LINE__, ##args)
struct ts_unit_t {
  uint16_t unit_id;
  uint8_t version_number;
  uint16_t rcv_cnt;     // increase when all sections has been received
  uint16_t rcv_number;  // increase when a new section received
  uint16_t last_section_number;
  bool b_has_parsed;
  uint32_t crc32[32];
  bool operator<(const ts_unit_t &other) const;
  void restart(bool parsed_flag);
};
/**
 * The Base class of all ts stream, including PAT, PMT, BAT, NIT, SDT
 * the parse function just check whether the ts is updated or whether
 * current section need to be parsed.
 * The child class should parse the data, if udpated.
 */
struct TS {
  uint16_t pid;
  uint32_t filer_id;
  uint8_t table_id;
  uint8_t unit_count;
  ts_unit_t unit[32];
  uint32_t last_crc32;  // calculted by unit
  bool b_need_notify;
  TS(uint16_t pid);
  virtual void reset(bool flag);
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual uint32_t cal_crc32(void);
  virtual bool check_finish(ts_unit_t &cur_unit);
  virtual ~TS(){};
};
#endif  // end of TS_HPP__