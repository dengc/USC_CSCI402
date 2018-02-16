/******************************************************************************/
/* Important Spring 2017 CSCI 402 usage information:                          */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5f2e8d450c0c5851acd538befe33744efca0f1c4f9fb5f       */
/*         3c8feabc561a99e53d4d21951738da923cd1c7bbd11b30a1afb11172f80b       */
/*         984b1acfbbf8fae6ea57e0583d2610a618379293cb1de8e1e9d07e6287e8       */
/*         de7e82f3d48866aa2009b599e92c852f7dbf7a6e573f1c7228ca34b9f368       */
/*         faaef0c0fcf294cb                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

#include "types.h"
#include "globals.h"
#include "errno.h"

#include "util/debug.h"
#include "util/string.h"

#include "proc/proc.h"
#include "proc/kthread.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/pframe.h"
#include "mm/mmobj.h"
#include "mm/pagetable.h"
#include "mm/tlb.h"

#include "fs/file.h"
#include "fs/vnode.h"

#include "vm/shadow.h"
#include "vm/vmmap.h"

#include "api/exec.h"

#include "main/interrupt.h"

/* Pushes the appropriate things onto the kernel stack of a newly forked thread
 * so that it can begin execution in userland_entry.
 * regs: registers the new thread should have on execution
 * kstack: location of the new thread's kernel stack
 * Returns the new stack pointer on success. */
static uint32_t
fork_setup_stack(const regs_t *regs, void *kstack)
{
        /* Pointer argument and dummy return address, and userland dummy return
         * address */
        uint32_t esp = ((uint32_t) kstack) + DEFAULT_STACK_SIZE - (sizeof(regs_t) + 12);
        *(void **)(esp + 4) = (void *)(esp + 8); /* Set the argument to point to location of struct on stack */
        memcpy((void *)(esp + 8), regs, sizeof(regs_t)); /* Copy over struct */
        return esp;
}

/*
 * The implementation of fork(2). Once this works,
 * you're practically home free. This is what the
 * entirety of Weenix has been leading up to.
 * Go forth and conquer.
 */
int
do_fork(struct regs *regs)
{
		KASSERT(regs != NULL);
		KASSERT(curproc != NULL);
		KASSERT(curproc->p_state == PROC_RUNNING);
		dbg(DBG_PRINT, "(GRADING3A 7.a)\n");
		
		
		/*Inside fork() a process creates a copy of itself, fork() return pid of child process in parent and 0
		 * in child. */
		  proc_t *obj = proc_create("child");
		
		  kthread_t *nthr = kthread_clone(curthr);
		  
		  
		  
		  nthr->kt_proc = obj;
		  
		  nthr->kt_kstack = strcpy(nthr->kt_kstack,curthr->kt_kstack);
		  
		  list_insert_tail(&((*obj).p_threads),&((*nthr).kt_plink));
		  
		  obj->p_status = curproc->p_status;
		  obj->p_state = curproc->p_state;
	  
		  vmmap_destroy(obj->p_vmmap);
		  obj->p_vmmap = vmmap_clone(curproc->p_vmmap);
		  
		  obj->p_vmmap->vmm_proc = obj;
			
        
		  obj->p_brk = curproc->p_brk;
		  obj->p_start_brk = curproc->p_start_brk;
      
		  obj->p_cwd = curproc->p_cwd;
      
		  int i;

			for(i=0;i<32;i++)
				{
					obj->p_files[i] = curproc->p_files[i];
					if(curproc->p_files[i]!=NULL)
					{
						dbg(DBG_PRINT, "(GRADING3D)\n");
						fref(curproc->p_files[i]);
					}
			}

		pt_unmap_range(curproc->p_pagedir,USER_MEM_LOW,USER_MEM_HIGH);	
		
		tlb_flush_all();
		
		regs->r_eax = 0;
		
		nthr->kt_ctx.c_pdptr = obj->p_pagedir;
		nthr->kt_ctx.c_eip = (uint32_t) userland_entry;
		nthr->kt_ctx.c_esp = fork_setup_stack(regs,nthr->kt_kstack);
		nthr->kt_ctx.c_kstack = (uintptr_t)nthr->kt_kstack;
		nthr->kt_ctx.c_kstacksz = DEFAULT_STACK_SIZE;
		
		
		regs->r_eax = obj->p_pid;	
		
		sched_make_runnable(nthr);

		KASSERT(obj->p_state == PROC_RUNNING);
		KASSERT(obj->p_pagedir != NULL);
		KASSERT(nthr->kt_kstack != NULL);

		return obj->p_pid;

/*        NOT_YET_IMPLEMENTED("VM: do_fork");
        return 0;*/
}
