#ifndef BAT_HPP__
#define BAT_HPP__
#include "ts.hpp"
/// 允许以下类被继承，从而使其功能得到扩展
namespace ts {
struct bat : public abstract_ts {
  std::vector<bat_info_t> bat_info;
  bat(uint16_t pid) : abstract_ts(pid), bat_info() {}
  virtual bool dump();
  bool loop1_parse(uint8_t *data, uint16_t len, bat_info_t &bat_info);
  bool loop2_parse(uint8_t *data, uint16_t len, bat_info_t &bat_info);
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void);  // 当有区块更新时回调
  virtual bool finish_callback(void);  // 所有区块更新完回调
};
}  // namespace ts
#endif  // BAT_HPP__