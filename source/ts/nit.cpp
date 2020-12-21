#include <cstring>
#include <ts/nit.hpp>
namespace ts {
bool freq_parse(uint8_t *data, uint8_t len, prog_tp_t &tp) {
  /**
   * frequency:32
   * reserved: 12
   * fec_outer: 4
   * modulation: 8
   * symbol_rate: 28
   * fec_inner: 4
   */
  uint8_t i = 0;
  tp.freq = bcd2integer(data, 4) / 100;
  tp.mod = data[6];  // data[5] == fec_outer
  tp.fec_inner = data[10] & 0xf;
  tp.sym = bcd2integer(data + 7, 3);  // 这里还有点问题,可能精度不够，但不管了
  return true;
}
bool service_list_parse(uint8_t *data, uint8_t len, prog_tp_t &tp) {
  uint8_t i = 0;
  while (i < len) {
    uint16_t service_id = (data[i] << 8) | data[i + 1];
    uint8_t service_type = data[i + 2];
    tp.service_info.push_back({service_id, service_type});
    i += 3;
    // LOG_INFO("service i = %d, len = %d", i, len);
  }
  return true;
}
bool nit::loop1_parse(uint8_t *data, uint16_t len) { return true; }
bool loop2_parse(uint8_t *data, uint16_t len, prog_tp_t &tp) {
  uint16_t i = 0;
  while (i < len) {
    uint8_t tag = data[i];
    uint8_t tag_len = data[i + 1];
    i += 2;
    switch (tag) {
      case 0x44:  // freq info
        freq_parse(data + i, tag_len, tp);
        break;
      case 0x41:  // service list
        service_list_parse(data + i, tag_len, tp);
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
  uint16_t original_network_id = (data[3] << 8) | data[4];
  uint16_t loop1_len = getu16len(data + 8, false);
  loop1_parse(data + 10, loop1_len);
  uint16_t loop2_begin = 10 + loop1_len;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end = loop2_begin + 2 + getu16len(data + loop2_begin, false);
  loop2_begin += 2;
  while (loop2_begin < loop2_end) {
    uint16_t ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    int k = 0;
    while (k < this->tp_info.size() && this->tp_info[k].ts_id != ts_id) {
      ++k;
    }
    if (k == this->tp_info.size()) {
      this->tp_info.push_back(prog_tp_t());
    }
    prog_tp_t &tp = this->tp_info[k];
    tp.original_network_id = original_network_id;
    tp.network_id = (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len = getu16len(data + loop2_begin + 4, false);
    loop2_parse(data + loop2_begin + 6, data_len, tp);
    this->tp_info.emplace_back(tp);
    loop2_begin += (6 + data_len);
  }
  return false;
}
bool nit::update_callback(void) { return true; };  // 当有区块更新时回调
bool nit::finish_callback(void) {
  LOG_INFO("all nit data finished.\n");
  dump();
  return true;
};  // 所有区块更新完回调
}  // namespace ts