#ifndef CMMC_SimplePair_H
#define CMMC_SimplePair_H

#include "ESP8266WiFi.h"
#include <functional>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
  #include "user_interface.h"
  #include "simple_pair.h"
}
#endif

enum CMMC_SimplePair_mode_t {
    MODE_AP, MODE_STA
};

class CMMC_SimplePair
{
  public:
      // constructure
      CMMC_SimplePair() {
        u8 tmp[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
        this->set_pair_key(tmp);
      }

      ~CMMC_SimplePair() {}

      void setup();
      void begin(CMMC_SimplePair_mode_t);
      void mode(CMMC_SimplePair_mode_t);
      void set_pair_key(u8 *);
  private:
    CMMC_SimplePair_mode_t _mode;
    simple_pair_status_cb_t _sp_callback;
    u8 tmp_key[16];
    void on_sp_st_finish(u8*);
    void on_sp_st_ap_recv_neg(u8*);
    void on_sp_st_wait_timeout(u8*);
    void on_sp_st_send_error(u8*);
    void on_sp_st_key_install_err(u8*);
    void on_sp_st_key_overlap_err(u8*);
    void on_sp_st_op_error(u8*);
    void on_sp_st_unknown_error(u8*);
    void on_sp_st_max(u8*);
    void _simple_pair_init();
};

#endif //CMMC_SimplePair_H
