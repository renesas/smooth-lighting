/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting_data_fpb_ra4e2.c
 * Description  : Contains pre-set configuration settings for fpb-ra4e2.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/* Pre-set configuration source file */
#if RS_SMOOTH_LED_LIGHTING_PRESET_CFG_USE // Use pre-set configuration for Smooth Lighting LED
#include <rs_smooth_led_lighting/config/rs_smooth_led_lighting_data_fpb_ra4e2.h>

const elc_cfg_t g_elc_cfg_preset =
{ .link[ELC_PERIPHERAL_GPT_A] = ELC_EVENT_GPT1_COUNTER_OVERFLOW, /* GPT1 COUNTER OVERFLOW (Overflow) */
};

elc_instance_ctrl_t g_elc_ctrl_preset;
const elc_instance_t g_elc_preset =
{ .p_ctrl = &g_elc_ctrl_preset, .p_api = &g_elc_on_elc, .p_cfg = &g_elc_cfg_preset };

dmac_instance_ctrl_t tx_cool_color_ctrl_preset;
transfer_info_t tx_cool_color_info_preset =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_OFFSET,
  .transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .length = 1, };
const dmac_extended_cfg_t tx_cool_color_extend_preset =
{ .offset = 4, .src_buffer_size = 1,
#if defined(VECTOR_NUMBER_DMAC1_INT)
  .irq                 = VECTOR_NUMBER_DMAC1_INT,
#endif
  .ipl = (2),
  .channel = 1, .p_callback = tx_callback, .activation_source = ELC_EVENT_GPT0_COUNTER_OVERFLOW, };
const transfer_cfg_t tx_cool_color_cfg_preset =
{ .p_info = &tx_cool_color_info_preset, .p_extend = &tx_cool_color_extend_preset, };
/* Instance structure to use this module. */
const transfer_instance_t tx_cool_color_preset =
{ .p_ctrl = &tx_cool_color_ctrl_preset, .p_cfg = &tx_cool_color_cfg_preset, .p_api = &g_transfer_on_dmac };

dmac_instance_ctrl_t tx_warm_color_ctrl_preset;
transfer_info_t tx_warm_color_info_preset =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_OFFSET,
  .transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .length = 1, };
const dmac_extended_cfg_t tx_warm_color_extend_preset =
{ .offset = 4, .src_buffer_size = 1,
  .channel = 0, .activation_source = ELC_EVENT_GPT0_COUNTER_OVERFLOW, };
const transfer_cfg_t tx_warm_color_cfg_preset =
{ .p_info = &tx_warm_color_info_preset, .p_extend = &tx_warm_color_extend_preset, };
/* Instance structure to use this module. */
const transfer_instance_t tx_warm_color_preset =
{ .p_ctrl = &tx_warm_color_ctrl_preset, .p_cfg = &tx_warm_color_cfg_preset, .p_api = &g_transfer_on_dmac };

gpt_instance_ctrl_t gpt_led_ctrl_preset;
const gpt_extended_cfg_t gpt_led_extend_preset =
        { .gtioca =
        { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtiocb =
          { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t gpt_led_cfg_preset =
{ .mode = TIMER_MODE_PWM,
/* Actual period: 0.00025 seconds. Actual duty: 0%. */.period_counts = (uint32_t) 0x61a8,
  .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t) 0, .channel = 1,
  .p_extend = &gpt_led_extend_preset,
        };
/* Instance structure to use this module. */
const timer_instance_t gpt_led_preset =
{ .p_ctrl = &gpt_led_ctrl_preset, .p_cfg = &gpt_led_cfg_preset, .p_api = &g_timer_on_gpt };

gpt_instance_ctrl_t gpt_fade_ctrl_preset;
const gpt_extended_cfg_t gpt_fade_extend_preset =
{
          .count_up_source = (gpt_source_t) (GPT_SOURCE_GPT_A | GPT_SOURCE_NONE),
        };
const timer_cfg_t gpt_fade_cfg_preset =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 3e-7 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x1e,
  .duty_cycle_counts = 0xf, .source_div = (timer_source_div_t) 0,.channel = 0,
  .p_extend = &gpt_fade_extend_preset,
        };
/* Instance structure to use this module. */
const timer_instance_t gpt_fade_preset =
{ .p_ctrl = &gpt_fade_ctrl_preset, .p_cfg = &gpt_fade_cfg_preset, .p_api = &g_timer_on_gpt };

#endif //Use pre-set configuration for Smooth LED Lighting
