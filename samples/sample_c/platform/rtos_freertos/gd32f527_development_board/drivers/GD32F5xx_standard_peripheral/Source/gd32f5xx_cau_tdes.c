/*!
    \file    gd32f5xx_cau_tdes.c
    \brief   CAU TDES driver

    \version 2023-12-20, V1.0.0, firmware for GD32F5xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f5xx_cau.h"

#define TDESBSY_TIMEOUT    ((uint32_t)0x00010000U)

/* TDES calculate process */
static ErrStatus cau_tdes_calculate(uint8_t *input, uint32_t in_length, uint8_t *output);

/*!
    \brief      encrypt and decrypt using TDES in ECB mode
    \param[in]  cau_parameter: pointer to the input structure
                  alg_dir: algorithm direction
                    CAU_ENCRYPT, CAU_DECRYPT
                  key: key, 24 bytes
                  input: input data
                  in_length: input data length in bytes, must be a multiple of 8 bytes
    \param[out] output: pointer to the output structure
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus cau_tdes_ecb(cau_parameter_struct *cau_parameter, uint8_t *output)
{
    ErrStatus ret = ERROR;
    cau_key_parameter_struct key_initpara;
    uint32_t keyaddr    = (uint32_t)(cau_parameter->key);
    uint32_t inputaddr  = (uint32_t)(cau_parameter->input);
    uint32_t outputaddr = (uint32_t)output;

    /* key structure initialization */
    cau_key_struct_para_init(&key_initpara);
    /* initialize the CAU peripheral */
    cau_init(cau_parameter->alg_dir, CAU_MODE_TDES_ECB, CAU_SWAPPING_8BIT);

    /* key initialization */
    key_initpara.key_1_high = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_1_low  = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_2_high = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_2_low  = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_3_high = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_3_low  = __REV(*(uint32_t *)(keyaddr));
    cau_key_init(&key_initpara);

    /* flush the IN and OUT FIFOs */
    cau_fifo_flush();

    /* enable the CAU peripheral */
    cau_enable();
    /* TDES calculate process */
    ret = cau_tdes_calculate((uint8_t *)inputaddr, cau_parameter->in_length, (uint8_t *)outputaddr);
    /* disable the CAU peripheral */
    cau_disable();

    return ret;
}

/*!
    \brief      encrypt and decrypt using TDES in CBC mode
    \param[in]  cau_parameter: pointer to the input structure
                  alg_dir: algorithm direction
                    CAU_ENCRYPT, CAU_DECRYPT
                  key: key, 24 bytes
                  iv: initialization vector, 8 bytes
                  input: input data
                  in_length: input data length in bytes, must be a multiple of 8 bytes
    \param[out] output: pointer to the output structure
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus cau_tdes_cbc(cau_parameter_struct *cau_parameter, uint8_t *output)
{
    ErrStatus ret = ERROR;
    cau_key_parameter_struct key_initpara;
    cau_iv_parameter_struct iv_initpara;
    uint32_t keyaddr    = (uint32_t)(cau_parameter->key);
    uint32_t inputaddr  = (uint32_t)(cau_parameter->input);
    uint32_t outputaddr = (uint32_t)output;
    uint32_t ivaddr     = (uint32_t)(cau_parameter->iv);

    /* key structure initialization */
    cau_key_struct_para_init(&key_initpara);
    /* initialize the CAU peripheral */
    cau_init(cau_parameter->alg_dir, CAU_MODE_TDES_CBC, CAU_SWAPPING_8BIT);

    /* key initialization */
    key_initpara.key_1_high = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_1_low  = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_2_high = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_2_low  = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_3_high = __REV(*(uint32_t *)(keyaddr));
    keyaddr += 4U;
    key_initpara.key_3_low  = __REV(*(uint32_t *)(keyaddr));
    cau_key_init(&key_initpara);

    /* vectors initialization */
    iv_initpara.iv_0_high = __REV(*(uint32_t *)(ivaddr));
    ivaddr += 4U;
    iv_initpara.iv_0_low  = __REV(*(uint32_t *)(ivaddr));
    cau_iv_init(&iv_initpara);

    /* flush the IN and OUT FIFOs */
    cau_fifo_flush();
    /* enable the CAU peripheral */
    cau_enable();
    /* TDES calculate process */
    ret = cau_tdes_calculate((uint8_t *)inputaddr, cau_parameter->in_length, (uint8_t *)outputaddr);
    /* disable the CAU peripheral */
    cau_disable();

    return ret;
}

/*!
    \brief      TDES calculate process
    \param[in]  input: pointer to the input buffer
    \param[in]  in_length: length of the input buffer in bytes, must be a multiple of 8 bytes
    \param[out] output: pointer to the returned buffer
    \retval     ErrStatus: SUCCESS or ERROR
*/
static ErrStatus cau_tdes_calculate(uint8_t *input, uint32_t in_length, uint8_t *output)
{
    uint32_t inputaddr  = (uint32_t)input;
    uint32_t outputaddr = (uint32_t)output;
    uint32_t i = 0U;
    __IO uint32_t counter = 0U;
    uint32_t busystatus = 0U;

    /* the clock is not enabled or there is no embeded CAU peripheral */
    if(DISABLE == cau_enable_state_get()) {
        return ERROR;
    }

    for(i = 0U; i < in_length; i += 8U) {
        /* write data to the IN FIFO */
        cau_data_write(*(uint32_t *)(inputaddr));
        inputaddr += 4U;
        cau_data_write(*(uint32_t *)(inputaddr));
        inputaddr += 4U;

        /* wait until the complete message has been processed */
        counter = 0U;
        do {
            busystatus = cau_flag_get(CAU_FLAG_BUSY);
            counter++;
        } while((TDESBSY_TIMEOUT != counter) && (RESET != busystatus));

        if(RESET != busystatus) {
            return ERROR;
        } else {
            /* read the output block from the output FIFO */
            *(uint32_t *)(outputaddr) = cau_data_read();
            outputaddr += 4U;
            *(uint32_t *)(outputaddr) = cau_data_read();
            outputaddr += 4U;
        }
    }

    return SUCCESS;
}