/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting_data_std_config.c
 * Description  : Contains standard configuration settings.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/* Pre-set configuration source file */
#if RS_SMOOTH_LED_LIGHTING_PRESET_CFG_USE// Use pre-set configuration for Smooth Lighting LED
#include <rs_smooth_led_lighting/config/rs_smooth_led_lighting_data_fpb_ra4e2.h>
#endif
#include <rs_smooth_led_lighting/config/rs_smooth_led_lighting_data_std_config.h>
/*
 * Standard Settings to verify configurations - START
 * */
const transfer_info_t tx_info_std =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_OFFSET,
  .transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .length = 1, };

const dmac_extended_cfg_t tx_ext_cfg_std =
{ .offset = 4,
  .activation_source = ELC_EVENT_GPT0_COUNTER_OVERFLOW, };

const timer_cfg_t gpt_fade_cfg_std =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 3e-7 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x1e,
        };

const gpt_extended_cfg_t gpt_fade_ext_cfg_std =
{
          .count_up_source = (gpt_source_t) (GPT_SOURCE_GPT_A | GPT_SOURCE_NONE),
        };


const timer_cfg_t gpt_led_cfg_std =
{ .mode = TIMER_MODE_PWM,
/* Actual period: 0.00025 seconds. Actual duty: 0%. */.period_counts = (uint32_t) 0x61a8,
        };

const gpt_extended_cfg_t gpt_led_ext_cfg_std =
        { .gtioca =
        { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtiocb =
          { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

/*
 * Standard Settings to verify configurations - END
 * */
