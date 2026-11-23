/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_sci_uart.h"
            #include "r_uart_api.h"
FSP_HEADER
/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_cool_color2;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_cool_color2_ctrl;
extern const transfer_cfg_t tx_cool_color2_cfg;

#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_warm_color2;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_warm_color2_ctrl;
extern const transfer_cfg_t tx_warm_color2_cfg;

#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t gpt_led2;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t gpt_led2_ctrl;
extern const timer_cfg_t gpt_led2_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_cool_color1;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_cool_color1_ctrl;
extern const transfer_cfg_t tx_cool_color1_cfg;

#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_warm_color1;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_warm_color1_ctrl;
extern const transfer_cfg_t tx_warm_color1_cfg;

#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t gpt_led1;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t gpt_led1_ctrl;
extern const timer_cfg_t gpt_led1_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart_jlob_vcom;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart_jlob_vcom_ctrl;
            extern const uart_cfg_t g_uart_jlob_vcom_cfg;
            extern const sci_uart_extended_cfg_t g_uart_jlob_vcom_cfg_extend;

            #ifndef uart_jlob_vcom_callback
            void uart_jlob_vcom_callback(uart_callback_args_t * p_args);
            #endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_cool_color;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_cool_color_ctrl;
extern const transfer_cfg_t tx_cool_color_cfg;

#ifndef tx_callback
void tx_callback(transfer_callback_args_t * p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t tx_warm_color;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t tx_warm_color_ctrl;
extern const transfer_cfg_t tx_warm_color_cfg;

#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t gpt_led;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t gpt_led_ctrl;
extern const timer_cfg_t gpt_led_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t gpt_fade;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t gpt_fade_ctrl;
extern const timer_cfg_t gpt_fade_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
