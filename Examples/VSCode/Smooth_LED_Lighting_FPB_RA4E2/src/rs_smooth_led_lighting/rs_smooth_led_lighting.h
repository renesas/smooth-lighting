/***********************************************************************************************************************
 * File Name    : rs_smooth_led_lighting.h
 * Description  : Contains Macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef RS_SMOOTH_LED_LIGHTING_H_
#define RS_SMOOTH_LED_LIGHTING_H_

/* Macros definitions */
#define RESET_VALUE                             0
#define RS_SMOOTH_LED_LIGHTING_GTCCRC_POS       (2U)
#define RS_SMOOTH_LED_LIGHTING_GTCCRD_POS       (3U)

#define RS_SMOOTH_LED_LIGHTING_TX_OFFSET        (4U)

/* Version: MAJOR.MINOR.PATCH */
#define RS_SMOOTH_LED_LIGHTING_VERSION_MAJOR    (1U)
#define RS_SMOOTH_LED_LIGHTING_VERSION_MINOR    (0U)
#define RS_SMOOTH_LED_LIGHTING_VERSION_PATCH    (0U)
#define RS_SMOOTH_LED_LIGHTING_VERSION_STRING   "1.0.0"

/* LED dimming stage info */
typedef struct st_smooth_led_dimming_stage
{
    volatile uint16_t led_stage;
    volatile uint16_t led_target;
}smooth_led_dimming_stage_t;

/* Dimming levels */
typedef enum st_smooth_led_dimming_level
{
    RS_SMOOTH_LED_LIGHTING_OFF         = 0,
    RS_SMOOTH_LED_LIGHTING_LOW         = 1,
    RS_SMOOTH_LED_LIGHTING_MIDDLE      = 2,
    RS_SMOOTH_LED_LIGHTING_HIGH        = 3,
    RS_SMOOTH_LED_LIGHTING_FULLON      = 4,
}smooth_led_dimming_level_t;

#if 0
/* LED dimming  module status */
typedef enum st_smooth_led_dimming_status
{
    RS_SMOOTH_LED_LIGHTING_CLOSED      = 0,
    RS_SMOOTH_LED_LIGHTING_OPEN        = 1,
}smooth_led_dimming_status_t;
#endif
/* Configurations/Controls */
typedef struct st_smooth_led_dimming_ctrl
{
    /* PWM timer */
    gpt_instance_ctrl_t * p_gpt_dimming_led_ctrl;       /* PWM timer control instance */
    timer_cfg_t           gpt_dimming_led_cfg;          /* PWM timer configuration */
    gpt_extended_cfg_t    gpt_dimming_led_ext_cfg;      /* PWM timer extended configuration */

    /* DMAC channel for warm color led */
    dmac_instance_ctrl_t * p_tx_dimming_warm_ctrl;       /* Data transfer control instance */
    transfer_cfg_t         tx_dimming_warm_cfg;          /* Data transfer configuration */
    dmac_extended_cfg_t    tx_dimming_warm_cfg_ext_cfg;  /* Data transfer extended configuration */
	
    /* DMAC channel for cool color led */
    dmac_instance_ctrl_t * p_tx_dimming_cool_ctrl;       /* Data transfer control instance */
    transfer_cfg_t         tx_dimming_cool_cfg;          /* Data transfer configuration */
    dmac_extended_cfg_t    tx_dimming_cool_cfg_ext_cfg;  /* Data transfer extended configuration */
}smooth_led_dimming_ctrl_t;

/* Periodic timer */
typedef struct st_fade_timer_led_ctrl
{
    gpt_instance_ctrl_t * p_gpt_dimming_fade_ctrl;        /* Periodic timer control instance */
    timer_cfg_t           gpt_dimming_fade_cfg;           /* Periodic timer configuration */
    gpt_extended_cfg_t    gpt_dimming_fade_ext_cfg;       /* Periodic timer extended configuration */
}fade_timer_led_ctrl_t;

/* Event link */
typedef struct st_elc_link
{
    elc_instance_ctrl_t * p_elc_ctrl;                      /* ELC control instance */
    elc_cfg_t             elc_cfg;                         /* ELC configuration */
}elc_link_ctrl_t;

/* LED dimming module info */
typedef struct st_led_bulb_dimming_info
{
    bool                        open;                     /* Open/Close state */
    smooth_led_dimming_stage_t  led_dimming_stage;        /* LED dimming stage */
    int                         bulb_config_cnt;          /* The bulb configuration count */
}led_bulb_dimming_info_t;

struct smooth_led_dimming_function_t;
/* LED bulb dimming control instance */
typedef struct st_led_bulb_diming_ctrl_instance
{
    smooth_led_dimming_ctrl_t      * p_led_dimming_ctrl;     /* PWM timer, DMAC controls */
    fade_timer_led_ctrl_t            fade_timer_led_ctrl;    /* Periodic-delay timer */
    elc_link_ctrl_t                  elc_link_ctrl;          /* Event link control */
    led_bulb_dimming_info_t          led_dimming_info;       /* LED dimming control info */
    const timer_api_t              * p_timer_api;            /* Timer API pointer */
    const elc_api_t                * p_elc_api;              /* ELC API pointer */
    const transfer_api_t           * p_transfer_api;         /* Transfer API pointer */
    const struct smooth_led_dimming_function_t  *p_led_dimming_function;  /* LED dimming functions pointers */
    const uint32_t                 * p_led_warm_color;       /* Pointer to warm color array */
    const uint32_t                 * p_led_cool_color;       /* Pointer to cool color array */
    int                              bulb_num;               /* Number of LED bulbs */
}led_bulb_dimming_ctrl_instance_t;

/* Module Public functions */
typedef struct smooth_led_dimming_function_t
{
    fsp_err_t (* Open)(led_bulb_dimming_ctrl_instance_t * p_bulb_ctrl);
    fsp_err_t (* Close)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl);
    fsp_err_t (* IntensitySet)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const uint16_t * p_dimming_target);
    fsp_err_t (* IntensityLevelSet)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const uint16_t* p_dimming_target);
    fsp_err_t (* CommonConfigGen)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_instance_t * p_smooth_gpt_fade, const elc_instance_t * p_elc, void * const p_dimming_warm_color, void * const p_dimming_cool_color);
    fsp_err_t (* ControlConfigGen)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_instance_t * const p_smooth_gpt_led,const transfer_instance_t * const p_smooth_tx_warm, const transfer_instance_t * const p_smooth_tx_cool);
    fsp_err_t (* ConfigSizeGet)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, size_t * const size);
    fsp_err_t (* GenericPointerCheck)(void * const p_src_api, void * const p_dest_api);
    fsp_err_t (* TXInfoExtConfigCheck)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const transfer_info_t * p_std, const dmac_extended_cfg_t * p_ext_cfg);
    fsp_err_t (* GPTFadingConfigCheck)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_cfg_t * p_std);
    fsp_err_t (* GPTLEDConfigCheck)(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_cfg_t * p_std, const gpt_extended_cfg_t * p_ext_std);
} smooth_led_dimming_function_t;

/* Exported functions block */
extern const smooth_led_dimming_function_t g_dimming_ctrl_on_led_bulb;

/* Function Declarations/Public functions */
fsp_err_t rs_smooth_lighting_Open(led_bulb_dimming_ctrl_instance_t * p_bulb_ctrl);
fsp_err_t rs_smooth_lighting_Close(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl);
fsp_err_t rs_smooth_lighting_IntensitySet(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const uint16_t * p_dimming_target);
fsp_err_t rs_smooth_lighting_IntensityLevelSet(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const uint16_t * p_dimming_target);
fsp_err_t rs_smooth_lighting_CommonConfigGen(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_instance_t * p_gpt_fade, const elc_instance_t * p_elc, void * const p_dimming_warm_color, void * const p_dimming_cool_color);
fsp_err_t rs_smooth_lighting_ControlConfigGen(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_instance_t * const p_gpt_led, const transfer_instance_t * const p_tx_warm_color, const transfer_instance_t * const p_tx_cool_color);
fsp_err_t rs_smooth_lighting_ConfigSizeGet(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, size_t * const size);
fsp_err_t rs_smooth_lighting_GenericPointerCheck(void * const p_src_api, void * const p_dest_api);
fsp_err_t rs_smooth_lighting_TXInfoExtConfigCheck(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const transfer_info_t * p_std, const dmac_extended_cfg_t * p_ext_cfg);
fsp_err_t rs_smooth_lighting_GPTFadingConfigCheck(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_cfg_t * p_std);
fsp_err_t rs_smooth_lighting_GPTLEDConfigCheck(led_bulb_dimming_ctrl_instance_t * const p_bulb_ctrl, const timer_cfg_t * p_std, const gpt_extended_cfg_t * p_ext_std);

#endif /* RS_SMOOTH_LED_LIGHTING_H_ */
