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
    CSP_MODE_AP, CSP_MODE_STA
};

enum CMMC_SimplePair_event_t {
    CSP_EVENT_SUCCESS, CSP_EVENT_ERROR
};


class CMMC_SimplePair
{
  public:
      // constructure
      CMMC_SimplePair() {
        auto blank = [](u8* sa, u8 status) {};
        this->_user_sp_callback = blank;
        this->_user_sp_success_callback = blank;
        this->_user_sp_error_callback = blank;
      }
      ~CMMC_SimplePair() {}

      void begin(CMMC_SimplePair_mode_t, u8*);
      void start();
      void mode(CMMC_SimplePair_mode_t);
      int mode();
      void set_pair_key(u8 *);
      void add_listener(simple_pair_status_cb_t);
      void on(CMMC_SimplePair_event_t, simple_pair_status_cb_t);
  private:
    u8 tmp_key[16];
    CMMC_SimplePair_mode_t _mode;
    simple_pair_status_cb_t _sp_callback = NULL;
    simple_pair_status_cb_t _user_sp_callback = NULL;
    simple_pair_status_cb_t _user_sp_success_callback = NULL;
    simple_pair_status_cb_t _user_sp_error_callback = NULL;
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
