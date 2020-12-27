#ifndef BAT_HPP__
#define BAT_HPP__
#include "ts.hpp"
namespace ts {
#define MAX_BOUQUET_NAME_LEN 32
// This is a struct to handle the bouquet header info
struct bouquet_header_t {
  uint16_t bouquet_id;
  uint16_t start_up_sid;
  uint8_t bouquet_name[MAX_BOUQUET_NAME_LEN];
  virtual bool bouquet_header_parse(uint8_t *data, uint16_t len);
  virtual ~bouquet_header_t(){};
};
// An abstract class to store the data bouquet data.
// it can be different type according to the tag
struct bouquet_element_t {
  uint16_t tag;
  virtual bool bouquet_element_parse(uint8_t *data, uint16_t len) = 0;
  virtual bool self_speak(void) = 0;
  virtual ~bouquet_element_t(){};
};
// the tag of service list
struct bouquet_service_list : public bouquet_element_t {
  std::vector<std::pair<uint16_t, uint8_t>> service;  // serviceid, service_type
  virtual bool bouquet_element_parse(uint8_t *data, uint16_t len);
  virtual bool self_speak(void);
};
// the tag of lcn, just for ucn special use;
struct bouquet_lcn_list : public bouquet_element_t {
  std::vector<std::pair<uint16_t, uint16_t>> service;  // serviceid, lcn
  virtual bool bouquet_element_parse(uint8_t *data, uint16_t len);
  virtual bool self_speak(void);
};
struct bouquet_factory_t {
  virtual bouquet_element_t *create_bouquet_element(uint8_t tag);
  virtual bouquet_header_t *create_bouquet_header(uint16_t id);
  virtual ~bouquet_factory_t(){};
};
struct bouquet_ts_t {
  uint16_t ts_id;
  uint16_t network_id;
  std::vector<bouquet_element_t *> element;
  virtual bool bouquet_ts_parse(uint8_t *data, uint16_t len,
                                bouquet_factory_t *fac);
  virtual bool self_speak(void);
};
// one header with a lot of transport ts
struct bouquet_info_t {
  bouquet_header_t *header;
  std::vector<bouquet_ts_t> ts;
  bool self_speak(void);
};
// bat is a common struct for bouqueit association table
struct bat : public abstract_ts {
  std::vector<bouquet_info_t> bouquet;
  bouquet_factory_t *fac;
  bat(uint16_t pid)
      : abstract_ts(pid), fac(new bouquet_factory_t()), bouquet(0) {}
  bool self_speak(void);
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void);  // 当有区块更新时回调
  virtual bool finish_callback(void);  // 所有区块更新完回调
};
}  // namespace ts
#endif  // BAT_HPP__