#include <cstring>
#include <ts/pmt.hpp>
namespace ts {
// TODO: parse the pmt info
// ------loop begin------
// tag: 8
// tag len : 8
// info: leg len
// -------loop end--------
bool pmt_info_t::pmt_info_parse(uint8_t *data, uint16_t len) { return true; }
// TODO: parse the pmt stream
// ------loop begin------
// tag: 8
// tag len: 8
// info: tag len
// ------loop end--------
bool stream_info_t::pmt_stream_parse(uint8_t *data, uint16_t len) {
  return true;
}
pmt_data_t::~pmt_data_t() {
  for (int i = 0; i < stream.size(); ++i) {
    if (stream[i]) {
      delete (stream[i]);
      stream[i] = nullptr;
    }
  }
  if (pmt_info) {
    delete (pmt_info);
  }
}
pmt_info_t *pmt_factory::create_pmt_info(uint16_t prog_number) {
  pmt_info_t *info = new pmt_info_t();
  info->program_number = prog_number;
  return info;
}
stream_info_t *pmt_factory::create_pmt_stream(uint8_t stream_type) {
  stream_info_t *s = new stream_info_t();
  s->stream_type = stream_type;
  return s;
}
bool pmt::parse(uint8_t *data, uint16_t len, void *priv) {
  bool ret = false;
  ret = abstract_ts::parse(data, len, priv);
  if (ret == false) {
    return false;
  }
  uint16_t section_len = getu16len(data + 1, false);
  uint16_t prog_number = data[3] << 8 | data[4];
  uint16_t i = 0;
  for (i = 0; i < this->pmt_data.size(); ++i) {
    if (this->pmt_data[i].pmt_info->program_number == prog_number) {
      break;
    }
  }
  if (i == this->pmt_data.size()) {
    this->pmt_data.push_back(pmt_data_t());
  }
  pmt_data_t &pmt_data = this->pmt_data[i];
  pmt_data.pmt_info = this->pmt_fac->create_pmt_info(prog_number);
  uint16_t prog_info_len = ((data[10] & 0xf) << 8) || data[11];
  pmt_data.pmt_info->pmt_info_parse(data + 12, prog_info_len);
  uint16_t pos = 12 + prog_info_len;
  while (pos < section_len + 3 - 4) {
    stream_info_t *stream = this->pmt_fac->create_pmt_stream(data[pos]);
    stream->pid = ((data[pos + 1] & 0x1F) << 8) | data[pos + 2];
    uint16_t es_len = getu16len(data + pos + 3, false);
    stream->pmt_stream_parse(data + pos + 5, es_len);
    pos += (5 + es_len);
    pmt_data.stream.push_back(stream);
  }
  return true;
}
}  // namespace ts