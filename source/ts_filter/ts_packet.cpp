#include <cstring>
#include <ts_filter/ts_packet.hpp>
bool ts_packet_t::header_parse(uint8_t *data, uint8_t len) {
  header.sync_byte = data[0];
  header.trans_error_indicator = (data[1] & 0x80) >> 7;
  header.payload_unit_start_indicator = (data[1] & 0x40) >> 6;
  header.trans_priority = (data[1] & 0x20) >> 5;
  header.pid = ((data[1] & 0x1f) << 8) | data[2];
  header.trans_scramble_control = (data[3] >> 6) & 0x3;
  header.ada_field_control = (data[3] >> 4) & 0x3;
  header.continuity_conter = data[3] & 0xf;
  if (header.sync_byte != 0x47) {
    // LOG_INFO("error, sync_byte not ok");
    return false;
  }
  return true;
}
void ts_packet_t::run(void) {
  uint8_t data[200];
  if (file < 0) {
    LOG_INFO("error:");
    return;
  }
  while (1) {
    memset(data, 0, sizeof(data));
    uint16_t len = read(file, data, 188);
    if (len < 188) {
      LOG_INFO("file read finished.");
      return;
    }
    header_parse(data, 188);
    if (header.sync_byte != 0x47) {
      uint8_t i = 0;
      while (i < 188 && data[i] != 0x47) {
        ++i;
      }
      if (i < 188) {
        read(file, data, i);  // sync
      }
      // LOG_INFO("sync");
      continue;
    }
    auto &section = filter[header.pid];
    if (section.ts.size() == 0) {
      continue;  // 不关心的section
    }
    len = 0;
    if (header.payload_unit_start_indicator == 1) {
      if (section.data_len > 0) {
        // TODO: callback
        // LOG_INFO("ok....pid = 0x%04x, len = %d", header.pid,
        // section.data_len);
        for (uint16_t i = 0; i < section.ts.size(); ++i) {
          if (section.ts[i] != nullptr &&
              section.ts[i]->check_match(section.buffer, section.data_len)) {
            section.ts[i]->parse(section.buffer, section.data_len, nullptr);
          }
        }
      }
      section.reset();
      uint16_t begin = 5 + data[4];
      len = 188 - begin;
      memcpy(section.buffer, data + begin, len);
    } else {
      if ((section.last_header.continuity_conter + 1) % 16 !=
          header.continuity_conter) {
        // LOG_INFO("error: continuous error");
        section.reset();
        continue;
      }
      if (section.data_len + 184 >= 4096) {
        section.reset();
        continue;
      }
      memcpy(section.buffer + section.data_len, data + 4, 184);
      len = 184;
    }
    section.data_len += len;
    if (section.data_len > 3990) {
      LOG_INFO("error: data too long, pid = 0x%4x, len = %d", header.pid,
               section.data_len);
    }
    memcpy(&section.last_header, &header, sizeof(header));
  }
  LOG_INFO("finished");
}