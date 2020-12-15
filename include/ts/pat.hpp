
#ifndef PAT_HPP__
#define PAT_HPP__
#include "ts.hpp"
struct pmt_table_t {
  uint16_t program_number;
  uint16_t pid;
};
struct PAT : public TS {
  std::vector<pmt_table_t> pmt_pid;
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool finish_callback(void);
  PAT(uint16_t pid, uint16_t max_count);
};
#endif  // PAT_HPP__