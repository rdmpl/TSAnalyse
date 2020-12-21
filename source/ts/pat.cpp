#include <cstring>
#include <ts/pat.hpp>
namespace ts {
pat::pat(uint16_t pid) : abstract_ts(pid), pmt_pid() {}
bool pat::finish_callback(void) {
  for (uint16_t i = 0; i < this->pmt_pid.size(); ++i) {
    printf("%d:program number, 0x%x, pid 0x%x.\n", i,
           this->pmt_pid[i].program_number, this->pmt_pid[i].program_number);
  }
  return true;
}
bool pat::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = true;
  ret = abstract_ts::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  uint16_t loop_begin = 8;
  uint16_t loop_len = getu16len(data + 1, false);
  while (loop_begin < loop_len - 1) {
    pmt_table_t pmt_pid;
    pmt_pid.program_number =
        (data[loop_begin] << 8) |
        data[loop_begin + 1];  // if program number, it is the network id
    pmt_pid.pid = ((data[loop_begin + 2] & 0x1F) << 8) | data[loop_begin + 3];
    this->pmt_pid.push_back(pmt_pid);
    loop_begin += 4;
  }
  return true;
}
}  // namespace ts