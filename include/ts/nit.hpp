
#ifndef NIT_HPP__
#define NIT_HPP__

#include "ts.hpp"
#include "vector"
namespace ts {
struct nit : public abstract_ts {
  std::vector<prog_tp_t> tp_info;
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void);  // 当有区块更新时回调
  virtual bool finish_callback(void);  // 所有区块更新完回调
  virtual bool loop1_parse(uint8_t *data, uint16_t len);
  bool dump(void) {
    for (uint16_t i = 0; i < this->tp_info.size(); ++i) {
      printf("\n%d: network_id = %x\n", i, tp_info[i].network_id);
      printf("ts_id: 0x%x, frq: %d, sym: %d, mod = %d\n", tp_info[i].ts_id,
             tp_info[i].freq, tp_info[i].sym, tp_info[i].mod);
      for (uint16_t j = 0; j < tp_info[i].service_info.size(); ++j) {
        printf("0x%x ", tp_info[i].service_info[j].first);
      }
      printf("\n");
    }
    return true;
  }
  nit(uint16_t pid) : abstract_ts(pid), tp_info() {}
  // TODO: 要添加loop1和loop2的解析函数
};
}  // namespace ts
#endif  // NIT_HPP__