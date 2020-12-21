
#ifndef PAT_HPP__
#define PAT_HPP__
#include "ts.hpp"
namespace ts {

struct pmt_table_t {
  uint16_t program_number;
  uint16_t pid;
};
struct pat : public abstract_ts {
  std::vector<pmt_table_t> pmt_pid;
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool finish_callback(void);
  pat(uint16_t pid);
};
}  // namespace ts
#endif  // PAT_HPP__