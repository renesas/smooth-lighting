/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting_data_fpb_ra4e2.h
 * Description  : Contains pre-set configuration declarations for fpb-ra4e2.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#if RS_SMOOTH_LED_LIGHTING_PRESET_CFG_USE // Use pre-set configuration for Smooth Lighting LED
#ifndef RS_CONFIG_FPB_RA4E2_H_
#define RS_CONFIG_FPB_RA4E2_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_transfer_api.h"
#include "r_dmac.h"
#include "r_timer_api.h"
#include "r_gpt_cfg.h"
#include "r_gpt.h"
#include "r_elc_api.h"
#include "r_elc.h"

FSP_HEADER

/** ELC Instance */
extern const elc_instance_t g_elc_preset;

/** Access the ELC instance using these structures when calling API functions directly (::p_api is not used). */
extern elc_instance_ctrl_t g_elc_ctrl_preset;
extern const elc_cfg_t g_elc_cfg_preset;

/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_cool_color_preset;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_cool_color_ctrl_preset;
extern const transfer_cfg_t tx_cool_color_cfg_preset;

#ifndef tx_callback
void tx_callback(transfer_callback_args_t *p_args);
#endif

/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_warm_color_preset;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_warm_color_ctrl_preset;
extern const transfer_cfg_t tx_warm_color_cfg_preset;

/** Timer on GPT Instance. */
extern const timer_instance_t gpt_led_preset;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t gpt_led_ctrl_preset;
extern const timer_cfg_t gpt_led_cfg_preset;

/** Timer on GPT Instance. */
extern const timer_instance_t gpt_fade_preset;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t gpt_fade_ctrl_preset;
extern const timer_cfg_t gpt_fade_cfg_preset;

FSP_FOOTER
#endif /* RS_CONFIG_FPB_RA4E2_H_ */
#endif //Use pre-set configuration for Smooth Lighting LED
