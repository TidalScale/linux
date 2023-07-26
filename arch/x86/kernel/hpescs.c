/* Copyright 2023 Hewlett Packard Enterprise Development LP */
#include <linux/types.h>
#include <linux/init.h>
#include <linux/jiffies.h>

#include <asm/hpescs.h>
#include <asm/pgtable_types.h>

#define TS_HC_SET_JIFFIES_64    0xFB

struct x86_registers {
        u64 rax;
        u64 rcx;
        u64 rdx;
        u64 rbx;
        u64 rsp;
        u64 rbp;
        u64 rsi;
        u64 rdi;
        u64 r8;
        u64 r9;
        u64 r10;
        u64 r11;
        u64 r12;
        u64 r13;
        u64 r14;
        u64 r15;
};

static inline void
vmcall(struct x86_registers *registers)
{
    asm volatile("vmcall"
		 : "=a" (registers->rax), "=b" (registers->rbx),
                   "=c" (registers->rcx), "=d" (registers->rdx),
                   "=S" (registers->rsi), "=D" (registers->rdi),
		   "=r" (registers->r8),  "=r" (registers->r9),
                   "=r" (registers->r10), "=r" (registers->r11),
		   "=r" (registers->r12), "=r" (registers->r13),
                   "=r" (registers->r14), "=r" (registers->r15)
		 :  "0" (registers->rax),  "1" (registers->rbx),
                    "2" (registers->rcx),  "3" (registers->rdx),
                    "4" (registers->rsi),  "5" (registers->rdi),
		    "6" (registers->r8),   "7" (registers->r9),
                    "8" (registers->r10),  "9" (registers->r11),
		   "10" (registers->r12), "11" (registers->r13),
                   "12" (registers->r14), "13" (registers->r15)
                 : "cc", "memory");
}

int
hpe_scs_setup_jiffies(u64 jiffies, u64 tsc)
{
        struct x86_registers registers = { 0 };

        registers.rax = TS_HC_SET_JIFFIES_64;
        registers.rbx = slow_virt_to_phys(&jiffies_64);
        registers.rcx = jiffies;
        registers.rdx = tsc;
        registers.rsi = CONFIG_HZ;

        vmcall(&registers);

	return 0;
}
