
#ifndef PAT_HPP__
#define PAT_HPP__
#include "ts.hpp"
struct pid_table_t {
  uint16_t program_number;
  uint16_t pid;
};
struct PAT : public TS {
  pid_table_t pid_cur_table[128];
  pid_table_t pid_last_table[128];
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
};
#endif  // PAT_HPP__