/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting.c
 * Description  : Contains function definition.
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
#include <rs_smooth_led_lighting/rs_smooth_led_lighting.h>
#include <stdlib.h>

/*******************************************************************************************************************//**
 * @addtogroup smooth_led_lighting
 * @{
 **********************************************************************************************************************/
/* Smooth LED lighting functions block*/
const smooth_led_dimming_function_t g_dimming_ctrl_on_led_bulb =
{
    .Open                  = rs_smooth_lighting_Open,
    .Close                 = rs_smooth_lighting_Close,
    .IntensitySet          = rs_smooth_lighting_IntensitySet,
    .IntensityLevelSet     = rs_smooth_lighting_IntensityLevelSet,
    .CommonConfigGen       = rs_smooth_lighting_CommonConfigGen,
    .ControlConfigGen      = rs_smooth_lighting_ControlConfigGen,
    .ConfigSizeGet         = rs_smooth_lighting_ConfigSizeGet,
    .GenericPointerCheck   = rs_smooth_lighting_GenericPointerCheck,
    .TXInfoExtConfigCheck  = rs_smooth_lighting_TXInfoExtConfigCheck,
    .GPTFadingConfigCheck  = rs_smooth_lighting_GPTFadingConfigCheck,
    .GPTLEDConfigCheck     = rs_smooth_lighting_GPTLEDConfigCheck,
};

/*****************************************************************************************************************
 * @brief       Open hardware modules and set the dimming level. Call this function to set dimming level after generating module configurations.
 * @param[in]   Smooth LED Lighting Control instance, LED dimming target.
 * @retval      FSP_SUCCESS     Upon successful start of modules and set LED dimming level.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful start .
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_IntensitySet(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const uint16_t * p_dimming_target)
{
    /* Check the pointer */
    if((NULL == p_bulb_ctrl) || (NULL == p_dimming_target))
    {
       return FSP_ERR_INVALID_POINTER;
    }

    fsp_err_t err = FSP_SUCCESS;

    if(false == p_bulb_ctrl->led_dimming_info.open)
    {
        /* Open FSP modules needed for smooth LED lighting control */
        err = rs_smooth_lighting_Open(p_bulb_ctrl);
        if(FSP_SUCCESS != err)
        {
            /* Close FSP modules and exit */
            rs_smooth_lighting_Close(p_bulb_ctrl);
            return err; //Return FSP error
        }
    }
    /* Set dimming level */
    err = rs_smooth_lighting_IntensityLevelSet(p_bulb_ctrl, p_dimming_target);
    if(err != FSP_SUCCESS)
    {
        /* Return the Open err */
        err = FSP_ERR_NOT_INITIALIZED;
    }

    return err;
}

/*****************************************************************************************************************
 * @brief       Open the FSP modules needed for Smooth LED dimming module.
 * @param[in]   Smooth LED Lighting Control instance.
 * @retval      FSP_SUCCESS     Upon successful open of timers/data transfer modules.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful open.
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_Open(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl)
{
    /* Check the pointer */
    if(NULL == p_bulb_ctrl)
    {
       return FSP_ERR_INVALID_POINTER;
    }
    /* Function variables */
    fsp_err_t             err    = FSP_SUCCESS;
    transfer_info_t     * p_cool = NULL;
    transfer_info_t     * p_warm = NULL;
    gpt_instance_ctrl_t * p_instance_ctrl = NULL;

    if(false == p_bulb_ctrl->led_dimming_info.open)
    {
        /* Initialize ELC Controller */
        err = p_bulb_ctrl->p_elc_api->open(p_bulb_ctrl->elc_link_ctrl.p_elc_ctrl, &p_bulb_ctrl->elc_link_ctrl.elc_cfg);
        if(err != FSP_SUCCESS)
        {
            return err;
        }

        err = p_bulb_ctrl->p_elc_api->enable(&p_bulb_ctrl->elc_link_ctrl.p_elc_ctrl);
        if(err != FSP_SUCCESS)
        {
            return err;
        }

        /* Initialize Periodic Timer */
        err = p_bulb_ctrl->p_timer_api->open(p_bulb_ctrl->fade_timer_led_ctrl.p_gpt_dimming_fade_ctrl, &p_bulb_ctrl->fade_timer_led_ctrl.gpt_dimming_fade_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        for(int i = 0; i< (int)p_bulb_ctrl->bulb_num; i++)
        {
            /* Initialize PWM Timer */
            err = p_bulb_ctrl->p_timer_api->open(p_bulb_ctrl->p_led_dimming_ctrl[i].p_gpt_dimming_led_ctrl,&p_bulb_ctrl->p_led_dimming_ctrl[i].gpt_dimming_led_cfg);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Set source addresses of data transfers */
            p_warm = (transfer_info_t *)p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info;
            p_cool = (transfer_info_t *)p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info;

            p_warm->p_src = (void const * volatile) p_bulb_ctrl->p_led_warm_color;
            p_cool->p_src = (void const * volatile)p_bulb_ctrl->p_led_cool_color;

            /* Set target addresses of data transfers */
            p_instance_ctrl = (gpt_instance_ctrl_t *)p_bulb_ctrl->p_led_dimming_ctrl[i].p_gpt_dimming_led_ctrl;
            p_warm->p_dest = (void *volatile) &(p_instance_ctrl->p_reg->GTCCR[RS_SMOOTH_LED_LIGHTING_GTCCRC_POS]);
            p_cool->p_dest = (void *volatile) &(p_instance_ctrl->p_reg->GTCCR[RS_SMOOTH_LED_LIGHTING_GTCCRD_POS]);

            /* Initialize DMAC Transfer */
            err = p_bulb_ctrl->p_transfer_api->open(p_bulb_ctrl->p_led_dimming_ctrl[i].p_tx_dimming_warm_ctrl, &p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg);
            if(err != FSP_SUCCESS)
            {

                return err;
            }

            /* Initialize DMAC Transfer */
            err = p_bulb_ctrl->p_transfer_api->open(p_bulb_ctrl->p_led_dimming_ctrl[i].p_tx_dimming_cool_ctrl, &p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg);
            if(err != FSP_SUCCESS)
            {
                return err;
            }
            /* Start PWM timer */
            err = p_bulb_ctrl->p_timer_api->start(p_instance_ctrl);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }

        /* Reset LED dimming stage and target */
        p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage  = RESET_VALUE;
        p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_target = RESET_VALUE;

        p_bulb_ctrl->led_dimming_info.open = true;
    }

    return err;
}
/*****************************************************************************************************************
 * @brief       Set the LED dimming level.
 * @param[in]   Smooth LED Lighting Control instance, LED dimming target.
 * @retval      FSP_SUCCESS     Upon successful setting of LED dimming level.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful start .
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_IntensityLevelSet(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const uint16_t * p_dimming_target)
{
    /* Check the pointer */
    if((NULL == p_bulb_ctrl) || (NULL == p_dimming_target))
    {
       return FSP_ERR_INVALID_POINTER;
    }
    /* Variables initialization */
    fsp_err_t             err     = FSP_SUCCESS;
    transfer_properties_t tx_info = {RESET_VALUE};

    if(true == p_bulb_ctrl->led_dimming_info.open)
    {
        /* Stop the timer triggering duty cycle updates, if any are still in progress */
        err = p_bulb_ctrl->p_timer_api->stop(p_bulb_ctrl->fade_timer_led_ctrl.p_gpt_dimming_fade_ctrl);
        if (FSP_SUCCESS != err)
           {
                return err;
           }

        /* Correct current stage, based on old target and transfers remaining */
        err = p_bulb_ctrl->p_transfer_api->infoGet(p_bulb_ctrl->p_led_dimming_ctrl[0].p_tx_dimming_cool_ctrl, &tx_info);
        if (FSP_SUCCESS != err)
           {
                return err;
           }

        if (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage < p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_target) // Stage was going up
        {
            p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage = p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_target - (uint16_t) tx_info.transfer_length_remaining;
        }
        else if (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage > p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_target)// Stage was going down
        {
            p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage = p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_target + (uint16_t) tx_info.transfer_length_remaining;
        }

        /* Set the new target */
        p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_target = (*p_dimming_target);

        /* Skip data transfer, the LED stage was at the target */
        if ((*p_dimming_target) == p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage)
        {
             return FSP_SUCCESS;
        }

        transfer_info_t * p_cool = NULL;
        transfer_info_t * p_warm = NULL;
        for(int i = 0; i< p_bulb_ctrl->bulb_num; i++)
        {
            p_cool = (transfer_info_t *)p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_cool_cfg.p_info;
            p_warm = (transfer_info_t *)p_bulb_ctrl->p_led_dimming_ctrl[i].tx_dimming_warm_cfg.p_info;

            if (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage < (*p_dimming_target)) // Stage will be going up
            {
                /* Calculate and set the number of steps to the new target */
                p_cool->length = (*p_dimming_target) - p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage;
                p_warm->length = (*p_dimming_target) - p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage;
                /* Set the transfer addressing mode */
                p_cool->transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED;
                p_warm->transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED;
                /* Set the first address to start the DMA with */
                p_warm->p_src = (const uint32_t *)(p_bulb_ctrl->p_led_warm_color) + (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage + 1);
                p_cool->p_src = (const uint32_t *)(p_bulb_ctrl->p_led_cool_color) + (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage + 1);
            }
            else // Stage will be going down
            {
                /* Calculate and set the number of steps to the new target */
                p_cool->length = p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage - (*p_dimming_target);
                p_warm->length = p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage - (*p_dimming_target);
                /* Set the transfer addressing mode */
                p_cool->transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_DECREMENTED;
                p_warm->transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_DECREMENTED;
                /* Set the first address to start the DMAC with */
                p_warm->p_src = (const uint32_t *)(p_bulb_ctrl->p_led_warm_color) + (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage - 1);
                p_cool->p_src = (const uint32_t *)(p_bulb_ctrl->p_led_cool_color) + (p_bulb_ctrl->led_dimming_info.led_dimming_stage.led_stage - 1);
            }

            err = p_bulb_ctrl->p_transfer_api->reconfigure(p_bulb_ctrl->p_led_dimming_ctrl[i].p_tx_dimming_cool_ctrl, p_cool);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            err = p_bulb_ctrl->p_transfer_api->reconfigure(p_bulb_ctrl->p_led_dimming_ctrl[i].p_tx_dimming_warm_ctrl, p_warm);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }

        /* Start periodic timer */
        err = p_bulb_ctrl->p_timer_api->start(p_bulb_ctrl->fade_timer_led_ctrl.p_gpt_dimming_fade_ctrl);
        if (FSP_SUCCESS != err)
        {
              return err;
        }
    }
    else
    {
        return FSP_ERR_NOT_OPEN;
    }

    return err;
}

/*****************************************************************************************************************
 * @brief      Close the FSP modules needed for Smooth LED lighting Control.
 * @param[in]  Smooth LED Lighting Control, LED dimming target.
 * @retval     fsp_err_t
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_Close(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl)
{
    /* Check the pointer */
    if(NULL == p_bulb_ctrl)
    {
       return FSP_ERR_INVALID_POINTER;
    }

    fsp_err_t err = FSP_SUCCESS;
    if(true == p_bulb_ctrl->led_dimming_info.open)
    {
        /* Close Periodic Timer */
        err = p_bulb_ctrl->p_timer_api->close(p_bulb_ctrl->fade_timer_led_ctrl.p_gpt_dimming_fade_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        /* Close ELC */
        err = p_bulb_ctrl->p_elc_api->close(p_bulb_ctrl->elc_link_ctrl.p_elc_ctrl);
        if(err != FSP_SUCCESS)
        {
            return err;
        }

        for(int i = 0; i< p_bulb_ctrl->bulb_num; i++)
        {
            /* Close PWM Timer */
            err = p_bulb_ctrl->p_timer_api->close(p_bulb_ctrl->p_led_dimming_ctrl[i].p_gpt_dimming_led_ctrl);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            /* Close data transfers */
            err = p_bulb_ctrl->p_transfer_api->close(p_bulb_ctrl->p_led_dimming_ctrl[i].p_tx_dimming_cool_ctrl);
            if(err != FSP_SUCCESS)
            {

                return err;
            }

            err = p_bulb_ctrl->p_transfer_api->close(p_bulb_ctrl->p_led_dimming_ctrl[i].p_tx_dimming_warm_ctrl);
            if(err != FSP_SUCCESS)
            {

                return err;
            }
        }
        /* Set the module state to _CLOSED */
        p_bulb_ctrl->led_dimming_info.open = false;
    }
    else
    {
        return FSP_ERR_NOT_OPEN;
    }

    return err;
}

/*****************************************************************************************************************
 * @brief      Get the Configuration Size.
 * @param[in]  Smooth LED Lighting Control instance, pointer to store size of module configurations
 * @retval     fsp_err_t
 ****************************************************************************************************************/
fsp_err_t rs_smooth_lighting_ConfigSizeGet(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, size_t * const size)
{
    /* Check the pointers */
    if((NULL == p_bulb_ctrl) || (NULL == size))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Return configuration size in * size */
    *size = sizeof(* p_bulb_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup smooth_led_lighting)
 **********************************************************************************************************************/
