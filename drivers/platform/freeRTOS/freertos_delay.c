/***************************************************************************//**
 *   @file   freeRTOS/freertos_delay.c
 *   @brief  Implementation of freertos delay functions.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "no_os_delay.h"
#include "task.h"

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 */

void no_os_udelay(uint32_t usecs)
{
	vTaskDelay((uint64_t)configTICK_RATE_HZ * usecs / 1000000);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 */
void no_os_mdelay(uint32_t msecs)
{
	vTaskDelay(configTICK_RATE_HZ * msecs / 1000);
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t;

	TickType_t _system_ticks  = xTaskGetTickCount();

	t.s = _system_ticks / configTICK_RATE_HZ;
	t.us = 1000000ULL * (_system_ticks % configTICK_RATE_HZ) / configTICK_RATE_HZ;

	return t;
}
