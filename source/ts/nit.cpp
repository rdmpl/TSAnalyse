#include <cstring>
#include <ts/nit.hpp>
namespace ts {
nit_info_t::~nit_info_t() {
  if (net) {
    delete (net);
  }
  if (header) {
    delete (header);
  }
  for (int i = 0; i < ts.size(); ++i) {
    if (ts[i]) {
      delete (ts[i]);
      ts[i] = nullptr;
    }
  }
}
nit::~nit() {
  if (nit_fac) {
    delete (nit_fac);
  }
}
nit_ts_t::~nit_ts_t() {}
bool nit_network_t::nit_network_parse(uint8_t *data, uint16_t size) {
  return true;
}
nit_header_t *nit_factory_t::create_nit_header(uint16_t original_network_id) {
  nit_header_t *header = new nit_header_t();
  header->network_id = original_network_id;
  return header;
}
nit_ts_t *nit_factory_t::create_nit_ts(uint16_t ts_id) {
  nit_ts_t *ts = new nit_ts_t();
  ts->ts_id = ts_id;
  return ts;
}
nit_network_t *nit_factory_t::create_nit_network(void) {
  nit_network_t *net = new nit_network_t();
  return net;
}
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
bool service_list_parse(uint8_t *data, uint8_t len, nit_ts_t *tp) {
  uint8_t i = 0;
  while (i < len) {
    uint16_t service_id = (data[i] << 8) | data[i + 1];
    uint8_t service_type = data[i + 2];
    tp->service_list.push_back({service_id, service_type});
    i += 3;
    // LOG_INFO("service i = %d, len = %d", i, len);
  }
  return true;
}
bool nit_ts_t::nit_ts_parse(uint8_t *data, uint16_t len) {
  uint16_t i = 0;
  while (i < len) {
    uint8_t tag = data[i];
    uint8_t tag_len = data[i + 1];
    i += 2;
    switch (tag) {
      case 0x44:  // freq info
        this->freq = bcd2integer(data + i, 4) / 100;
        this->mod = data[i + 6];
        this->sym = bcd2integer(data + i + 7, 3);
        this->fec_inner = data[i + 10] & 0xf;
        break;
      case 0x41:  // service list
        service_list_parse(data + i, tag_len, this);
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
  int i = 0;
  for (i = 0; i < this->nit_info.size(); ++i) {
    if (this->nit_info[i].header->network_id == original_network_id) {
      break;
    }
  }
  if (i == this->nit_info.size()) {
    this->nit_info.push_back(nit_info_t());
  }
  nit_info_t &nit = this->nit_info[i];
  nit.net = nit_fac->create_nit_network();
  nit.header = nit_fac->create_nit_header(original_network_id);
  uint16_t loop1_len = getu16len(data + 8, false);
  nit.net->nit_network_parse(data + 10, loop1_len);  // now not care
  uint16_t loop2_begin = 10 + loop1_len;
  // TODO: notify other thread to parse loop1 data
  uint16_t loop2_end = loop2_begin + 2 + getu16len(data + loop2_begin, false);
  loop2_begin += 2;
  while (loop2_begin < loop2_end) {
    uint16_t ts_id = (data[loop2_begin] << 8) | data[loop2_begin + 1];
    nit_ts_t *ts = nit_fac->create_nit_ts(ts_id);
    nit.ts.push_back(ts);
    ts->network_id = (data[loop2_begin + 2] << 8) | data[loop2_begin + 3];
    uint16_t data_len = getu16len(data + loop2_begin + 4, false);
    ts->nit_ts_parse(data + loop2_begin + 6, data_len);
    loop2_begin += (6 + data_len);
  }
  return false;
}
bool nit_ts_t::self_speak(void) {
  fmt::print("ts_id = 0x{:04x}, freq = {:d}, mod = {:d}, service cnt = {}.\n",
             ts_id, freq, mod, service_list.size());
  for (int i = 0; i < service_list.size(); ++i) {
    fmt::print("0x{:4x}({})  ", service_list[i].first, service_list[i].second);
  }
  fmt::print("\n\n");
  return true;
}
bool nit_info_t::self_speak(void) {
  fmt::print("network_id = 0x{:04x}. ts_count = {:d}\n", header->network_id,
             this->ts.size());
  for (int i = 0; i < this->ts.size(); ++i) {
    this->ts[i]->self_speak();
  }
  fmt::print("\n");
  return true;
}
bool nit::self_speak(void) {
  fmt::print("original net count = {}\n", this->nit_info.size());
  for (int i = 0; i < this->nit_info.size(); ++i) {
    this->nit_info[i].self_speak();
  }
  return true;
}
bool nit::update_callback(void) { return true; };  // 当有区块更新时回调
bool nit::finish_callback(void) {
  LOG_INFO("all nit data finished.\n");
  self_speak();
  return true;
};  // 所有区块更新完回调
}  // namespace ts