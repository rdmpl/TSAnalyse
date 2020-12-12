#include <cstring>
#include <ts/nit.hpp>
bool NIT::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  uint16_t loop2_begin = 0;
  uint16_t loop2_end = 0;
  uint16_t i = 0;
  ret == TS::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  this->network_id = (data[3] << 8) | data[4];
  this->loop1_len = ((data[8] & 0xf) << 4) | data[9];
  loop2_begin = 10 + this->loop1_len;
  if (this->loop1_len > lengthof(this->loop1_data)) {
    LOG_INFO("Error: loop1 too long.[%d][%ld]\n", this->loop1_len,
             lengthof(this->loop1_data));
    this->loop1_len = lengthof(this->loop1_data);
  }
  memcpy(this->loop1_data, data + 10, this->loop1_len);
  // TODO: notify other thread to parse loop1 data
  loop2_end = loop2_begin + 2 + ((data[loop2_begin] & 0xf) << 8) |
              data[loop2_begin + 1];
  loop2_begin += 2;
  i = 0;
  while (loop2_begin < loop2_end) {
    this->network_descriptor[i].ts_id =
        (data[loop2_begin] << 8) | data[loop2_begin + 1];
    this->network_descriptor[i].original_network_id =
        (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len =
        ((data[loop2_begin + 4] & 0xf) << 8) | data[loop2_begin + 5];
    this->network_descriptor[i].descriptor_length =
        data_len < lengthof(this->network_descriptor[i].descriptor_data)
            ? data_len
            : lengthof(this->network_descriptor[i].descriptor_data);
    memcpy(this->network_descriptor[i].descriptor_data, data + loop2_begin + 6,
           this->network_descriptor[i].descriptor_length);
    loop2_begin += (6 + data_len);
    ++i;
  }
  return false;
}