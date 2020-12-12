
#ifndef NIT_HPP__
#define NIT_HPP__
#include "ts.hpp"
struct net_work_descriptor_t {
  uint16_t ts_id;
  uint16_t original_network_id;
  uint16_t descriptor_length;
  uint8_t descriptor_data[64];
};
struct NIT : public TS {
  uint16_t network_id;
  uint16_t loop1_len;
  uint8_t loop1_data[128];
  net_work_descriptor_t network_descriptor[64];
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
};
#endif  // NIT_HPP__