/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl38x project.
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example_main.h"
#endif

#ifdef SELFTEST_EXAMPLE
#include "selftest_example_main.h"
#endif

#ifdef FIFO_EXAMPLE
#include "fifo_example_main.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	adxl38x_ip.comm_init.spi_init = adxl38x_spi_ip;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adxl38x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#ifdef SELFTEST_EXAMPLE
	ret = selftest_example_main();
#endif

#ifdef FIFO_EXAMPLE
	ret = fifo_example_main();
#endif

#if (BASIC_EXAMPLE + SELFTEST_EXAMPLE + FIFO_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + SELFTEST_EXAMPLE + FIFO_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	no_os_uart_remove(uart_desc);
	return ret;
}

