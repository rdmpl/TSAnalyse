#include <cstring>
#include <ts/pmt.hpp>

bool PMT::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  ret = TS::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  this->pcr_pid = ((data[8] & 0x1f) << 8) | data[9];
  this->program_info_length = ((data[10] & 0xf) << 8) || data[11];
  uint16_t loop1_begin = 12;
  uint16_t i = 0;
  uint16_t es_info_len = 0;
  while (loop1_begin < 12 + this->program_info_length) {
    this->pmt_data_cur[i].stream_type = data[loop1_begin];
    this->pmt_data_cur[i].pid =
        ((data[loop1_begin + 1] & 0x1f) << 8) | data[loop1_begin + 2];
    this->pmt_data_cur[i].es_info_len =
        ((data[loop1_begin + 3] & 0xf) << 8) | data[loop1_begin + 4];
    es_info_len = this->pmt_data_cur[i].es_info_len;
    if (this->pmt_data_cur[i].es_info_len >
        lengthof(this->pmt_data_cur[i].es_info)) {
      LOG_INFO("error: es info len too long!!![%d][%d].\n",
               this->pmt_data_cur[i].stream_type,
               this->pmt_data_cur[i].es_info_len);
      this->pmt_data_cur[i].es_info_len =
          lengthof(this->pmt_data_cur[i].es_info);
    }
    memcpy(this->pmt_data_cur[i].es_info, data + loop1_begin + 5,
           this->pmt_data_cur[i].es_info_len);
    ++i;
    loop1_begin += (5 + es_info_len);
    // TODO: Call registered function to do something
  }
  return true;
}