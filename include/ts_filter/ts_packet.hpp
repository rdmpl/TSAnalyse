#ifndef TS_PACKET_HPP__
#define TS_PACKET_HPP__
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include <cstring>
#include <map>
#include <ts/ts.hpp>
#include <ultilities/ultilities.hpp>
#include <vector>
struct ts_packet_header_t {
  uint8_t sync_byte;
  uint8_t trans_error_indicator;
  uint8_t payload_unit_start_indicator;
  uint8_t trans_priority;
  uint16_t pid;
  uint8_t trans_scramble_control;
  uint8_t ada_field_control;
  uint8_t continuity_conter;
};
struct ts_section_t {
  ts_packet_header_t last_header;
  uint16_t data_len;
  uint8_t buffer[5200];
  std::vector<TS *> ts;
  void reset() {
    memset(&last_header, 0, sizeof(last_header));
    data_len = 0;
    memset(buffer, 0, sizeof(buffer));
  }
};
struct ts_packet_t {
  ts_packet_header_t header;
  uint8_t data[184];
  int file;
  const char *file_path;
  std::map<uint16_t, ts_section_t> filter;
  void run(void);
  bool header_parse(uint8_t *data, uint8_t len);
  bool open_filter(TS *ts) {
    if (ts) {
      uint16_t pid = ts->pid;
      filter[pid].ts.push_back(ts);
      return true;
    }
    return false;
  }
  ts_packet_t(const char *file_path) {
    this->file_path = file_path;
    file = open(file_path, O_RDONLY);
    if (file == -1) {
      LOG_INFO("Open file error.\n");
    }
  }
  ~ts_packet_t() {
    if (file != -1) {
      close(file);
    }
  }
};

#endif  // TS_PACKET_HPP__