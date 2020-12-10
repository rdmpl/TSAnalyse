#include "program/program.hpp"

#include <algorithm>
#include <cstring>
error_type_t data_base_t::add_prog(const prog_node_t &prog) {
  int i = 0;
  for (i = 0; i < this->prog_count; i++) {
    if (this->prog_list[i].b_use == false) {
      // 替换被标记为无效的节目
      break;
    }
  }
  if (i == this->prog_count) {
    this->prog_count++;
  }
  return update_prog(prog, i);
}
uint16_t data_base_t::lookup_node(
    const std::pair<uint16_t, uint16_t> &target) const {
  uint16_t mode = target.first;
  uint16_t val = target.second;
  uint16_t idx = this->prog_count;
  switch (mode) {
    case 0:
      idx = val;
      break;
    case 1:
      for (uint16_t i = 0; i < this->prog_count; i++) {
        if (this->prog_list[i].service_id == val) {
          idx = i;
          break;
        }
      }
      break;
    case 2:
      for (uint16_t i = 0; i < this->prog_count; i++) {
        if (this->prog_list[i].lcn_value == val) {
          idx = i;
          break;
        }
      }
      break;
    default:
      break;
  }
  return idx;
}
error_type_t data_base_t::modify_prog(
    const prog_node_t &prog, const std::pair<uint16_t, uint16_t> &target) {
  uint16_t i = lookup_node(target);
  return update_prog(prog, i);
}
error_type_t data_base_t::update_prog(const prog_node_t &prog, uint16_t idx) {
  if (idx >= this->prog_count) {
    return -1;
  }
  prog_node_t &tar = this->prog_list[idx];
  memcpy(&tar, &prog, sizeof(prog));
  tar.b_use = true;
  uint16_t i = 0;
  for (i = 0; i < this->tp_count; i++) {
    if (this->tp_list[i].ts_id == prog.ts_id) {
      break;
    }
  }
  tar.tp_idx = i;
  if (i == this->tp_count) {
    memset(&this->tp_list[i], 0, sizeof(this->tp_list[i]));
    this->tp_list[i].ts_id = prog.ts_id;
    this->tp_list[i].network_id = prog.network_id;
    this->tp_count++;
  }
  return 0;
}
error_type_t data_base_t::del_prog(
    const std::pair<uint16_t, uint16_t> &target) {
  uint16_t idx = lookup_node(target);
  if (idx >= this->prog_count) {
    return -1;
  }
  this->prog_list[idx].b_use = false;
  return 0;
}
error_type_t data_base_t::refresh(void) {
  uint16_t i = 0, j = 0;
  bool b_change = false;
  for (i = 0; i < this->prog_count; i++) {
    if (this->prog_list[i].b_use == true) {
      memcpy(&this->prog_list[j], &this->prog_list[i],
             sizeof(this->prog_list[i]));
      ++j;
    }
  }
  this->prog_count = j;
  auto sort_method = [this](prog_node_t &p1, prog_node_t &p2) {
    return p1.compare_pos(p2, this->sort_type);
  };
  std::sort(this->prog_list, &this->prog_list[this->prog_count], sort_method);
  // TODO: 更新完数据库之后需要及时更新观察者列表
  return 0;
}
