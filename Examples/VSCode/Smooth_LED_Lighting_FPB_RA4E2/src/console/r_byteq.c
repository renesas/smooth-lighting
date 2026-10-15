#include "r_byteq.h"

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#include <stddef.h>
_Static_assert(offsetof(byteq_state_ctrl_t, version) == 0u, "version must be first element");
_Static_assert(offsetof(byteq_state_ctrl_t, size)    == 4u, "size must be second element");
#endif

static inline void byteq_notify(byteq_state_ctrl_t * p_ctrl,
                                fsp_err_t error,
                                uint32_t event)
{
    if ((p_ctrl->callback != NULL) &&
        (p_ctrl->event_mask & event))
    {
        byteq_callback_args_t args =
        {
            .p_context = p_ctrl->p_context,
            .error     = error,
            .event     = event,
            .data      = p_ctrl->used,
        };

        p_ctrl->callback(&args);
    }
}

static inline fsp_err_t byteq_open_check_(byteq_state_ctrl_t const * const p_ctrl)
{
    if (NULL == p_ctrl)
    {
        return FSP_ERR_ASSERTION;
    }

    if (false == p_ctrl->is_open)
    {
        return FSP_ERR_NOT_OPEN;
    }

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Open(byteq_state_ctrl_t * const p_ctrl, byteq_cfg_t const * const p_cfg)
{
    if ((NULL == p_ctrl) || (NULL == p_cfg))
    {
        return FSP_ERR_ASSERTION;
    }

    if (p_ctrl->is_open)
    {
        return FSP_ERR_ALREADY_OPEN;
    }

    if ((NULL == p_cfg->p_buffer) || (p_cfg->buffer_size < 1u))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Initialize control block (contiguous state only). */
    p_ctrl->version   = BYTEQ_STATE_VERSION;
    p_ctrl->size      = (uint32_t) sizeof(byteq_state_ctrl_t);

    p_ctrl->p_buffer  = p_cfg->p_buffer;
    p_ctrl->capacity  = p_cfg->buffer_size;

    p_ctrl->event_mask = p_cfg->event_mask;
    p_ctrl->callback = p_cfg->p_callback;
    p_ctrl->p_context = p_cfg->p_context;
    
    p_ctrl->head      = 0u;
    p_ctrl->tail      = 0u;
    p_ctrl->used      = 0u;

    p_ctrl->is_open   = true;


    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Close(byteq_state_ctrl_t * const p_ctrl)
{
    fsp_err_t err = byteq_open_check_(p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Mark closed and scrub pointers/state (does not free user buffer). */
    p_ctrl->is_open  = false;
    p_ctrl->p_buffer = NULL;
    p_ctrl->capacity = 0u;
    p_ctrl->head     = 0u;
    p_ctrl->tail     = 0u;
    p_ctrl->used     = 0u;

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Put(byteq_state_ctrl_t * const p_ctrl, uint8_t const byte)
{
    fsp_err_t err = byteq_open_check_(p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (p_ctrl->used >= p_ctrl->capacity)
    {
        byteq_notify(p_ctrl, FSP_ERR_OUT_OF_MEMORY, BYTEQ_EVENT_OUT_OF_MEMORY);
        
        return FSP_ERR_OUT_OF_MEMORY;
    }

    p_ctrl->p_buffer[p_ctrl->head] = byte;

    p_ctrl->head++;
    if (p_ctrl->head >= p_ctrl->capacity)
    {
        p_ctrl->head = 0u;
    }

    p_ctrl->used++;

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Get(byteq_state_ctrl_t * const p_ctrl, uint8_t * const p_byte)
{
    fsp_err_t err = byteq_open_check_(p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (NULL == p_byte)
    {
        return FSP_ERR_ASSERTION;
    }

    if (0u == p_ctrl->used)
    {        
        byteq_notify(p_ctrl, FSP_ERR_BUFFER_EMPTY, BYTEQ_EVENT_EMPTY);
        return FSP_ERR_BUFFER_EMPTY;
    }

    *p_byte = p_ctrl->p_buffer[p_ctrl->tail];

    p_ctrl->tail++;
    if (p_ctrl->tail >= p_ctrl->capacity)
    {
        p_ctrl->tail = 0u;
    }

    p_ctrl->used--;

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Flush(byteq_state_ctrl_t * const p_ctrl)
{
    fsp_err_t err = byteq_open_check_(p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    p_ctrl->head = 0u;
    p_ctrl->tail = 0u;
    p_ctrl->used = 0u;

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Used(byteq_state_ctrl_t * const p_ctrl, uint32_t * const p_cnt)
{
    fsp_err_t err = byteq_open_check_(p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (NULL == p_cnt)
    {
        return FSP_ERR_ASSERTION;
    }

    *p_cnt = p_ctrl->used;
    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_Unused(byteq_state_ctrl_t * const p_ctrl, uint32_t * const p_cnt)
{
    fsp_err_t err = byteq_open_check_(p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (NULL == p_cnt)
    {
        return FSP_ERR_ASSERTION;
    }

    *p_cnt = (p_ctrl->capacity - p_ctrl->used);
    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_PeekLast(byteq_state_ctrl_t * const p_ctrl,
                           uint8_t * const p_byte)
{
    if ((p_ctrl == NULL) || (p_byte == NULL))
    {
        return FSP_ERR_ASSERTION;
    }

    if (0U == p_ctrl->used)
    {
        byteq_notify(p_ctrl, FSP_ERR_BUFFER_EMPTY, BYTEQ_EVENT_EMPTY);
        return FSP_ERR_BUFFER_EMPTY;
    }

    uint32_t idx = (p_ctrl->head == 0U)
                   ? (p_ctrl->capacity - 1U)
                   : (p_ctrl->head - 1U);

    *p_byte = p_ctrl->p_buffer[idx];

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_PopLast(byteq_state_ctrl_t * const p_ctrl,
                          uint8_t * const p_byte)
{
    if ((p_ctrl == NULL) || (p_byte == NULL))
    {
        return FSP_ERR_ASSERTION;
    }

    if (0U == p_ctrl->used)
    {
        byteq_notify(p_ctrl, FSP_ERR_BUFFER_EMPTY, BYTEQ_EVENT_EMPTY);
        return FSP_ERR_BUFFER_EMPTY;
    }

    p_ctrl->head = (p_ctrl->head == 0U)
                   ? (p_ctrl->capacity - 1U)
                   : (p_ctrl->head - 1U);

    *p_byte = p_ctrl->p_buffer[p_ctrl->head];

    p_ctrl->used--;

    return FSP_SUCCESS;
}

fsp_err_t R_BYTEQ_GetVersion(byteq_state_ctrl_t * const p_ctrl, uint32_t * const p_version)
{
    /* State is still first argument per your rule set, even though this call does not require Open. */
    if ((NULL == p_ctrl) || (NULL == p_version))
    {
        return FSP_ERR_ASSERTION;
    }

    *p_version = BYTEQ_MODULE_VERSION;
    return FSP_SUCCESS;
}
