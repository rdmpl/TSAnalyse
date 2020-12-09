#ifndef PROGRAM_H__
#define PROGRAM_H__
#include <stdint.h>
typedef int32_t error_type_t;
enum {
  MAX_BOUQUET_COUNT = 2,  // max bouquet count = 32 * MAX_BOUQUET_COUNT
  MAX_SERVICE_NAME_LENGTH = 32,
  MAX_TP_COUNT = 128,
  MAX_PROG_COUNT = 1024,
};
enum SORT_TYPE {
  SORT_BY_LCN,
  SORT_BY_SID,
};
struct prog_node_t {
  uint16_t service_id;
  uint16_t lcn;
  uint16_t ts_id;
  uint16_t ts_pos;
  uint16_t pmt_pid;
  uint32_t attr;
  uint32_t bat_attr[MAX_BOUQUET_COUNT];
  int8_t service_name[MAX_SERVICE_NAME_LENGTH];
  int8_t ca_flag;
  bool b_use;
  bool sort_with(prog_node_t *other, SORT_TYPE t, uint8_t as) {
    uint16_t v1 = t == SORT_BY_LCN ? lcn : service_id;
    uint16_t v2 = t == SORT_BY_LCN ? t->lcn : t->service_id;
    return v1 < v2 && as;
  }
};
struct prog_tp_t {
  uint16_t ts_id;
  uint16_t network_id;
  uint16_t freq;
  uint16_t botrate;
  uint16_t sym;
};
struct bat_info_t {
  uint8_t bouquet_id;
  int8_t bouquet_name[MAX_SERVICE_NAME_LENGTH];
};
struct data_base_t {
  uint16_t prog_count;
  prog_node_t prog[MAX_PROG_COUNT];
  bat_info_t bouquet[32 * MAX_PROG_COUNT];
  prog_tp_t prog_tp[MAX_TP_COUNT];
  uint32_t current_attr;
  uint32_t current_bat_attr;
  uint16_t cur_view_list[MAX_PROG_COUNT];
  SORT_TYPE sort_type;
  uint8_t desend_asend_flag;
  error_type_t add_prog(prog_node_t *prog);
  error_type_t modify_prog_by_pos(prog_node_t *prog);
  error_type_t modify_prog_by_sid(prog_node_t *prog);
  error_type_t del_prog_by_pos(uint16 pos);
  error_type_t del_prog_by_sid(uint16 sid);
};
#endif