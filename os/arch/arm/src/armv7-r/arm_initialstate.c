/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 *  arch/arm/src/armv7-r/arm_initialstate.c
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>

#include <stdint.h>
#include <string.h>

#include <tinyara/arch.h>

#include "arm.h"
#include "up_internal.h"
#include "up_arch.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_initial_state
 *
 * Description:
 *   A new thread is being started and a new TCB has been created. This
 *   function is called to initialize the processor specific portions of
 *   the new TCB.
 *
 *   This function must setup the initial architecture registers and/or
 *   stack so that execution will begin at tcb->start on the next context
 *   switch.
 *
 ****************************************************************************/

void up_initial_state(struct tcb_s *tcb)
{
	struct xcptcontext *xcp = &tcb->xcp;
	uint32_t cpsr;

	/* Initialize the initial exception register context structure */

	memset(xcp, 0, sizeof(struct xcptcontext));

	/* Save the initial stack pointer */

	xcp->regs[REG_SP] = (uint32_t)tcb->adj_stack_ptr;

	/* Save the task entry point */

	xcp->regs[REG_PC] = (uint32_t)tcb->start;

#if defined(CONFIG_BUILD_PROTECTED)
	up_setup_regions(tcb, tcb->flags);
#endif

	/* If this task is running PIC, then set the PIC base register to the
	 * address of the allocated D-Space region.
	 */

#ifdef CONFIG_PIC
	if (tcb->dspace != NULL) {
		/* Set the PIC base register (probably R10) to the address of the
		 * alloacated D-Space region.
		 */

		xcp->regs[REG_PIC] = (uint32_t)tcb->dspace->region;
	}
#endif

	/* Set supervisor-mode and disable FIQs, regardless of how TinyAra is
	 * configured and of what kind of thread is being started.  That is
	 * because all threads, even user-mode threads will start in kernel
	 * trampoline at task_start() or pthread_start().  The thread's
	 * privileges will be dropped before transitioning to user code.
	 */

	cpsr = PSR_MODE_SVC;

	/* Enable or disable interrupts, based on user configuration */

#ifdef CONFIG_SUPPRESS_INTERRUPTS
	/* Disable interrupts (both IRQs and FIQs) */

	cpsr |= (PSR_I_BIT | PSR_F_BIT);

#else							/* CONFIG_SUPPRESS_INTERRUPTS */
	/* Leave IRQs enabled (Also FIQs if CONFIG_ARMV7R_DECODEFIQ is selected) */

#ifndef CONFIG_ARMV7R_DECODEFIQ

	cpsr |= PSR_F_BIT;

#endif							/* !CONFIG_ARMV7R_DECODEFIQ */
#endif							/* CONFIG_SUPPRESS_INTERRUPTS */

#ifdef CONFIG_ARCH_THUMB
	cpsr |= PSR_T_BIT;
#endif

	xcp->regs[REG_CPSR] = cpsr;
}
