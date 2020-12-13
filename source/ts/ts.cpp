#include <algorithm>
#include <cstring>
#include <ts/ts.hpp>
TS::TS() {
  this->max_ts_num = 0;
  this->unit.resize(0);
  this->unit_num = 0;
  this->pid = 0;
  this->filer_id = 0;
  this->last_crc32 = 0;
  this->table_id = 0;
}
TS::TS(uint16_t pid, uint16_t count) : TS() {
  this->pid = pid;
  this->b_need_notify = true;
  this->max_ts_num = count;
  this->unit.resize(count);
}
void ts_unit_t::restart(bool parsed_flag) {
  rcv_number = 0;
  last_section_number = 0;
  memset(crc32, 0, sizeof(crc32));
  version_number = 0;
  this->b_has_parsed = parsed_flag;
}
bool ts_unit_t::operator<(const ts_unit_t &other) const {
  return this->unit_id < other.unit_id ||
         (this->unit_id == other.unit_id &&
          this->last_section_number < other.last_section_number);
}
void TS::reset(bool flag) {
  this->table_id = 0;
  this->unit_num = 0;
  this->last_crc32 = 0;
  this->b_need_notify = flag;
  for (uint16_t i = 0; i < this->unit.size(); ++i) {
    this->unit[i].restart(false);
  }
}
uint32_t TS::cal_crc32(void) {
  for (uint8_t i = 0; i < lengthof(this->unit); ++i) {
    this->unit[i].rcv_cnt = 0;
    this->unit[i].rcv_number = 0;
    this->unit[i].b_has_parsed = true;
  }
  std::sort(this->unit.begin(), this->unit.end());
  // TODO: calculate the crc32 of units
  return 0;
}
bool TS::check_finish(ts_unit_t &cur_unit) {
  uint16_t i = 0;
  bool ret = false;
  if (cur_unit.rcv_number == cur_unit.last_section_number + 1) {
    cur_unit.rcv_cnt++;
    uint16_t min_cnt = this->unit[0].rcv_cnt;
    uint16_t max_cnt = this->unit[0].rcv_cnt;
    for (i = 1; i < this->unit_num; i++) {
      min_cnt = std::min(min_cnt, this->unit[i].rcv_cnt);
      max_cnt = std::max(max_cnt, this->unit[i].rcv_cnt);
    }
    if (max_cnt - min_cnt > 6) {
      reset(true);  // 值越大，越精确，但反应越慢
      update_callback();
    } else if (min_cnt > 2) {
      uint32_t crc32 = cal_crc32();
      if (this->last_crc32 != crc32) {
        LOG_INFO(
            "Note: received all units, and the crc32 "
            "changed.[0x%08x][0x%08x]\n",
            this->last_crc32, crc32);
        reset(true);
        update_callback();
      } else {
        for (i = 0; i < this->unit_num; ++i) {
          this->unit[i].restart(true);
        }
        ret = true;
        if (b_need_notify = true) {
          b_need_notify = false;
          finish_callback();
        }
      }
    } else {
      cur_unit.restart(true);
    }
  }
  return ret;
}
bool TS::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  if (len < 3 || data == nullptr) {
    LOG_INFO("error\n");
    return false;
  }
  this->table_id = data[0];
  uint16_t section_length = ((data[1] & 0x0F) << 8) | data[1];
  uint16_t pos = 3 + section_length;
  if (pos != len) {
    LOG_INFO("Note: data length not match.[%d][%d].\n", pos, len);
  }
  if (pos < 4) {
    LOG_INFO("Error: rcv data too short.[%d]\n", pos);
    return false;
  }
  uint32_t crc32 = (data[pos - 4] << 24) | (data[pos - 3] << 16) |
                   (data[pos - 2] << 8) | data[pos - 1];
  uint16_t unit_id = (data[3] << 8) | data[4];
  uint8_t i = 0;
  while (i < this->unit_num && this->unit[i].unit_id != unit_id) {
    ++i;
  }
  if (i >= this->unit.size()) {
    LOG_INFO("Error: current ts unit too much.[%d].\n", this->table_id);
    reset(true);  // 全部重新接收
    return false;
  }
  if (i == this->unit_num) {
    this->unit_num++;
  }
  ts_unit_t &unit = this->unit[i];
  unit.unit_id = unit_id;
  unit.version_number = (data[5] >> 1) & 0x1F;
  uint8_t section_number = data[6];
  unit.last_section_number = data[7];
  if (section_number >= lengthof(unit.crc32)) {
    LOG_INFO("Error: too much section.[%d][%d]", this->table_id,
             section_number);
    return true;
  }
  if (unit.crc32[section_number] == 0) {
    ret = true ^ unit.b_has_parsed;
    unit.rcv_number++;
    unit.crc32[section_number] = crc32;
  } else if (unit.crc32[section_number] != crc32) {
    unit.restart(true);  // 说明当前unit的crc更新了，让当前unit重新接收
    unit.rcv_cnt = 0;
  }
  check_finish(unit);
  LOG_INFO("ret = %d", ret);
  return ret;
}
