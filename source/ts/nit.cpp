#include <cstring>
#include <ts/nit.hpp>
namespace ts {
bool tp_info_t::freq_parse(uint8_t *data, uint8_t len) {
  /**
   * frequency:32
   * reserved: 12
   * fec_outer: 4
   * modulation: 8
   * symbol_rate: 28
   * fec_inner: 4
   */
  uint8_t i = 0;
  this->tp.freq = bcd2integer(data, 4) / 100;
  this->tp.mod = data[6];  // data[5] == fec_outer
  this->tp.fec_inner = data[10] & 0xf;
  this->tp.sym =
      bcd2integer(data + 7, 3);  // 这里还有点问题,可能精度不够，但不管了
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
      case 0x83:
        break;  // TODO: lcn parse
      default:
        break;
    }
    i += tag_len;
  }
  return true;
}
bool nit::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  ret = abstract_ts::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  uint16_t network_id = (data[3] << 8) | data[4];
  uint16_t i = 0;
  while (i < this->ts_num && this->tp_info[i].network_id != network_id) {
    ++i;
  }
  if (i >= this->max_ts_num) {
    LOG_INFO("Error: bat num too much\n");
    return false;
  } else if (i == this->ts_num) {
    this->ts_num++;
  }
  nit_tp_info_t &tp_info = this->tp_info[i];
  tp_info.network_id = network_id;
  uint16_t loop1_len = getu16len(data + 8, false);
  tp_info.loop1_parse(data + 10, loop1_len);
  uint16_t loop2_begin = 10 + loop1_len;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end = loop2_begin + 2 + getu16len(data + loop2_begin, false);
  loop2_begin += 2;
  i = tp_info.tp_count;
  while (loop2_begin < loop2_end && i < lengthof(tp_info.tp)) {
    tp_info.tp[i].tp.ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    tp_info.tp[i].tp.network_id =
        (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len = getu16len(data + loop2_begin + 4, false);
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
bool nit::update_callback(void) { return true; };  // 当有区块更新时回调
bool nit::finish_callback(void) {
  LOG_INFO("all nit data finished.\n");
  dump();
  return true;
};  // 所有区块更新完回调
}  // namespace ts