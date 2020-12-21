#include <algorithm>
#include <cstring>
#include <ts/ts.hpp>
namespace ts {
abstract_ts::abstract_ts() {
  this->max_ts_num = 0;
  this->unit.resize(0);
  this->unit_num = 0;
  this->pid = 0;
  this->filer_id = 0;
  this->last_crc32 = 0;
  this->ts_num = 0;
  memset(slot, 0, sizeof(slot));
  memset(mask, 0, sizeof(mask));
}
abstract_ts::abstract_ts(uint16_t pid) : abstract_ts() {
  this->pid = pid;
  this->b_need_notify = true;
}
bool abstract_ts::check_match(uint8_t *data, uint8_t len) {
  uint8_t i = 0;
  for (i = 0; i < len && i < lengthof(this->slot); ++i) {
    if ((data[i] & mask[i]) != slot[i]) {
      break;
    }
  }
  return i == lengthof(this->slot);
}
void ts_unit_t::restart(bool parsed_flag) {
  rcv_number = 0;
  last_section_number = 0;
  crc32.clear();
  version_number = 0;
  this->b_has_parsed = parsed_flag;
}
bool ts_unit_t::operator<(const ts_unit_t &other) const {
  // unit id
  // 从小到大会导致一些默认的0值排前面出问题，需要排序的时候一定不能加上无效的
  return this->unit_id < other.unit_id ||
         (this->unit_id == other.unit_id &&
          this->last_section_number < other.last_section_number);
}
void abstract_ts::reset(bool flag) {
  this->unit_num = 0;
  this->last_crc32 = 0;
  this->b_need_notify = flag;
  for (uint16_t i = 0; i < this->unit.size(); ++i) {
    this->unit[i].restart(false);
  }
}
bool abstract_ts::set_filter_table_id(uint8_t table_id) {
  slot[0] = table_id;
  mask[0] = 0xFF;
  return true;
}
bool abstract_ts::set_filter_param(uint8_t *s, uint8_t *m, uint8_t depth) {
  if (depth > 8) {
    depth = 8;
  }
  if (s != nullptr && m != nullptr) {
    for (uint8_t i = 0; i < depth; ++i) {
      slot[i] = s[i];
      mask[i] = m[i];
    }
  }
  return true;
}
uint32_t abstract_ts::cal_crc32(void) {
  for (uint8_t i = 0; i < this->unit.size(); ++i) {
    this->unit[i].rcv_cnt = 0;
    this->unit[i].rcv_number = 0;
    this->unit[i].b_has_parsed = true;
  }
  std::sort(this->unit.begin(),
            this->unit.end());  // this->unit.end());
  // TODO: calculate the crc32 of units
  return 0;
}
bool abstract_ts::check_finish(ts_unit_t &cur_unit) {
  uint16_t i = 0;
  bool ret = false;
  if (cur_unit.rcv_number == cur_unit.last_section_number + 1) {
    cur_unit.rcv_cnt++;
    cur_unit.rcv_number = 0;
    uint16_t min_cnt = this->unit[0].rcv_cnt;
    uint16_t max_cnt = this->unit[0].rcv_cnt;
    for (i = 1; i < this->unit.size(); i++) {
      min_cnt = std::min(min_cnt, this->unit[i].rcv_cnt);
      max_cnt = std::max(max_cnt, this->unit[i].rcv_cnt);
    }

    if (max_cnt - min_cnt > 6) {
      for (i = 0; i < this->unit.size(); i++) {
        LOG_INFO("%d: 0x%x...", i, this->unit[i].unit_id);
      }
      LOG_INFO("received new units.\n");
      LOG_INFO("max_cnt: %d, min_cnt: %d, unit_cnt = %d", max_cnt, min_cnt,
               this->unit.size());
      reset(true);  // 值越大，越精确，但反应越慢
      update_callback();
    } else if (min_cnt > 3) {
      uint32_t crc32 = cal_crc32();
      if (this->last_crc32 != crc32) {
        LOG_INFO(
            "Note: received all units, and the crc32 "
            "changed.[0x%08x][0x%08x]\n",
            this->last_crc32, crc32);
        reset(true);
        update_callback();
      } else {
        for (i = 0; i < this->unit.size(); ++i) {
          this->unit[i].restart(true);
        }
        ret = true;
        if (b_need_notify == true) {
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
bool abstract_ts::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  if (len < 3 || data == nullptr) {
    LOG_INFO("error data too short\n");
    return false;
  }
  uint16_t section_length = ((data[1] & 0x0F) << 8) | data[1];
  uint16_t pos = 3 + section_length;
  if (pos != len) {
    // LOG_INFO("Note: data length not match.[%d][%d].\n", pos, len);
  }
  if (pos < 4) {
    LOG_INFO("Error: rcv data too short.[%d]\n", pos);
    return false;
  }
  uint32_t crc32 = (data[pos - 4] << 24) | (data[pos - 3] << 16) |
                   (data[pos - 2] << 8) | data[pos - 1];
  uint16_t unit_id = (data[3] << 8) | data[4];
  uint16_t i = 0;
  while (i < this->unit.size() && this->unit[i].unit_id != unit_id) {
    ++i;
  }
  if (i >= this->unit.size()) {
    unit.push_back(ts_unit_t());
  }
  ts_unit_t &unit = this->unit[i];
  unit.unit_id = unit_id;
  unit.version_number = (data[5] >> 1) & 0x1F;
  uint8_t section_number = data[6];
  unit.last_section_number = data[7];
  if (section_number >= unit.crc32.size()) {
    unit.crc32.resize(unit.last_section_number + 1, 0);
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
  return ret;
}
}  // namespace ts
