
#ifndef PMT_HPP__
#define PMT_HPP__
#include "ts.hpp"
struct pmt_data_t {
  uint8_t stream_type;
  uint16_t pid;
  uint16_t es_info_len;
  uint8_t es_info[64];  // 每个流最多允许64字节的es标记
};
struct PMT : public TS {
  uint16_t pcr_pid;
  uint16_t program_info_length;
  pmt_data_t pmt_data_cur[128];
  pmt_data_t pmt_data_last[128];
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
};
#endif  // PMT_HPP__