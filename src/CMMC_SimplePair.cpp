#include "CMMC_SimplePair.h"

void CMMC_SimplePair::debug_cb(const char* str) {
  this->_user_debug_cb(str);
}

void show_key(u8 *buf, u8 len) {
  u8 i;
  for (i = 0; i < len; i++)
    Serial.printf("%02x,%s", buf[i], (i%16 == 15?"\n":" "));
    // sprintf(debug_buffer, "%02x,%s", buf[i], (i%16 == 15?"\n":" "));
    // debug_cb(debug_buffer);
}

void CMMC_SimplePair::mode(CMMC_SimplePair_mode_t mode) {
  this->_mode = mode;
}

void CMMC_SimplePair::set_pair_key(u8 *tmp) {
  memcpy(this->tmp_key, tmp, 16);
}

void CMMC_SimplePair::on_sp_st_finish(u8* sa) {
  if (this->_mode == CSP_SLAVE_MODE)
  {
    u8 ex_key[16];
    simple_pair_get_peer_ref(NULL, NULL, ex_key);
    sprintf(this->debug_buffer, "Simple Pair: AP FINISH\n");
    debug_cb(this->debug_buffer);
    sprintf(this->debug_buffer, "slave mac: \n");
    debug_cb(this->debug_buffer);
    show_key(sa, 6);
    Serial.println();
    sprintf(this->debug_buffer, "exkey: \n");
    debug_cb(this->debug_buffer);
    show_key(ex_key, 16);
    Serial.println();

    /* if test ok , deinit simple pair */
    simple_pair_deinit();
  }
  else
  {
    u8 ex_key[16];
    simple_pair_get_peer_ref(NULL, NULL, ex_key);
    sprintf(this->debug_buffer, "Simple Pair: STA FINISH, Ex_key ");
    debug_cb(this->debug_buffer);
    show_key(ex_key, 16);
    simple_pair_deinit();
  }
}

int CMMC_SimplePair::mode() {
  return this->_mode;
}

void CMMC_SimplePair::on(CMMC_SimplePair_event_t evt, simple_pair_status_cb_t cb) {
  if (evt == CSP_EVENT_SUCCESS && cb != NULL) {
    this->_user_sp_success_callback = cb;
  }
}

void CMMC_SimplePair::on(CMMC_SimplePair_event_t evt, cmmc_simple_pair_status_cb_t cb) {
  if (evt == CSP_EVENT_ERROR && cb != NULL) {
    this->_user_sp_error_callback = cb;
  }
}

void CMMC_SimplePair::_simple_pair_init() {
  int ret;
  static CMMC_SimplePair *_this = this;

  if (this->_mode == CSP_MASTER_MODE) {
    wifi_set_opmode(SOFTAP_MODE);
    /* init simple pair */
    ret = simple_pair_init();
    if (ret) {
      sprintf(this->debug_buffer, "Simple Pair: init error, %d\n", ret);
      debug_cb(this->debug_buffer);
      return;
    }
    /* register simple pair status callback function */
    ret = register_simple_pair_status_cb(this->_sp_callback);
    if (ret) {
      sprintf(this->debug_buffer, "Simple Pair: register status cb error, %d\n", ret);
      debug_cb(this->debug_buffer);
      return;
    }
    else {
      sprintf(this->debug_buffer, "Simple Pair: AP Enter Announce Mode ...\n");
      debug_cb(this->debug_buffer);
      /* ap must enter announce mode , so the sta can know which ap is ready to simple pair */
      ret = simple_pair_ap_enter_announce_mode();
      if (ret) {
        sprintf(this->debug_buffer, "Simple Pair: AP Enter Announce Mode Error, %d\n", ret);
        debug_cb(this->debug_buffer);
        return;
      }
    }
  }
  else if (this->_mode == SLAVE_MODE) {
    wifi_set_opmode(STATION_MODE);
    /* init simple pair */
    ret = simple_pair_init();
    if (ret) {
      sprintf(this->debug_buffer, "Simple Pair: init error, %d\n", ret);
      debug_cb(this->debug_buffer);
      return;
    }
    /* register simple pair status callback function */
    ret = register_simple_pair_status_cb(this->_sp_callback);
    if (ret) {
      sprintf(this->debug_buffer, "Simple Pair: register status cb error, %d\n", ret);
      debug_cb(this->debug_buffer);
      return;
    }

    sprintf(this->debug_buffer, "Simple Pair: STA Enter Scan Mode ...\n");
    debug_cb(this->debug_buffer);
    ret = simple_pair_sta_enter_scan_mode();
    if (ret) {
      sprintf(this->debug_buffer, "Simple Pair: STA Enter Scan Mode Error, %d\n", ret);
      debug_cb(this->debug_buffer);
      return;
    } else {
      /* scan ap to searh which ap is ready to simple pair */
      sprintf(this->debug_buffer, "Simple Pair: STA Scan AP ...\n");
      debug_cb(this->debug_buffer);
      wifi_station_scan(NULL, [](void *arg, STATUS status) {
        int ret;
        if (status == OK) {
          Serial.println("scan_done");
          struct bss_info *bss_link = (struct bss_info *)arg;
          while (bss_link != NULL) {
            if (bss_link->simple_pair) {
              sprintf(_this->debug_buffer,
                "Simple Pair: bssid %02x:%02x:%02x: %02x:%02x:%02x Ready!\n",
                bss_link->bssid[0], bss_link->bssid[1],
                bss_link->bssid[2], bss_link->bssid[3],
                bss_link->bssid[4], bss_link->bssid[5]);
              _this->debug_cb(_this->debug_buffer);
              simple_pair_set_peer_ref(bss_link->bssid, _this->tmp_key, NULL);
              ret = simple_pair_sta_start_negotiate();
              if (ret) {
                sprintf(_this->debug_buffer, "Simple Pair: STA start NEG Failed\n");
                _this->debug_cb(_this->debug_buffer);
              }
              else {
                sprintf(_this->debug_buffer, "Simple Pair: STA start NEG OK\n");
                _this->debug_cb(_this->debug_buffer);
              }
              break;
            }
            // bss_link = bss_link->next->stqe_next;
            Serial.println("next...");
            bss_link = bss_link->next;
          }
        } else {
          sprintf(_this->debug_buffer, "err, scan status %d\n", status);
          _this->debug_cb(_this->debug_buffer);
        }
      });
    }
  }
  else {

  }
}

void CMMC_SimplePair::on_sp_st_ap_recv_neg(u8* sa) {
  /* AP recv a STA's negotiate request */
  sprintf(this->debug_buffer, "Simple Pair: Recv STA Negotiate Request\n");
  debug_cb(this->debug_buffer);

  /* set peer must be called, because the simple pair need to know what peer mac is */
  u8 ex_key[16] = { 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
    0x01, 0x02, 0x03, 0x04, 0xFF, 0xFF, 0xFF, 0xFF };
  simple_pair_set_peer_ref(sa, this->tmp_key, ex_key);
  simple_pair_ap_start_negotiate();
}

void CMMC_SimplePair::on_sp_st_wait_timeout(u8* sa) { }
void CMMC_SimplePair::on_sp_st_send_error(u8* sa) { }
void CMMC_SimplePair::on_sp_st_key_install_err(u8* sa) { }
void CMMC_SimplePair::on_sp_st_key_overlap_err(u8* sa) { }
void CMMC_SimplePair::on_sp_st_op_error(u8* sa) { }
void CMMC_SimplePair::on_sp_st_unknown_error(u8* sa) { }
void CMMC_SimplePair::on_sp_st_max(u8* sa) { }


void CMMC_SimplePair::add_debug_listener(cmmc_debug_cb_t cb) {
  if (cb != NULL) {
    this->_user_debug_cb = cb;
  }
}

void CMMC_SimplePair::begin(CMMC_SimplePair_mode_t mode, u8 *key,
  simple_pair_status_cb_t succ_cb, cmmc_simple_pair_status_cb_t err_cb) {
    this->on(CSP_EVENT_SUCCESS, succ_cb);
    this->on(CSP_EVENT_ERROR, err_cb);
    this->begin(mode, key);
}

void CMMC_SimplePair::begin(CMMC_SimplePair_mode_t mode, u8 *key) {
    this->mode(mode);
    this->set_pair_key(key);
    static CMMC_SimplePair* _this = this;
    this->_sp_callback = [](u8 *sa, u8 status) {
        sprintf(_this->debug_buffer, "event %d\r\n", status);
        _this->debug_cb(_this->debug_buffer);
        _this->_user_sp_callback(sa, status);
        switch (status) {
          case 0:
            _this->on_sp_st_finish(sa);
            _this->_user_sp_success_callback(sa, status);
            break;
          case SP_ST_AP_RECV_NEG:
            _this->on_sp_st_ap_recv_neg(sa);
            break;
          case SP_ST_WAIT_TIMEOUT:
            _this->on_sp_st_wait_timeout(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_WAIT_TIMEOUT");
            break;
          case SP_ST_SEND_ERROR:
            _this->on_sp_st_send_error(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_SEND_ERROR");
            break;
          case SP_ST_KEY_INSTALL_ERR:
            _this->on_sp_st_key_install_err(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_KEY_INSTALL_ERR");
            break;
          case SP_ST_KEY_OVERLAP_ERR:
            _this->on_sp_st_key_overlap_err(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_KEY_OVERLAP_ERR");
            break;
          case SP_ST_OP_ERROR:
            _this->on_sp_st_op_error(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_OP_ERROR");
            break;
          case SP_ST_UNKNOWN_ERROR:
            _this->on_sp_st_unknown_error(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_UNKNOWN_ERROR");
            break;
          case SP_ST_MAX:
            _this->on_sp_st_max(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_MAX");
            break;
          default:
            _this->on_sp_st_unknown_error(sa);
            _this->_user_sp_error_callback(sa, status, "SP_ST_UNKNOWN_ERROR");
              break;
          }
    };
}

void CMMC_SimplePair::add_listener(simple_pair_status_cb_t cb) {
  this->_user_sp_callback = cb;
}

void CMMC_SimplePair::start() {
    this->_simple_pair_init();
}
