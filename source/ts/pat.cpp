#include <cstring>
#include <ts/pat.hpp>
bool PAT::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = true;
  ret = TS::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  memset(this->pid_cur_table, 0, sizeof(this->pid_cur_table));
  uint16_t loop_end = ((data[6] & 0xf) << 8) | data[7] + 8;
  uint16_t loop_begin = 8;
  int i = 0;
  while (loop_begin < loop_end) {
    this->pid_cur_table[i].program_number =
        (data[loop_begin] << 8) | data[loop_begin + 1];
    this->pid_cur_table[i].pid =
        ((data[loop_begin + 2] & 0x1F) << 8) | data[loop_begin + 3];
    loop_begin += 4;
    ++i;
  }
  // TODO: compare whether the data changed with the last one
  // and notify to take action

  return true;
}