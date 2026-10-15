#ifndef R_BYTEQ_H
#define R_BYTEQ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* FSP common error codes and type. */
#include "bsp_api.h"

/***********************************************************************************************************************
 * Versioning
 **********************************************************************************************************************/
#define BYTEQ_VERSION_MAJOR        (1u)
#define BYTEQ_VERSION_MINOR        (0u)
#define BYTEQ_VERSION_PATCH        (0u)

/* Packed into 0xMMMMmmpp: Major(16) Minor(8) Patch(8) */
#define BYTEQ_MODULE_VERSION       (((uint32_t) BYTEQ_VERSION_MAJOR << 16) | \
                                    ((uint32_t) BYTEQ_VERSION_MINOR << 8)  | \
                                    ((uint32_t) BYTEQ_VERSION_PATCH))

/* Control block ABI version (increment on layout/behavior changes that break compatibility) */
#define BYTEQ_STATE_VERSION        (1u)

/***********************************************************************************************************************
 * Public Types
 **********************************************************************************************************************/

 /***********************************************************************************************************************
 * Callback Support
 **********************************************************************************************************************/

typedef enum e_byteq_event
{
    BYTEQ_EVENT_NONE            = 0,
    BYTEQ_EVENT_OUT_OF_MEMORY   = (1u << 1),
    BYTEQ_EVENT_EMPTY           = (1u << 2),
} byteq_event_t;

/** Arguments passed to user callback */
typedef struct st_byteq_callback_args
{
    void      * p_context;
    fsp_err_t   error;
    uint32_t    event;
    uint32_t    data;      /* optional: e.g. used count */
} byteq_callback_args_t;

typedef void (* byteq_callback_t)(byteq_callback_args_t * p_args);

/** Configuration passed into R_BYTEQ_Open(). */
typedef struct st_byteq_cfg
{
    uint8_t  * p_buffer;
    uint32_t   buffer_size;

    /* NEW */
    byteq_callback_t p_callback;
    void           * p_context;
    uint32_t         event_mask;
} byteq_cfg_t;

/**
 * Queue state control block.
 * Rule: first two elements are version and size and must remain first/unchanged ordering.
 */
typedef struct st_byteq_state_ctrl
{
    uint32_t version;
    uint32_t size;

    uint8_t  * p_buffer;
    uint32_t   capacity;

    uint32_t   head;
    uint32_t   tail;
    uint32_t   used;

    bool       is_open;

    /* NEW (safe extension) */
    byteq_callback_t callback;
    void           * p_context;
    uint32_t         event_mask;
} byteq_state_ctrl_t;

/***********************************************************************************************************************
 * Public API
 **********************************************************************************************************************/

/**
 * Allocates/initializes a queue control block for a user-provided buffer.
 *
 * @param[in,out] p_ctrl   Queue control block (user allocated).
 * @param[in]     p_cfg    Configuration containing buffer pointer and size.
 */
fsp_err_t R_BYTEQ_Open(byteq_state_ctrl_t * const p_ctrl, byteq_cfg_t const * const p_cfg);

/**
 * Releases the queue control block (marks it closed; does not free user memory).
 */
fsp_err_t R_BYTEQ_Close(byteq_state_ctrl_t * const p_ctrl);

/**
 * Adds a byte of data to the queue.
 */
fsp_err_t R_BYTEQ_Put(byteq_state_ctrl_t * const p_ctrl, uint8_t const byte);

/**
 * Removes a byte of data from the queue.
 */
fsp_err_t R_BYTEQ_Get(byteq_state_ctrl_t * const p_ctrl, uint8_t * const p_byte);

/**
 * Resets a queue to an empty state.
 */
fsp_err_t R_BYTEQ_Flush(byteq_state_ctrl_t * const p_ctrl);

/**
 * Provides the number of data bytes in the queue.
 */
fsp_err_t R_BYTEQ_Used(byteq_state_ctrl_t * const p_ctrl, uint32_t * const p_cnt);

/**
 * Provides the number of data bytes available for storage in the queue.
 */
fsp_err_t R_BYTEQ_Unused(byteq_state_ctrl_t * const p_ctrl, uint32_t * const p_cnt);

fsp_err_t R_BYTEQ_CallbackSet(byteq_state_ctrl_t * const p_ctrl,
                              byteq_callback_t             p_callback,
                              void *                       p_context,
                              uint32_t                     event_mask);

/**
 * Returns the last byte written to the queue without removing it.
 */
fsp_err_t R_BYTEQ_PeekLast(byteq_state_ctrl_t * const p_ctrl,
                           uint8_t * const p_byte);

/**
 * Removes and returns the last byte written to the queue.
 */
fsp_err_t R_BYTEQ_PopLast(byteq_state_ctrl_t * const p_ctrl,
                          uint8_t * const p_byte);

/**
 * Returns the version number at runtime.
 */
fsp_err_t R_BYTEQ_GetVersion(byteq_state_ctrl_t * const p_ctrl, uint32_t * const p_version);

#ifdef __cplusplus
}
#endif

#endif /* BYTEQ_H */
