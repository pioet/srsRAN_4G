/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/asn1/s1ap_utils.h"
#include "srslte/interfaces/rrc_interface_types.h"

#ifndef SRSLTE_ENB_S1AP_INTERFACES_H
#define SRSLTE_ENB_S1AP_INTERFACES_H

namespace srsenb {

struct s1ap_args_t {
  uint32_t    enb_id;  // 20-bit id (lsb bits)
  uint8_t     cell_id; // 8-bit cell id
  uint16_t    tac;     // 16-bit tac
  uint16_t    mcc;     // BCD-coded with 0xF filler
  uint16_t    mnc;     // BCD-coded with 0xF filler
  std::string mme_addr;
  std::string gtp_bind_addr;
  std::string s1c_bind_addr;
  std::string enb_name;
};

// S1AP interface for RRC
class s1ap_interface_rrc
{
public:
  struct bearer_status_info {
    uint8_t  erab_id;
    uint16_t pdcp_dl_sn, pdcp_ul_sn;
    uint16_t dl_hfn, ul_hfn;
  };

  virtual void initial_ue(uint16_t                              rnti,
                          uint32_t                              enb_cc_idx,
                          asn1::s1ap::rrc_establishment_cause_e cause,
                          srslte::unique_byte_buffer_t          pdu) = 0;
  virtual void initial_ue(uint16_t                              rnti,
                          uint32_t                              enb_cc_idx,
                          asn1::s1ap::rrc_establishment_cause_e cause,
                          srslte::unique_byte_buffer_t          pdu,
                          uint32_t                              m_tmsi,
                          uint8_t                               mmec)                     = 0;

  virtual void write_pdu(uint16_t rnti, srslte::unique_byte_buffer_t pdu)                              = 0;
  virtual bool user_exists(uint16_t rnti)                                                              = 0;
  virtual void user_mod(uint16_t old_rnti, uint16_t new_rnti)                                          = 0;
  virtual bool user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio)              = 0;
  virtual void ue_ctxt_setup_complete(uint16_t rnti, const asn1::s1ap::init_context_setup_resp_s& res) = 0;
  virtual void ue_erab_setup_complete(uint16_t rnti, const asn1::s1ap::erab_setup_resp_s& res)         = 0;
  virtual bool is_mme_connected()                                                                      = 0;

  /**
   * Command the s1ap to transmit a HandoverRequired message to MME.
   * This message initiates the S1 handover preparation procedure at the Source eNB
   *
   * @param rnti user to perform S1 handover
   * @param target_eci eNB Id + Cell Id of the target eNB
   * @param target_plmn PLMN of the target eNB
   * @param fwd_erabs E-RABs that are candidates to DL forwarding
   * @param rrc_container RRC container with SourceENBToTargetENBTransparentContainer message.
   * @return true if successful
   */
  virtual bool send_ho_required(uint16_t                     rnti,
                                uint32_t                     target_eci,
                                srslte::plmn_id_t            target_plmn,
                                srslte::span<uint32_t>       fwd_erabs,
                                srslte::unique_byte_buffer_t rrc_container) = 0;

  /**
   * Command the s1ap to transmit eNBStatusTransfer message to MME. This message passes the PDCP context of the UE
   * performing S1 handover from source eNB to target eNB.
   *
   * @param rnti user to perform S1 handover
   * @param bearer_status_list PDCP SN and HFN status of the bearers to be preserved at target eNB
   * @return true if successful
   */
  virtual bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) = 0;

  /* Acknowledge Handover Request message back to MME.
   * This message signals the completion of the HandoverPreparation from the TeNB point of view. */
  virtual bool send_ho_req_ack(const asn1::s1ap::ho_request_s&                msg,
                               uint16_t                                       rnti,
                               uint32_t                                       enb_cc_idx,
                               srslte::unique_byte_buffer_t                   ho_cmd,
                               srslte::span<asn1::s1ap::erab_admitted_item_s> admitted_bearers) = 0;

  /**
   * Notify MME that Handover is complete
   */
  virtual void send_ho_notify(uint16_t rnti, uint64_t target_eci) = 0;

  /**
   * Cancel on-going S1 Handover. MME should release UE context in target eNB
   * SeNB --> MME
   */
  virtual void send_ho_cancel(uint16_t rnti) = 0;

  /**
   * Called during release of a subset of eNB E-RABs. Send E-RAB RELEASE INDICATION to MME.
   * SeNB --> MME
   */
  virtual bool release_erabs(uint16_t rnti, const std::vector<uint16_t>& erabs_successfully_released) = 0;

  virtual bool send_ue_cap_info_indication(uint16_t rnti, srslte::unique_byte_buffer_t ue_radio_cap) = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_S1AP_INTERFACES_H
