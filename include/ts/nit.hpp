
#ifndef NIT_HPP__
#define NIT_HPP__

#include "ts.hpp"
#include "vector"
namespace ts {
// the header of nit table
struct nit_header_t {
  uint16_t network_id;
};
// to parse the network information, for example,
// the ota infomation
struct nit_network_t {
  virtual bool nit_network_parse(uint8_t *data, uint16_t size);
  virtual ~nit_network_t(){};
};
// nit ts information, including the freq info
// and service with it
struct nit_ts_t {
  uint16_t ts_id;
  uint16_t original_network_id;
  uint16_t network_id;
  uint32_t freq;
  uint16_t sym;
  uint8_t mod;  // modulation
  uint8_t fec_inner;
  std::vector<std::pair<uint16_t, uint8_t>> service_list;
  virtual bool self_speak(void);
  virtual bool nit_ts_parse(uint8_t *data, uint16_t size);
  virtual ~nit_ts_t();
};
// the nit information
struct nit_info_t {
  nit_header_t *header;
  nit_network_t *net;
  std::vector<nit_ts_t *> ts;
  virtual bool self_speak(void);
  virtual ~nit_info_t();
};
// the factory of nit information, you can inherit this
// struct to define your own parse function
struct nit_factory_t {
  virtual nit_header_t *create_nit_header(uint16_t original_network_id);
  virtual nit_ts_t *create_nit_ts(uint16_t ts_id);
  virtual nit_network_t *create_nit_network(void);
  virtual ~nit_factory_t(){};
};
struct nit : public abstract_ts {
  std::vector<nit_info_t> nit_info;
  nit_factory_t *nit_fac;  // used to generate the nit info
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void);  // 当有区块更新时回调
  virtual bool finish_callback(void);  // 所有区块更新完回调
  nit(uint16_t pid)
      : abstract_ts(pid), nit_info(0), nit_fac(new nit_factory_t()){};
  nit(uint16_t pid, nit_factory_t *fac)
      : abstract_ts(pid), nit_info(0), nit_fac(fac){};
  bool self_speak(void);
  virtual ~nit();
};
}  // namespace ts
#endif  // NIT_HPP__