#include <ts/bat.hpp>
bool ts_bat_t::loop1_parse(uint8_t *data, uint16_t len) { return true; }
bool ts_bat_t::loop2_parse(uint8_t *data, uint16_t len) { return true; }
bool BAT::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  ret = TS::parse(data, len, priv);
  if (ret == false) {  // current section not need to parse
    return false;
  }
  this->table_id = data[0];
  uint16_t bouquet_id = (data[3] << 8) | data[4];
  uint16_t i = 0;
  while (i < this->ts_num && this->bat_info[i].bat.bouquet_id != bouquet_id) {
    ++i;
  }
  if (i >= this->max_ts_num) {
    LOG_INFO("Error: bat num too much\n");
    return false;
  } else if (i == this->ts_num) {
    this->ts_num++;
  }
  ts_bat_t &bat_info = this->bat_info[i];
  bat_info.bat.bouquet_id = bouquet_id;
  uint16_t loop1_begin = 10;
  uint16_t loop1_end = 10 + ((data[8] << 0xf) << 8) | data[9];
  while (loop1_begin < loop1_end) {
    uint8_t desc_tag = data[loop1_begin];
    uint8_t desc_len = data[loop1_begin + 1];
    bat_info.loop1_parse(data + loop1_begin + 2, desc_len);
    loop1_begin += (2 + desc_len);
  }
  uint16_t loop2_begin = loop1_end;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end = loop2_begin + 2 + ((data[loop2_begin] & 0xf) << 8) |
                       data[loop2_begin + 1];
  loop2_begin += 2;
  while (loop2_begin < loop2_end) {
    bat_info.bat.ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    bat_info.bat.network_id =
        (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len =
        ((data[loop2_begin + 4] & 0xf) << 8) | data[loop2_begin + 5];
    bat_info.loop2_parse(data + loop2_begin + 6, data_len);
    loop2_begin += (6 + data_len);
  }
  return false;
  return true;
}

bool BAT::update_callback(void) { return true; }  // 当有区块更新时回调
bool BAT::finish_callback(void) { return true; }  // 所有区块更新完回调