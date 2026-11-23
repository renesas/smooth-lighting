#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#if !defined (__PICOLIBC__)
#error "Use this file only with picolibc. Typically packaged with LLVM ARM Embedded Toolchain."
#endif /* defined (__PICOLIBC__) */

#include "r_uart_api.h"
#include "r_byteq.h"

static uint8_t rx_memory_buffer[256];
static uint8_t tx_memory_buffer[256];

byteq_cfg_t uart_rx_q_cfg =
{
 .p_buffer = &rx_memory_buffer[0],
 .buffer_size = sizeof(rx_memory_buffer),
 .p_callback = NULL,
 .p_context = NULL,
 .event_mask = BYTEQ_EVENT_NONE,
};

byteq_cfg_t uart_tx_q_cfg =
{
 .p_buffer = &tx_memory_buffer[0],
 .buffer_size = sizeof(tx_memory_buffer),
 .p_callback = NULL,
 .p_context = NULL,
 .event_mask = BYTEQ_EVENT_NONE,
};

byteq_state_ctrl_t uart_rx_q_ctrl;
byteq_state_ctrl_t uart_tx_q_ctrl;

static char * p_tx_memory = NULL;

static volatile bool tx_busy = false;
static uart_instance_t * p_uart_jlob_vcom = NULL;
static uart_callback_args_t uart_callback_args;

static int uart_putc(char c, FILE *f);
static int uart_getc(FILE *f);
static int uart_flush(FILE *f);

#if defined(__PICOLIBC__)
static FILE g_stdin_file  = FDEV_SETUP_STREAM(NULL,
                                              uart_getc,
                                              NULL,
                                              _FDEV_SETUP_READ);

static FILE g_stdout_file = FDEV_SETUP_STREAM(uart_putc,
                                              NULL,
                                              uart_flush,
                                              _FDEV_SETUP_WRITE);

FILE *const stdin  = &g_stdin_file;
FILE *const stdout = &g_stdout_file;
FILE *const stderr = &g_stdout_file;
#endif 

static fsp_err_t jlob_uart_init_once(void);

static bool g_stdio_uart_initialized = false;
#if defined(UNIT_TEST_UART)
void picolibc_stdio_to_sci_uart_test_reset(void)
{
    g_stdio_uart_initialized = false;

    tx_busy = false;

    if (NULL != p_tx_memory)
    {
        free(p_tx_memory);
        p_tx_memory = NULL;
    }

    memset(rx_memory_buffer, 0, sizeof(rx_memory_buffer));
    memset(tx_memory_buffer, 0, sizeof(tx_memory_buffer));

    memset(&uart_rx_q_ctrl, 0, sizeof(uart_rx_q_ctrl));
    memset(&uart_tx_q_ctrl, 0, sizeof(uart_tx_q_ctrl));
}
#endif

void uart_jlob_vcom_callback(uart_callback_args_t *p_args)
{
    uart_event_t event = p_args->event;

    if((UART_EVENT_RX_CHAR != event) &&
            (UART_EVENT_TX_COMPLETE != event))
    {
        /* This callback handles only new char received and Transmit data empty events */
        return;
    }

    if(UART_EVENT_RX_CHAR == event)
    {
        fsp_err_t err = FSP_SUCCESS;
        char c = (char)(p_args->data & UINT8_MAX);
        if('\b' == c)
        {
            err = R_BYTEQ_PopLast(&uart_rx_q_ctrl, (uint8_t*)&c);
            (void) err;
            return;
        }

        err = R_BYTEQ_Put(&uart_rx_q_ctrl, (uint8_t)(p_args->data & UINT8_MAX));

        if(FSP_ERR_OUT_OF_MEMORY == err)
        {
            /* Notify upper layer */
        }
    }

    if(UART_EVENT_TX_COMPLETE == event)
    {
        if (NULL != p_tx_memory)
        {
            free(p_tx_memory);
            p_tx_memory = NULL;
            tx_busy = false;
        }
    }

}

static int uart_getc(FILE *f)
{
    if (stdin != f)
    {
        return -1;
    }

    unsigned char c = 0xFF;
    uint32_t count = 0;

    fsp_err_t err = FSP_SUCCESS;

    err = jlob_uart_init_once ();
    if (FSP_SUCCESS != err)
    {
        return -1;
    }

    do
    {
        err = R_BYTEQ_Used(&uart_rx_q_ctrl, &count);
        if(FSP_SUCCESS != err)
        {
            return -1;
        }
    }while(0 == count);

    err = R_BYTEQ_Get(&uart_rx_q_ctrl, (uint8_t*)&c);
    if (FSP_SUCCESS == err)
    {
        int ret = c;
        return ret;
    }

    /* Return failure */
    return -1;
}

static int uart_flush(FILE *f)
{
    if((stdout != f) && (stdin != f) && (stderr != f))
    {
        return -1;
    }

    fsp_err_t err = FSP_SUCCESS;

    if (stdin == f)
    {
        err = R_BYTEQ_Flush(&uart_rx_q_ctrl);

        if (FSP_SUCCESS != err)
        {
            return -1;
        }

        return 0;
    }

    uint32_t tx_count = 0;


    err = jlob_uart_init_once();
    if (FSP_SUCCESS != err)
    {
        return -1;
    }

    err = R_BYTEQ_Used (&uart_tx_q_ctrl, &tx_count);
    if (FSP_SUCCESS != err)
    {
        return -1;
    }

    if( 0 == tx_count)
    {
        return 0;
    }

    /* Wait if the UART is busy. */
    while(true == tx_busy);

    /* Allocate tx_count worth memory */
    p_tx_memory = malloc(tx_count * sizeof(char));

    if(NULL == p_tx_memory)
    {
        return -1;
    }

    /* Unload queue into allocated memory */
    for(uint32_t itr = 0; (itr < tx_count) && (FSP_SUCCESS == err); itr++)
    {
        err = R_BYTEQ_Get(&uart_tx_q_ctrl, (uint8_t*)&p_tx_memory[itr]);
    }

    if(FSP_SUCCESS != err)
    {
        free(p_tx_memory);
        p_tx_memory = NULL;
        return -1;
    }

    /* Send allocated memory for transmission */
    err = p_uart_jlob_vcom->p_api->write(p_uart_jlob_vcom->p_ctrl, (uint8_t*)p_tx_memory, tx_count);

    if (FSP_SUCCESS != err)
    {
        return -1;
    }

    tx_busy = true;

    return 0;
}

static int uart_putc(char c, FILE *f)
{
    if ((stdout != f) && (stderr != f))
    {
        return -1;
    }

    uint32_t tx_available = 0;
    fsp_err_t err = FSP_SUCCESS;

    err = jlob_uart_init_once();
    if (FSP_SUCCESS != err)
    {
        return -1;
    }

    err = R_BYTEQ_Unused(&uart_tx_q_ctrl, &tx_available);

    if((tx_available == 0) && (FSP_SUCCESS == err))
    {
        /* No room left. Flush per the UNIX standard. */
        fflush(stdout);
    }

    /* Store char in a queue */
    err = R_BYTEQ_Put (&uart_tx_q_ctrl, (uint8_t) c);

    if ('\n' == c)
    {
        /* Newline character. Flush per the UNIX standard */
        fflush (stdout);
    }

    return c;
}

static fsp_err_t jlob_uart_init_once(void)
{
    fsp_err_t retval = FSP_SUCCESS; 
    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    if (g_stdio_uart_initialized)
    {
        retval = FSP_SUCCESS;
        goto jlob_comms_init_once_exit;
    }
    
    fsp_err_t err = FSP_SUCCESS;
    
#if defined(UNITY_FRAMEWORK_H)
    p_uart_jlob_vcom = &g_uart_jlob_vcom_fake;
#else
    extern uart_instance_t g_uart_jlob_vcom;
    p_uart_jlob_vcom = &g_uart_jlob_vcom;
#endif   
    
    if ((NULL == p_uart_jlob_vcom->p_api) ||
        (NULL == p_uart_jlob_vcom->p_ctrl) ||
        (NULL == p_uart_jlob_vcom->p_cfg))
    {
        retval = FSP_ERR_ASSERTION;
        goto jlob_comms_init_once_exit;
    }

    err = R_BYTEQ_Open(&uart_rx_q_ctrl, &uart_rx_q_cfg);
    if (FSP_SUCCESS != err)
    {
        retval = err;
        goto jlob_comms_init_once_exit;
    }

    err = R_BYTEQ_Open(&uart_tx_q_ctrl, &uart_tx_q_cfg);
    if (FSP_SUCCESS != err)
    {
        retval = err;
        goto jlob_comms_init_once_exit;
    }

    err = p_uart_jlob_vcom->p_api->open(p_uart_jlob_vcom->p_ctrl, p_uart_jlob_vcom->p_cfg);
    if (FSP_SUCCESS != err)
    {
        retval = err;
        goto jlob_comms_init_once_exit;
    }

    if(uart_jlob_vcom_callback != p_uart_jlob_vcom->p_cfg->p_callback)
    {
        err = p_uart_jlob_vcom->p_api->callbackSet(p_uart_jlob_vcom->p_ctrl, uart_jlob_vcom_callback, NULL, &uart_callback_args);

        if (FSP_SUCCESS != err)
        {
            retval = err;
            goto jlob_comms_init_once_exit;
        }
    }

    g_stdio_uart_initialized = true;
jlob_comms_init_once_exit:
    FSP_CRITICAL_SECTION_EXIT;
    return retval;
}



