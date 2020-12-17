#ifndef BAT_HPP__
#define BAT_HPP__
#include "ts.hpp"
/// 允许以下类被继承，从而使其功能得到扩展
namespace ts {
struct ts_bat_t {
  bat_info_t bat;
  bool loop1_parse(uint8_t *data, uint16_t len);
  bool loop2_parse(uint8_t *data, uint16_t len);
  virtual ~ts_bat_t() { LOG_INFO(); }
};
struct bat : public abstract_ts {
  std::vector<ts_bat_t> bat_info;
  bat(uint16_t pid, uint16_t max_cnt) : abstract_ts(pid, max_cnt) {
    this->bat_info.resize(max_cnt);
  }
  virtual bool dump();
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  virtual bool update_callback(void);  // 当有区块更新时回调
  virtual bool finish_callback(void);  // 所有区块更新完回调
};
}  // namespace ts
#endif  // BAT_HPP__