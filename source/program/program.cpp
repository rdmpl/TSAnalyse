#include "program/program.hpp"
error_type_t data_base_t::add_prog(prog_node_t *prog) {
  int i = 0;
  for (i = 0; i < this->prog_count; i++) {
    if (this->prog_list[i].b_use == false) {
      // 替换被标记为无效的节目
      break;
    }
  }
  prog_node_t *prog_in_base = &(this->prog_list[i]);
  if (i == this->prog_count) {
    this->prog_count++;
  }
  memset(prog_in_base, 0, sizeof(prog_in_base[0]));
  memcpy(prog_in_base, prog, sizeof(prog_in_base[0]));
  prog_in_base->b_use = true;  // 有效标记
  // TODO: link the bouquet data;
  // TODO: link the tp data;
  for (i = 0; i < this->tp_count; i++) {
    if (this->tp_list[i].ts_id == prog->ts_id) {
      break;
    }
  }
  prog_in_base->tp_idx = i;
  if (i == this->tp_count) {
    memset(&this->tp_list[i], 0, sizeof(this->tp_list[i]));
    this->tp_list[i].ts_id = prog->ts_id;
    this->tp_list[i].network_id = prog->network_id;
    this->tp_count++;
  }
}