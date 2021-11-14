/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RRC_NR_UE_H
#define SRSRAN_RRC_NR_UE_H

#include "rrc_nr.h"

namespace srsenb {

class rrc_nr::ue
{
public:
  enum activity_timeout_type_t {
    MSG3_RX_TIMEOUT = 0,   ///< Msg3 has its own timeout to quickly remove fake UEs from random PRACHs
    UE_INACTIVITY_TIMEOUT, ///< (currently unused) UE inactivity timeout (usually bigger than reestablishment timeout)
    MSG5_RX_TIMEOUT,       ///< (currently unused) for receiving RRCConnectionSetupComplete/RRCReestablishmentComplete
    nulltype
  };

  /// @param [in] start_msg3_timer: indicates whether the UE is created as part of a RACH process
  ue(rrc_nr* parent_, uint16_t rnti_, const sched_nr_ue_cfg_t& uecfg, bool start_msg3_timer = true);
  ~ue();

  void send_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_dcch_msg);

  int  handle_sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params);
  void crnti_ce_received();

  // getters
  bool     is_connected() { return state == rrc_nr_state_t::RRC_CONNECTED; }
  bool     is_idle() { return state == rrc_nr_state_t::RRC_IDLE; }
  bool     is_inactive() { return state == rrc_nr_state_t::RRC_INACTIVE; }
  bool     is_endc() { return endc; }
  uint16_t get_eutra_rnti() { return eutra_rnti; }
  void     get_metrics(rrc_ue_metrics_t& ue_metrics) { ue_metrics = {}; /*TODO fill RRC metrics*/ };

  // setters

  void deactivate_bearers();

  /// methods to handle activity timer
  std::string to_string(const activity_timeout_type_t& type);
  void        set_activity_timeout(activity_timeout_type_t type);
  void        set_activity(bool enabled = true);
  void        activity_timer_expired(const activity_timeout_type_t type);

  /* TS 38.331 - 5.3.3 RRC connection establishment */
  void handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg);
  void handle_rrc_setup_complete(const asn1::rrc_nr::rrc_setup_complete_s& msg);

private:
  rrc_nr*  parent = nullptr;
  uint16_t rnti   = SRSRAN_INVALID_RNTI;

  /* TS 38.331 - 5.3.3 RRC connection establishment */
  void send_rrc_setup();
  void send_rrc_reject(uint8_t reject_wait_time_secs);

  int pack_rrc_reconfiguration(asn1::dyn_octstring& packed_rrc_reconfig);
  int pack_secondary_cell_group_cfg(asn1::dyn_octstring& packed_secondary_cell_config);

  int pack_secondary_cell_group_rlc_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_secondary_cell_group_mac_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_secondary_cell_group_sp_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded_init_dl_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_init_dl_bwp_pdsch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_init_dl_bwp_radio_link_monitoring(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded_ul_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pucch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pusch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded_pdcch_serving_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_recfg_with_sync(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common_phy_cell_group_cfg(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp_pdsch_cfg_common(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp_pusch_cfg_common(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_nr_radio_bearer_config(asn1::dyn_octstring& packed_nr_bearer_config);

  int add_drb();

  // logging helpers
  template <class M>
  void log_rrc_message(srsran::nr_srb          srb,
                       const direction_t       dir,
                       srsran::const_byte_span pdu,
                       const M&                msg,
                       const char*             msg_type);
  template <class M>
  void log_rrc_container(const direction_t dir, srsran::const_byte_span pdu, const M& msg, const char* msg_type);

  // state
  rrc_nr_state_t       state          = rrc_nr_state_t::RRC_IDLE;
  uint8_t              transaction_id = 0;
  srsran::unique_timer activity_timer; /// for basic DL/UL activity timeout

  // RRC configs for UEs
  asn1::rrc_nr::cell_group_cfg_s   cell_group_cfg;
  asn1::rrc_nr::radio_bearer_cfg_s radio_bearer_cfg;

  // MAC controller
  sched_nr_interface::ue_cfg_t uecfg{};

  const uint32_t drb1_lcid = 4;

  // NSA specific variables
  bool     endc       = false;
  uint16_t eutra_rnti = SRSRAN_INVALID_RNTI;
};

} // namespace srsenb

#endif // SRSRAN_RRC_NR_UE_H