/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting_data_std_config.h
 * Description  : Contains standard configuration declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef RS_SMOOTH_LED_LIGHTING_STD_CONFIG_H_
#define RS_SMOOTH_LED_LIGHTING_STD_CONFIG_H_
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
/*
 * Data for configuration checking START
 * */
extern const transfer_info_t tx_info_std;
const extern dmac_extended_cfg_t tx_ext_cfg_std;
extern const timer_cfg_t gpt_fade_cfg_std;
extern const timer_cfg_t gpt_led_cfg_std;
extern const gpt_extended_cfg_t gpt_fade_ext_cfg_std;
extern const gpt_extended_cfg_t gpt_led_ext_cfg_std;
/*
 * Data for configuration checking END
 * */
FSP_FOOTER
#endif /* RS_CONFIG_STD_H_ */

