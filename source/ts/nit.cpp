#include <cstring>
#include <ts/nit.hpp>
bool tp_info_t::freq_parse(uint8_t *data, uint8_t len) {
  uint8_t i = 0;
  this->tp.freq = bcd2integer(data, 4) / 100;
  this->tp.fec_inner = data[6] + 3;
  this->tp.sym = bcd2integer(data + 7, 3);
  return true;
}
bool tp_info_t::service_list_parse(uint8_t *data, uint8_t len) {
  uint8_t i = 0, j = 0;
  while (i < len && j < lengthof(this->service_list)) {
    this->service_list[j].service_id = (data[i] << 8) | data[i + 1];
    this->service_list[j].service_type = data[i + 2];
    ++j;
    i += 3;
    // LOG_INFO("service i = %d, len = %d", i, len);
  }
  this->service_count = j;
  if (j >= lengthof(this->service_list)) {
    LOG_INFO("Error: too much service in current tp.\n");
  }
  return true;
}
bool nit_tp_info_t::loop1_parse(uint8_t *data, uint16_t len) { return true; }
bool tp_info_t::loop2_parse(uint8_t *data, uint16_t len) {
  uint16_t i = 0;
  while (i < len) {
    uint8_t tag = data[i];
    uint8_t tag_len = data[i + 1];
    i += 2;
    switch (tag) {
      case 0x44:  // freq info
        this->freq_parse(data + i, tag_len);
        break;
      case 0x41:  // service list
        this->service_list_parse(data + i, tag_len);
        break;
      default:
        break;
    }
    i += tag_len;
  }
  return true;
}
bool NIT::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  ret = TS::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  this->table_id = data[0];
  uint16_t network_id = (data[3] << 8) | data[4];
  uint16_t i = 0;
  while (i < this->ts_num && this->tp_info[i].network_id != network_id) {
    ++i;
  }
  LOG_INFO("i = %d, max = %d", i, this->max_ts_num);
  if (i >= this->max_ts_num) {
    LOG_INFO("Error: bat num too much\n");
    return false;
  } else if (i == this->ts_num) {
    this->ts_num++;
  }
  nit_tp_info_t &tp_info = this->tp_info[i];
  tp_info.network_id = network_id;
  uint16_t loop1_begin = 10;
  uint16_t loop1_end = 10 + getu16len(data + 8, false);  // ((data[8] & 0xf) << 4) | data[9];
  while (loop1_begin < loop1_end) {
    uint8_t desc_tag = data[loop1_begin];
    uint8_t desc_len = data[loop1_begin + 1];
    tp_info.loop1_parse(data + loop1_begin + 2, desc_len);
    loop1_begin += (2 + desc_len);
  }
  uint16_t loop2_begin = loop1_end;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end
      = loop2_begin + 2
        + getu16len(data + loop2_begin,
                    false);  //((data[loop2_begin] & 0xf) << 8) | data[loop2_begin + 1];
  loop2_begin += 2;
  i = tp_info.tp_count;
  while (loop2_begin < loop2_end && i < lengthof(tp_info.tp)) {
    tp_info.tp[i].tp.ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    tp_info.tp[i].tp.network_id = (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len
        = getu16len(data + loop2_begin + 4,
                    false);  //((data[loop2_begin + 4] & 0xf) << 8) | data[loop2_begin + 5];
    tp_info.tp[i].loop2_parse(data + loop2_begin + 6, data_len);
    loop2_begin += (6 + data_len);
    ++i;
  }
  if (i >= lengthof(tp_info.tp)) {
    LOG_INFO("error: too much tp.");
  }
  tp_info.tp_count = i;
  return false;
}
bool NIT::update_callback(void) { return true; };  // 当有区块更新时回调
bool NIT::finish_callback(void) {
  LOG_INFO("all nit data finished.\n");
  dump();
  return true;
};  // 所有区块更新完回调