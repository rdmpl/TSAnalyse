#include <cstring>
#include <ts/pat.hpp>
PAT::PAT(uint16_t pid, uint16_t max_count) : TS(pid, max_count) {
  pmt_pid.resize(max_count);
}
bool PAT::finish_callback(void) {
  for (uint16_t i = 0; i < this->ts_num; ++i) {
    printf("%d:program number, 0x%x, pid 0x%x.\n", i,
           this->pmt_pid[i].program_number, this->pmt_pid[i].program_number);
  }
  return true;
}
bool PAT::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = true;
  ret = TS::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  uint16_t loop_begin = 8;
  uint16_t loop_len = getu16len(data + 1, false);
  int i = this->ts_num;
  while (loop_begin < loop_len - 1 && i < this->max_ts_num) {
    this->pmt_pid[i].program_number =
        (data[loop_begin] << 8) |
        data[loop_begin + 1];  // if program number, it is the network id
    this->pmt_pid[i].pid =
        ((data[loop_begin + 2] & 0x1F) << 8) | data[loop_begin + 3];
    loop_begin += 4;
    ++i;
  }
  if (i >= this->max_ts_num) {
    LOG_INFO("error: PAT too much!!!");
  }
  LOG_INFO("i = %d", i);
  this->ts_num = i;
  return true;
}