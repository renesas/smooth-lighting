/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting_config.c
 * Description  : Contains configuration function definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/* Generic headers */
#if RS_SMOOTH_LED_LIGHTING_PRESET_CFG_USE// Use pre-set configuration for Smooth Lighting LED
#include <rs_smooth_led_lighting/config/rs_smooth_led_lighting_data_fpb_ra4e2.h>
#endif                                    // Use pre-set configuration for Smooth Lighting LED
#include "hal_data.h"
#include <rs_smooth_led_lighting/config/rs_smooth_led_lighting_data_std_config.h>
#include <rs_smooth_led_lighting/rs_smooth_led_lighting.h>
#include <stdlib.h>

/*******************************************************************************************************************//**
 * @addtogroup smooth_led_lighting
 * @{
 **********************************************************************************************************************/
/*****************************************************************************************************************
 * @brief       Generate the fading/periodic timer, and elc configurations; setting up pointers to dimming arrays.
 * @param[in]   Smooth LED Lighting Control instance, fading timer instance, elc timer instance, dimming data arrays.
 * @retval      FSP_SUCCESS     Upon successful generating of configurations.
 * @retval      FSP_ERR_INVALID_POINTER if any input pointer is invalid.
******************************************************************************************************************/
fsp_err_t rs_smooth_lighting_CommonConfigGen(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_instance_t * const p_gpt_fade, const elc_instance_t * p_elc,
                                             void * const p_dimming_warm_color, void * const p_dimming_cool_color)
{
    /* Check the pointers */
    if((NULL == p_bulb_ctrl) || (NULL == p_gpt_fade) || (NULL == p_elc) || (NULL == p_dimming_warm_color) || (NULL == p_dimming_cool_color))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Initialize the configurations */
    memset(&p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_cfg, 0, sizeof(timer_cfg_t));
    memset(&p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_ext_cfg, 0, sizeof(gpt_extended_cfg_t));
    memset(&p_bulb_ctrl->elc_link_ctrl.elc_cfg, 0, sizeof(elc_cfg_t));

    /* Copy the periodic timer settings into SRAM */
    memcpy(&p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_cfg, p_gpt_fade->p_cfg, sizeof(timer_cfg_t));
    memcpy(&p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_ext_cfg, p_gpt_fade->p_cfg->p_extend, sizeof(gpt_extended_cfg_t));
    memcpy(&p_bulb_ctrl->elc_link_ctrl.elc_cfg, p_elc->p_cfg, sizeof(elc_cfg_t));


    /* Manually assign addresses of the p_ pointers to configurations in SRAM, except driver APIs*/
    p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_cfg.p_extend = &p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_ext_cfg;
    p_bulb_ctrl->fade_timer_led_ctrl.p_gpt_dimming_fade_ctrl       = p_gpt_fade->p_ctrl; //Point to GPT timer control instance

    /* Point to APIs */
    p_bulb_ctrl->p_timer_api       = p_gpt_fade->p_api;
    p_bulb_ctrl->p_elc_api         = p_elc->p_api;

    /* Point to dimming data tables */
    p_bulb_ctrl->p_led_warm_color  = p_dimming_warm_color;
    p_bulb_ctrl->p_led_cool_color  = p_dimming_cool_color;

    return FSP_SUCCESS;
}

/*****************************************************************************************************************
 * @brief       Generate the configurations of the PWM timer and DMAC channels.
 * @param[in]   Smooth LED Lighting Control instance, PWM timer instance, transfer instance.
 * @retval      FSP_SUCCESS     Upon successful generating of configurations.
 * @retval      FSP_ERR_INVALID_POINTER if any input pointer is invalid.
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_ControlConfigGen(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_instance_t * const p_gpt_led, const transfer_instance_t * const p_tx_warm_color, const transfer_instance_t * const p_tx_cool_color)
{
    /* Check the pointers */
    if((NULL == p_bulb_ctrl ) || (NULL == p_gpt_led) || (NULL == p_tx_warm_color) || (NULL == p_tx_cool_color))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Initialize data structures */
    if(0 == p_bulb_ctrl->led_dimming_info.bulb_config_cnt)
    {

        /* Point to transfer APIs */
        p_bulb_ctrl->p_transfer_api = p_tx_warm_color->p_api;
    }
    /* Copy the PWM timer and DMAC Settings into SRAM */
    if(p_bulb_ctrl->led_dimming_info.bulb_config_cnt < p_bulb_ctrl->bulb_num)
    {
        /* Initialize the configurations */
        memset(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_cfg, 0, sizeof(timer_cfg_t));
        memset(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_ext_cfg, 0, sizeof(gpt_extended_cfg_t));

        memset(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg, 0, sizeof(transfer_cfg_t));
        memset(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg, 0, sizeof(transfer_cfg_t));

        memset(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg_ext_cfg, 0, sizeof(dmac_extended_cfg_t));
        memset(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg_ext_cfg, 0, sizeof(dmac_extended_cfg_t));
		
        /* Copy the Settings into SRAM */
        memcpy(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_cfg, p_gpt_led->p_cfg, sizeof(timer_cfg_t));
        memcpy(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_ext_cfg, p_gpt_led->p_cfg->p_extend, sizeof(gpt_extended_cfg_t));

        memcpy(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg, p_tx_warm_color->p_cfg, sizeof(transfer_cfg_t));
        memcpy(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg, p_tx_cool_color->p_cfg, sizeof(transfer_cfg_t));

        memcpy(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg_ext_cfg, p_tx_warm_color->p_cfg->p_extend, sizeof(dmac_extended_cfg_t));
        memcpy(&p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg_ext_cfg, p_tx_cool_color->p_cfg->p_extend, sizeof(dmac_extended_cfg_t));

        /* Initial duty cycle count for GPT timers */
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_cfg.duty_cycle_counts = 1U;

        /* Assign addresses of the p_ pointers to configurations in SRAM, except driver APIs*/
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_cfg.p_extend  = &p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].gpt_dimming_led_ext_cfg;
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].p_gpt_dimming_led_ctrl        = p_gpt_led->p_ctrl; //Point to GPT timer control instance

        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg.p_info    = p_tx_warm_color->p_cfg->p_info; //Pointer to Transfer info in SRAM
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg.p_extend  = &p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_warm_cfg_ext_cfg;
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].p_tx_dimming_warm_ctrl        = p_tx_warm_color->p_ctrl; // Point to DMAC control instance

        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg.p_info    = p_tx_cool_color->p_cfg->p_info; //Pointer to Transfer info in SRAM
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg.p_extend  = &p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].tx_dimming_cool_cfg_ext_cfg;
        p_bulb_ctrl->p_led_dimming_ctrl[p_bulb_ctrl->led_dimming_info.bulb_config_cnt].p_tx_dimming_cool_ctrl        = p_tx_cool_color->p_ctrl; // Point to DMAC control instance
			
        p_bulb_ctrl->led_dimming_info.bulb_config_cnt++;
    }

    return FSP_SUCCESS;
}

/*****************************************************************************************************************
 * @brief      Check and compare address of two pointers.
 * @param[in]  Pointers to API blocks.
 * @retval     FSP_SUCCESS              Upon successful comparing the address of the two pointers.
 * @retval     FSP_ERR_INVALID_POINTER  if any input pointer is invalid.
 * @retval     FSP_ERR_INVALID_MODE     if the addresses are mismatched.
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_GenericPointerCheck(void * const p_src_api, void * const p_dest_api)
{
    /* Check the pointers */
    if((NULL == p_dest_api) || (NULL == p_src_api))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Compare pointers */
    if((p_dest_api)!= (p_src_api))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    return FSP_SUCCESS;;
}

/**********************************************************************************************************************************************
 * @brief      Verify DMAC info settings and extended configurations. Check the present of a DMAC callback.
 * @param[in]  Smooth LED Lighting Control instance, standard transfer info and extended DMAC configurations.
 * @retval     FSP_SUCCESS              Upon successful comparing the Smooth LED Lighting Control instance configurations vs the standard configuration.
 * @retval     FSP_ERR_INVALID_POINTER  if any input pointer is invalid.
 * @retval     FSP_ERR_INVALID_MODE     if any of the configurations is mismatched.
 **********************************************************************************************************************************************/
fsp_err_t rs_smooth_lighting_TXInfoExtConfigCheck(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const transfer_info_t * p_std, const dmac_extended_cfg_t * p_ext_cfg)
{
    /* Check the pointers */
    if((NULL == p_bulb_ctrl) || (NULL == p_std) || (NULL == p_ext_cfg))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    int result   = 0;
    int callback = 0;
    for (int i =0; i< p_bulb_ctrl->bulb_num; i++)
    {
        /* Check p_dest info vs p_src info (the standard settings for DMAC in this software module)) */
        result = (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info->transfer_settings_word_b.mode           == p_std->transfer_settings_word_b.mode) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info->transfer_settings_word_b.dest_addr_mode == p_std->transfer_settings_word_b.dest_addr_mode) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info->transfer_settings_word_b.repeat_area    == p_std->transfer_settings_word_b.repeat_area) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info->transfer_settings_word_b.size           == p_std->transfer_settings_word_b.size) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info->transfer_settings_word_b.src_addr_mode  == p_std->transfer_settings_word_b.src_addr_mode)&&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info->length                                  == p_std->length) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info->transfer_settings_word_b.mode           == p_std->transfer_settings_word_b.mode) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info->transfer_settings_word_b.dest_addr_mode == p_std->transfer_settings_word_b.dest_addr_mode) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info->transfer_settings_word_b.repeat_area    == p_std->transfer_settings_word_b.repeat_area) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info->transfer_settings_word_b.size           == p_std->transfer_settings_word_b.size) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info->transfer_settings_word_b.src_addr_mode  == p_std->transfer_settings_word_b.src_addr_mode)&&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info->length                                  == p_std->length)  &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg_ext_cfg.offset                                  == p_ext_cfg->offset) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg_ext_cfg.offset                                  == p_ext_cfg->offset) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg_ext_cfg.activation_source                       != ELC_EVENT_NONE) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg_ext_cfg.activation_source                       != ELC_EVENT_NONE);

        /* Convert the result to fsp_err_t */
        if(!result)
        {
            return FSP_ERR_INVALID_MODE;
        }

        /* Check the present of callback */
        if(!callback)
        {
            callback = (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg_ext_cfg.p_callback == NULL) &&
                       (p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg_ext_cfg.p_callback != NULL);
         }
    }

    /* Return the callback error if any */
    if(!callback)
    {
        /* There is no callback */
        return FSP_ERR_INVALID_POINTER;
     }

    return FSP_SUCCESS;
 }

/*******************************************************************************************************************************************************
 * @brief      Verify Fading timer module configuration.
 * @param[in]  Smooth LED Lighting Control instance, standard fading timer.
 * @retval     FSP_SUCCESS              Upon successful comparing the Smooth LED Lighting Control instance configurations vs the standard configuration.
 * @retval     FSP_ERR_INVALID_POINTER  if any input pointer is invalid.
 * @retval     FSP_ERR_INVALID_MODE     if any of the configurations is mismatched.
 *******************************************************************************************************************************************************/
fsp_err_t rs_smooth_lighting_GPTFadingConfigCheck(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_cfg_t * p_std)
{
    /* Check the pointers */
    if((NULL == p_bulb_ctrl) || (NULL == p_std))
    {
        return FSP_ERR_INVALID_POINTER;
    }
    fsp_err_t result = FSP_SUCCESS;
    /* Check p_dest info vs p_src info (the standard settings for DMAC in this software module)) */
    result = (p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_cfg.mode                == p_std->mode) &&
             (p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_ext_cfg.count_up_source != 0); // !=  GPT_SOURCE_NONE = 0U

    /* Convert the result to fsp_err_t */
    if(!result)
    {
        return FSP_ERR_INVALID_MODE;
    }

    return FSP_SUCCESS;
 }

/*******************************************************************************************************************************************************
 * @brief      Verify GTP PWM timer module configurations.
 * @param[in]  Smooth LED Lighting Control instance, standard PWM timer configuration and extended standard PWM timer configuration.
 * @retval     FSP_SUCCESS              Upon successful comparing the Smooth LED Lighting Control instance configurations vs the standard configuration.
 * @retval     FSP_ERR_INVALID_POINTER  if any input pointer is invalid.
 * @retval     FSP_ERR_INVALID_MODE     if any of the configurations is mismatched.
 *******************************************************************************************************************************************************/
fsp_err_t rs_smooth_lighting_GPTLEDConfigCheck(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_cfg_t * p_std, const gpt_extended_cfg_t * p_ext_std)
{
    /* Check the pointers */
    if((NULL == p_bulb_ctrl) || (NULL == p_std) || (NULL == p_ext_std))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    fsp_err_t result = FSP_SUCCESS;
    for (int i = 0; i< p_bulb_ctrl->bulb_num; i++)
    {
        /* Check p_dest gpt cfg vs p_std and p_ext_std ext cfg (the standard settings for GPT PWM timer) */
        result = (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_cfg.mode                      == p_std->mode)&&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_ext_cfg.gtioca.output_enabled == p_ext_std->gtioca.output_enabled) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_ext_cfg.gtioca.stop_level     == p_ext_std->gtioca.stop_level) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_ext_cfg.gtioca_polarity       == p_ext_std->gtioca_polarity) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_ext_cfg.gtiocb.output_enabled == p_ext_std->gtiocb.output_enabled) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_ext_cfg.gtiocb.stop_level     == p_ext_std->gtiocb.stop_level) &&
                 (p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_ext_cfg.gtiocb_polarity       == p_ext_std->gtiocb_polarity);
        /* Convert the result to fsp_err_t */
        if(!result)
        {
            return FSP_ERR_INVALID_MODE;
        }
    }

    return FSP_SUCCESS;
 }

/*******************************************************************************************************************//**
 * @} (end addtogroup smooth_led_lighting)
 **********************************************************************************************************************/
