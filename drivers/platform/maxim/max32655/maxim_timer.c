/***************************************************************************//**
 *   @file   maxim_timer.c
 *   @brief  Source file for Maxim timer platform driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "tmr.h"
#include "maxim_timer.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Convert the prescaler from a clock division value to
 * an enum's value for the init function.
 * @param div - clock division value (this should be a power of 2).
 * @param prescaler - enum value to be provided to the init function.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int _get_prescaler(uint32_t div, mxc_tmr_pres_t *prescaler)
{
	if (div > 4096)
		return -EINVAL;

	if (div < 2)
		*prescaler = TMR_PRES_1;
	else if (div < 4)
		*prescaler = TMR_PRES_2;
	else if (div < 8)
		*prescaler = TMR_PRES_4;
	else if (div < 16)
		*prescaler = TMR_PRES_8;
	else if (div < 32)
		*prescaler = TMR_PRES_16;
	else if (div < 64)
		*prescaler = TMR_PRES_32;
	else if (div < 128)
		*prescaler = TMR_PRES_64;
	else if (div < 256)
		*prescaler = TMR_PRES_128;
	else if (div < 512)
		*prescaler = TMR_PRES_256;
	else if (div < 1024)
		*prescaler = TMR_PRES_512;
	else if (div < 2048)
		*prescaler = TMR_PRES_1024;
	else if (div < 4096)
		*prescaler = TMR_PRES_2048;
	else
		*prescaler = TMR_PRES_4096;

	return 0;
}

/**
 * @brief Timer driver init function
 * @param desc - timer descriptor to be initialized
 * @param param - initialization parameter for the desc
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int max_timer_init(struct no_os_timer_desc **desc,
		   const struct no_os_timer_init_param *param)
{
	int ret;
	mxc_tmr_pres_t prescaler;
	mxc_tmr_cfg_t *cfg;
	mxc_tmr_regs_t *tmr_regs;
	struct no_os_timer_desc *descriptor;

	if (!param || param->id >= MXC_CFG_TMR_INSTANCES)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	cfg = no_os_calloc(1, sizeof(*cfg));
	if (!cfg) {
		ret = -ENOMEM;
		goto free_descriptor;
	}

	descriptor->id = param->id;
	descriptor->freq_hz = param->freq_hz;
	descriptor->ticks_count = param->ticks_count;

	tmr_regs = MXC_TMR_GET_TMR(param->id);

	prescaler = SOURCE_CLOCK_FREQ / param->freq_hz;
	ret = _get_prescaler(prescaler, &prescaler);
	if (ret)
		goto free_cfg;

	cfg->bitMode = TMR_BIT_MODE_32;
	cfg->mode = TMR_MODE_CONTINUOUS;
	cfg->clock = MXC_TMR_APB_CLK;
	cfg->cmp_cnt = descriptor->ticks_count;
	cfg->pol = 1;
	cfg->pres = prescaler;

	descriptor->extra = cfg;

	MXC_TMR_Shutdown(tmr_regs);
	ret = MXC_TMR_Init(tmr_regs, cfg, 0);
	if (ret) {
		ret = -EINVAL;
		goto free_cfg;
	}

	*desc = descriptor;

	return 0;

free_cfg:
	no_os_free(cfg);
free_descriptor:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Timer driver remove function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Shutdown(MXC_TMR_GET_TMR(desc->id));
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Timer count start function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_start(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Start(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Timer count stop function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_stop(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Stop(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Function to get the current timer counter value
 * @param desc - timer descriptor
 * @param counter - the timer counter value
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_counter_get(struct no_os_timer_desc *desc, uint32_t *counter)
{
	if (!desc)
		return -EINVAL;

	*counter = MXC_TMR_GetCount(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Function to set the timer counter value
 * @param desc - timer descriptor
 * @param new_val - timer counter value to be set
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_counter_set(struct no_os_timer_desc *desc, uint32_t new_val)
{
	if (!desc || new_val >= SOURCE_CLOCK_FREQ)
		return -EINVAL;

	MXC_TMR_SetCount(MXC_TMR_GET_TMR(desc->id), new_val);

	return 0;
}

/**
 * @brief Function to get the timer frequency
 * @param desc - timer descriptor
 * @param freq_hz - the timer frequency value
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_count_clk_get(struct no_os_timer_desc *desc, uint32_t *freq_hz)
{
	if (!desc)
		return -EINVAL;

	*freq_hz = desc->freq_hz;

	return 0;
}

/**
 * @brief Function to set the timer frequency.
 * @param desc - timer descriptor.
 * @param freq_hz - the timer frequency value to be set.
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int max_timer_count_clk_set(struct no_os_timer_desc *desc, uint32_t freq_hz)
{
	int ret;
	uint32_t div;
	mxc_tmr_pres_t prescaler;
	uint32_t counter;
	uint32_t is_enabled;
	mxc_tmr_cfg_t *cfg;

	if (!desc)
		return -EINVAL;

	is_enabled = (MXC_TMR_GET_TMR(desc->id)->ctrl0 & NO_OS_BIT(15));
	div = SOURCE_CLOCK_FREQ / freq_hz;
	ret = _get_prescaler(div, &prescaler);
	if (ret)
		return ret;

	cfg = desc->extra;
	cfg->pres = prescaler;

	ret = no_os_timer_counter_get(desc, &counter);
	if (ret)
		return ret;

	MXC_TMR_Shutdown(MXC_TMR_GET_TMR(desc->id));
	ret = MXC_TMR_Init(MXC_TMR_GET_TMR(desc->id), cfg, 0);
	if (ret)
		return -EINVAL;

	MXC_TMR_SetCount(MXC_TMR_GET_TMR(desc->id), counter);
	if (is_enabled)
		MXC_TMR_Start(MXC_TMR_GET_TMR(desc->id));

	desc->freq_hz = freq_hz;

	return 0;
}

/**
 * @brief Not implemented
 * @param desc - timer descriptor
 * @param elapsed_time - time in nanoseconds
 * @return -ENOSYS
 */
int max_get_elapsed_nsec(struct no_os_timer_desc *desc, uint64_t *elapsed_time)
{
	return -ENOSYS;
}

/**
 * @brief maxim platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops max_timer_ops = {
	.init = (int32_t (*)())max_timer_init,
	.start = (int32_t (*)())max_timer_start,
	.stop = (int32_t (*)())max_timer_stop,
	.counter_get = (int32_t (*)())max_timer_counter_get,
	.counter_set = (int32_t (*)())max_timer_counter_set,
	.count_clk_get = (int32_t (*)())max_timer_count_clk_get,
	.count_clk_set = (int32_t (*)())max_timer_count_clk_set,
	.get_elapsed_time_nsec = (int32_t (*)())max_get_elapsed_nsec,
	.remove = (int32_t (*)())max_timer_remove
};
