#ifndef PROGRAM_H__
#define PROGRAM_H__
#include <stdint.h>
typedef int32_t error_type_t;
enum {
  MAX_BOUQUET_COUNT = 64,  // max bouquet count = 32 * MAX_BOUQUET_COUNT
  MAX_SERVICE_NAME_LENGTH = 32,
  MAX_TP_COUNT = 128,
  MAX_PROG_COUNT = 1024,
};
enum SORT_TYPE {
  SORT_BY_LCN,
  SORT_BY_SID,
};
enum PSI_FLAG {
  PSI_NIT = 0x01,
  PSI_PAT = 0x02,
  PSI_PMT = 0x04,
  PSI_BAT = 0x08,
  PSI_CAT = 0x10,
  PSI_SDT_A = 0x20,
  PSI_SDT_O = 0x40,
};
struct prog_node_t {
  uint16_t service_id;
  uint16_t lcn_value;
  uint16_t network_id;
  uint16_t ts_id;
  uint16_t tp_idx;
  uint16_t pmt_pid;
  uint16_t video_pid;
  uint16_t audio_pid;
  uint16_t pcr_pid;
  uint16_t ecm_pid;
  uint32_t service_price;
  uint32_t psi_flag;  // 标记当前节目存在于哪些表中
  uint32_t reserved;
  uint8_t parent_lock_flag;
  uint8_t scramble_flag;
  uint8_t scramble_type;
  uint8_t pat_version;
  uint8_t pmt_version;
  uint8_t nit_version;
  uint8_t sdt_version;
  uint8_t bat_version;
  uint8_t cat_version;
  uint8_t service_type;  // radio or video
  int8_t service_name[MAX_SERVICE_NAME_LENGTH];
  bool b_use;  // to flag whether this prog is in use
  bool compare_pos(prog_node_t *other, SORT_TYPE t, uint8_t as) {
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
  uint8_t bouquet_name[MAX_SERVICE_NAME_LENGTH];
  uint16_t service_id[128];  // 标记bouquet包含的节目
};
struct data_base_t {
  uint16_t prog_count;  // total count of prog
  uint16_t view_count;  // count of prog in view
  uint16_t tp_count;
  uint16_t bouquet_count;
  prog_node_t prog_list[MAX_PROG_COUNT];
  bat_info_t bouquet_list[MAX_PROG_COUNT];
  prog_tp_t tp_list[MAX_TP_COUNT];
  uint32_t current_type_attr;  // radio or video or all
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