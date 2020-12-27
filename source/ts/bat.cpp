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

bool bouquet_service_list::bouquet_element_parse(uint8_t *data, uint16_t len) {
  uint16_t i = 0;
  while (i < len) {
    uint16_t sid = (data[i] << 8) | data[i + 1];
    uint8_t type = data[i + 2];
    i += 3;
    this->service.push_back(std::make_pair(sid, type));
  }
  return true;
}
bool bouquet_lcn_list::bouquet_element_parse(uint8_t *data, uint16_t len) {
  uint16_t i = 0;
  while (i < len) {
    uint16_t sid = (data[i] << 8) | data[i + 1];
    uint16_t lcn = ((data[i + 2] & 0x1f) << 8) | data[i + 3];
    this->service.push_back(std::make_pair(sid, lcn));
    i += 4;
  }
  return true;
}

bool bouquet_header_t::bouquet_header_parse(uint8_t *data, uint16_t size) {
  uint16_t i = 0;
  while (i < size) {
    uint8_t tag = data[i];
    uint8_t len = data[i + 1];
    if (tag == 0x47) {  //
      convert_to_unicode(data + i + 2, len, this->bouquet_name,
                         lengthof(this->bouquet_name));
    } else if (tag == 0x4a) {
      uint16_t start_up_sid = (data[i + 2] << 8) | data[i + 3];
    }
    i += (2 + len);
  }
  return true;
}
bouquet_element_t *bouquet_factory_t::create_bouquet_element(uint8_t tag) {
  bouquet_element_t *ele = nullptr;
  if (tag == 0x41) {
    ele = new bouquet_service_list();
  } else if (tag == 0x83) {
    ele = new bouquet_lcn_list();
  }
  if (ele) {
    ele->tag = tag;
  }
  return ele;
}
bouquet_header_t *bouquet_factory_t::create_bouquet_header(uint16_t id) {
  bouquet_header_t *header = nullptr;
  header = new bouquet_header_t();
  header->bouquet_id = id;
  return header;
}
bool bouquet_ts_t::bouquet_ts_parse(uint8_t *data, uint16_t size,
                                    bouquet_factory_t *fac) {
  if (data == nullptr || fac == nullptr) {
    return false;
  }
  uint16_t i = 0;
  while (i < size) {
    uint8_t tag = data[i];
    uint8_t len = data[i + 1];
    bouquet_element_t *ele = fac->create_bouquet_element(tag);
    if (ele != nullptr) {
      this->element.push_back(ele);
      ele->bouquet_element_parse(data + i + 2, len);
    }
    i += (2 + len);
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
  while (i < this->bouquet.size() &&
         this->bouquet[i].header->bouquet_id != bouquet_id) {
    ++i;
  }
  if (i >= this->bouquet.size()) {
    this->bouquet.push_back(bouquet_info_t());
  }
  bouquet_info_t &bat_info = this->bouquet[i];
  bat_info.header = fac->create_bouquet_header(bouquet_id);
  uint16_t header_len = getu16len(data + 8, false);  //
  bat_info.header->bouquet_header_parse(data + 10, header_len);
  uint16_t loop2_begin = 10 + header_len;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end = loop2_begin + 2 + getu16len(data + loop2_begin, false);
  loop2_begin += 2;
  while (loop2_begin < loop2_end) {
    bouquet_ts_t ts;
    ts.ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    ts.network_id = (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len = getu16len(data + loop2_begin + 4, false);
    ts.bouquet_ts_parse(data + loop2_begin + 6, data_len, fac);
    loop2_begin += (6 + data_len);
    bat_info.ts.emplace_back(ts);
  }
  return true;
}
bool bouquet_lcn_list::self_speak(void) {
  for (int i = 0; i < this->service.size(); ++i) {
    fmt::print("sid(lcn):{:04x}({:d})  ", this->service[i].first,
               this->service[i].second);
  }
  return true;
}
bool bouquet_service_list::self_speak(void) {
  for (int i = 0; i < this->service.size(); ++i) {
    fmt::print("0x{:04x}({:d})  ", this->service[i].first,
               this->service[i].second);
  }
  return true;
}
bool bouquet_ts_t::self_speak(void) {
  fmt::print("ts_id = 0x{:04x}, network_id = {:04x}\n", this->ts_id,
             this->network_id);
  for (int i = 0; i < this->element.size(); ++i) {
    this->element[i]->self_speak();
  }
  fmt::print("\n");
  return true;
}
bool bouquet_info_t::self_speak(void) {
  fmt::print("\n\nbouquet_id = 0x{:04x}, ts_count = {:d}\n",
             this->header->bouquet_id, this->ts.size());
  for (int i = 0; i < this->ts.size(); ++i) {
    this->ts[i].self_speak();
  }
  return true;
}
bool bat::self_speak(void) {
  for (uint16_t i = 0; i < this->bouquet.size(); ++i) {
    this->bouquet[i].self_speak();
  }
  return true;
}
bool bat::update_callback(void) { return true; }  // 当有区块更新时回调
bool bat::finish_callback(void) {
  LOG_INFO("all bat finished: bat_num = {}", this->bouquet.size());
  this->self_speak();
  return true;
}  // 所有区块更新完回调
}  // namespace ts