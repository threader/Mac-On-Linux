/* 
 *	<hostirq.c>
 *	
 *	host IRQ handling (for pciproxied devices)
 *   
 *   Copyright (C) 2005 Mattias Nissler <mattias.nissler@gmx.de>
 *   
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   version 2
 *   
 */

#include "kernel_vars.h"
#include <linux/interrupt.h>
#include <linux/irq.h>
#include "misc.h"

/*
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/bitops.h>
#include <asm/atomic.h>

#include "archinclude.h"
*/

irqreturn_t hostirq_handler(int irq, void *pkv)
{
	siginfo_t si;
	kernel_vars_t *kv = (kernel_vars_t *) pkv;

	/* disable the irq */
	disable_irq_nosync(irq);
	/* have the interrupt handled */
	if (!test_and_set_bit(irq, kv->mregs.active_irqs.irqs))
		atomic_inc((atomic_t *) &
			       (kv->mregs.hostirq_active_cnt));
	kv->mregs.hostirq_update = 1;
	kv->mregs.interrupt = 1;
	/* signal the main thread (it might be DOZEing) */
	if (kv->main_thread != NULL) {
		memset(&si, 0, sizeof(si));
		si.si_signo = SIGHUP;
		si.si_code = irq;
		send_sig_info(SIGHUP, &si, kv->main_thread);
	}

	return IRQ_HANDLED;
}

static char *molirqdescstring = "MOL irq mapping";

int grab_host_irq(kernel_vars_t * kv, int irq)
{
	int ret;

	/* sanity check */
	if (irq < 0 || irq >= NR_HOST_IRQS
	    || check_bit_mol(irq, (char *)kv->mregs.mapped_irqs.irqs))
		return 0;

	/* request the irq */
	ret =
	    request_irq(irq, hostirq_handler, IRQF_DISABLED | IRQF_SHARED,
			molirqdescstring, kv);
	if (!ret) {
//              printk(KERN_INFO "mapped irq line %d\n", irq);
		set_bit_mol(irq, (char *)kv->mregs.mapped_irqs.irqs);
	}

	return ret;
}

int release_host_irq(kernel_vars_t * kv, int irq)
{
	/* sanity check */
	if (irq < 0 || irq >= NR_HOST_IRQS
	    || !check_bit_mol(irq, (char *)kv->mregs.mapped_irqs.irqs))
		return 0;

	clear_bit_mol(irq, (char *)kv->mregs.mapped_irqs.irqs);
	disable_irq(irq);
	free_irq(irq, kv);

	return 1;
}

void init_host_irqs(kernel_vars_t * kv)
{
	memset(&(kv->mregs.mapped_irqs), 0, sizeof(kv->mregs.mapped_irqs));
	kv->main_thread = current;
	kv->mregs.hostirq_update = 0;
}

void cleanup_host_irqs(kernel_vars_t * kv)
{
	int n;

	for (n = 0; n < NR_HOST_IRQS; n++) {
		if (check_bit_mol(n, (char *)kv->mregs.mapped_irqs.irqs))
			release_host_irq(kv, n);
	}
}
