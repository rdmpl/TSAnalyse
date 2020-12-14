
#ifndef NIT_HPP__
#define NIT_HPP__

#include "ts.hpp"
#include "vector"
struct nit_service_info_t {
  uint16_t service_id;
  uint16_t service_type;
};
struct tp_info_t {
  prog_tp_t tp;
  nit_service_info_t service_list[32];
  uint16_t service_count;
  bool freq_parse(uint8_t *data, uint8_t len);
  bool service_list_parse(uint8_t *data, uint8_t len);
  virtual bool loop2_parse(uint8_t *data, uint16_t len);
};
struct nit_tp_info_t {
  uint16_t network_id;
  tp_info_t tp[64];
  uint16_t tp_count;
  virtual bool loop1_parse(uint8_t *data, uint16_t len);
  virtual ~nit_tp_info_t(){};
};
struct NIT : public TS {
  std::vector<nit_tp_info_t> tp_info;
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void);  // 当有区块更新时回调
  virtual bool finish_callback(void);  // 所有区块更新完回调
  bool dump(void) {
    for (uint16_t i = 0; i < this->ts_num; ++i) {
      printf("\n%d: network_id = %x\n", i, tp_info[i].network_id);
      for (uint16_t j = 0; j < tp_info[i].tp_count; ++j) {
        printf("\n%d:ts_id: 0x%x, frq: %d, sym: %d, mod = %d\n", j, tp_info[i].tp[j].tp.ts_id,
               tp_info[i].tp[j].tp.freq, tp_info[i].tp[j].tp.sym, tp_info[i].tp[j].tp.mod);
        for (uint16_t k = 0; k < tp_info[i].tp[j].service_count; k++) {
          printf("0x%x  ", tp_info[i].tp[j].service_list[k].service_id);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
  NIT(uint16_t pid, uint16_t max_tp_count) : TS(pid, max_tp_count) { tp_info.resize(max_tp_count); }
  // TODO: 要添加loop1和loop2的解析函数
};
#endif  // NIT_HPP__