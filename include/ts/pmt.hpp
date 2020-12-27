
#ifndef PMT_HPP__
#define PMT_HPP__
#include "ts.hpp"
namespace ts {
// The basic clas storing the pmt info data, such as program_number
struct pmt_info_t {
  uint16_t program_number;
  uint16_t pcr_pid;
  virtual bool pmt_info_parse(uint8_t *data, uint16_t len);
  virtual ~pmt_info_t(){};
};
// The basic class, which store the es info, such as video and audio
struct stream_info_t {
  uint8_t stream_type;
  uint16_t pid;
  virtual bool pmt_stream_parse(uint8_t *data, uint16_t len);
  virtual ~stream_info_t(){};
};
// The pmt data factory, which create prog_info
struct pmt_factory {
  virtual pmt_info_t *create_pmt_info(uint16_t program_number);
  // you can create diffrent element stream type by the stream_type
  virtual stream_info_t *create_pmt_stream(uint8_t stream_type);
  virtual ~pmt_factory(){};
};
// Directly used by the pmt struct, to abstractly parse pmt data
struct pmt_data_t {
  pmt_info_t *pmt_info;
  std::vector<stream_info_t *> stream;
  ~pmt_data_t();
};

// common pmt parse function
struct pmt : public abstract_ts {
  std::vector<pmt_data_t> pmt_data;
  pmt_factory *pmt_fac;
  virtual bool parse(uint8_t *data, uint16_t len, void *priv);
  pmt(uint16_t pid) : abstract_ts(pid), pmt_data(0){};
  pmt(uint16_t pid, pmt_factory *fac) : abstract_ts(pid), pmt_fac(fac){};
  virtual ~pmt();
};
}  // namespace ts
#endif  // PMT_HPP__