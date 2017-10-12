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
    CSP_MASTER_MODE, CSP_SLAVE_MODE
};
enum CMMC_SimplePair_event_t {
    CSP_EVENT_SUCCESS, CSP_EVENT_ERROR
};

#ifndef CSP_DEBUG_BUFFER
  #define CSP_DEBUG_BUFFER 120
#endif
#define MASTER_MODE CSP_MASTER_MODE
#define SLAVE_MODE CSP_SLAVE_MODE
#define EVENT_SUCCESS CSP_EVENT_SUCCESS
#define EVENT_ERROR CSP_EVENT_ERROR

typedef void (*cmmc_simple_pair_err_status_t)(u8 *sa, u8 status, const char* cause);
typedef void (*cmmc_simple_pair_succ_status_t)(u8 *sa, u8 status, const u8* key);
typedef void (*cmmc_debug_cb_t)(const char* cause);

class CMMC_SimplePair
{
  public:
      // constructure
      CMMC_SimplePair() {
        auto cmmc_err_blank = [](u8* sa, u8 status, const char* s) {};
        auto cmmc_succ_blank = [](u8* sa, u8 status, const u8* b) {};
        auto blank = [](u8* sa, u8 status) {};
        this->_sp_callback = blank;
        this->_user_sp_callback = blank;
        this->_user_debug_cb = [](const char* s) { };
        this->_user_cmmc_sp_success_callback = cmmc_succ_blank;
        this->_user_cmmc_sp_error_callback = cmmc_err_blank;
      }
      ~CMMC_SimplePair() {}

      void begin(CMMC_SimplePair_mode_t, u8*, u8*);
      void begin(CMMC_SimplePair_mode_t, u8*, u8*,
          cmmc_simple_pair_succ_status_t, cmmc_simple_pair_err_status_t);
      void start();
      void mode(CMMC_SimplePair_mode_t);
      int mode();
      void set_pair_key(u8 *);
      void set_message(u8 *);
      void add_listener(simple_pair_status_cb_t);
      void add_debug_listener(cmmc_debug_cb_t);
      void on(CMMC_SimplePair_event_t, cmmc_simple_pair_succ_status_t);
      void on(CMMC_SimplePair_event_t, cmmc_simple_pair_err_status_t);
  private:
      char debug_buffer[CSP_DEBUG_BUFFER];
      u8 _pair_key[16];
      u8 _message[16];
      CMMC_SimplePair_mode_t _mode;
      simple_pair_status_cb_t _sp_callback = NULL;
      simple_pair_status_cb_t _user_sp_callback = NULL;
      cmmc_debug_cb_t _user_debug_cb;
      cmmc_simple_pair_err_status_t _user_cmmc_sp_error_callback = NULL;
      cmmc_simple_pair_succ_status_t _user_cmmc_sp_success_callback = NULL;
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
      void debug_cb(const char*);

};

#endif //CMMC_SimplePair_H
