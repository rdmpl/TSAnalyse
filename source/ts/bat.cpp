#include <ts/bat.hpp>
namespace ts {
uint16_t convert_to_unicode(uint8_t *src, uint16_t len, uint8_t *dest,
                            uint16_t max_len) {
  uint16_t i = 0;
  for (i = 0; i < len && i < max_len; ++i) {
    dest[i] = src[i];
  }
  return i;
}
bool bat::loop1_parse(uint8_t *data, uint16_t len, bat_info_t &bat) {
  uint16_t loop_len = 0;
  while (loop_len < len) {
    uint8_t desc_tag = data[loop_len];
    uint8_t desc_len = data[loop_len + 1];
    switch (desc_tag) {
      case 0x47:  // bat name
        bat.bouquet_name_len =
            convert_to_unicode(data + loop_len + 2, desc_len, bat.bouquet_name,
                               lengthof(bat.bouquet_name));
        break;
      case 0x4a:  // if bid = 0xc0 is the startup program of ucn
        if (bat.bouquet_id == 0xc0) {
          uint16_t sid = (data[loop_len + 2] << 8) | data[loop_len + 3];
          LOG_INFO("start_up service id = {}", sid);
        }
        break;
      default:
        break;
    }
    loop_len += (2 + desc_len);
  }
  return true;
}
bool bat::loop2_parse(uint8_t *data, uint16_t len, bat_info_t &bat) {
  uint16_t loop_len = 0;
  while (loop_len < len) {
    uint8_t desc_tag = data[loop_len];
    uint8_t desc_len = data[loop_len + 1];
    // LOG_INFO("tag = %x", desc_tag);
    switch (desc_tag) {
      case 0x41:
        for (uint8_t p = loop_len + 2; p < loop_len + desc_len + 2; p += 3) {
          uint16_t service_id = data[p] << 8 | data[p + 1];
          bat.service_info.push_back({service_id, 1});
        }
        break;
      case 0x83:
        // UCN's lcn parse
        break;
      default:
        break;
    }
    loop_len += (2 + desc_len);
  }
  return true;
}
bool bat::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  ret = abstract_ts::parse(data, len, priv);
  if (ret == false) {  // current section not need to parse
    return false;
  }
  uint16_t bouquet_id = (data[3] << 8) | data[4];
  uint16_t i = 0;
  while (i < this->bat_info.size() &&
         this->bat_info[i].bouquet_id != bouquet_id) {
    ++i;
  }
  if (i >= this->bat_info.size()) {
    this->bat_info.push_back(bat_info_t());
  }
  bat_info_t &bat_info = this->bat_info[i];
  bat_info.bouquet_id = bouquet_id;
  uint16_t loop1_len = getu16len(data + 8, false);  //
  this->loop1_parse(data + 10, loop1_len, bat_info);
  uint16_t loop2_begin = 10 + loop1_len;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end = loop2_begin + 2 + getu16len(data + loop2_begin, false);
  loop2_begin += 2;
  while (loop2_begin < loop2_end) {
    bat_info.ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    bat_info.network_id = (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len = getu16len(data + loop2_begin + 4, false);
    this->loop2_parse(data + loop2_begin + 6, data_len, bat_info);
    loop2_begin += (6 + data_len);
  }
  return true;
}
bool bat::dump(void) {
  for (uint16_t i = 0; i < this->bat_info.size(); ++i) {
    printf("\n\n%d: bat_id = 0x%x\n", i, this->bat_info[i].bouquet_id);
    for (uint16_t j = 0; j < this->bat_info[i].service_info.size(); ++j) {
      printf("0x%x  ", this->bat_info[i].service_info[j].first);
    }
  }
  return true;
}
bool bat::update_callback(void) { return true; }  // 当有区块更新时回调
bool bat::finish_callback(void) {
  LOG_INFO("all bat finished: bat_num = {}", this->bat_info.size());
  dump();
  return true;
}  // 所有区块更新完回调
}  // namespace ts