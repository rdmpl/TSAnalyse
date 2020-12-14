#ifndef TS_HPP__
#define TS_HPP__
#include <stdint.h>

#include <cstring>
#include <program/program.hpp>
#include <ultilities/ultilities.hpp>
#include <vector>
inline uint16_t getu16len(uint8_t *data, bool endian_big) {
  uint16_t res;
  uint8_t *ptr = (uint8_t *)(&res);
  if (endian_big) {
    ptr[0] = data[0] & 0xf;
    ptr[1] = data[1];
  } else {
    ptr[0] = data[1];
    ptr[1] = data[0] & 0xf;
  }
  return res;
}
inline uint32_t bcd2integer(uint8_t *data, uint8_t len) {
  uint8_t i = 0;
  uint32_t res = 0;
  while (i < 2 * len) {
    uint8_t bcd = (i & 0x1) ? (data[i >> 1] & 0xf) : (data[i >> 1] >> 4);
    res = res * 10 + bcd;
    ++i;
  }
  return res;
}
struct ts_unit_t {
  uint16_t unit_id;
  uint8_t version_number;
  uint16_t rcv_cnt;     // increase when all sections has been received
  uint16_t rcv_number;  // increase when a new section received
  uint16_t last_section_number;
  bool b_has_parsed;
  uint32_t crc32[32];
  virtual bool operator<(const ts_unit_t &other) const;
  virtual void restart(bool parsed_flag);
  virtual ~ts_unit_t() {}
};
/**
 * The Base class of all ts stream, including PAT, PMT, BAT, NIT, SDT
 * the parse function just check whether the ts is updated or whether
 * current section need to be parsed.
 * The child class should parse the data, if udpated.
 */
struct TS {
  uint16_t pid;
  uint8_t slot[8];  // 最多可以设置8个过滤深度
  uint8_t mask[8];
  uint32_t filer_id;
  uint32_t last_crc32;  // calculted by unit
  bool b_need_notify;
  uint16_t max_ts_num;  // equal to max_ts_num;
  uint16_t unit_num;    // num of unit
  uint16_t ts_num;      // num of ts_data
  std::vector<ts_unit_t> unit;
  TS();
  TS(uint16_t pid, uint16_t max_count);
  bool check_match(uint8_t *data, uint8_t len);
  bool set_filter_table_id(uint8_t table_id);
  bool set_filter_param(uint8_t *s, uint8_t *m, uint8_t depth);
  virtual void reset(bool flag);
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void) { return true; };  // 当有区块更新时回调
  virtual bool finish_callback(void) { return true; };  // 所有区块更新完回调
  virtual uint32_t cal_crc32(void);
  virtual bool check_finish(ts_unit_t &cur_unit);
  virtual ~TS(){};
};
#endif  // end of TS_HPP__