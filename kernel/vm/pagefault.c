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
#include "kernel.h"
#include "errno.h"

#include "util/debug.h"

#include "proc/proc.h"
#include "api/access.h"
#include "vm/shadow.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/pagetable.h"

#include "vm/pagefault.h"
#include "vm/vmmap.h"



/*
 * This gets called by _pt_fault_handler in mm/pagetable.c The
 * calling function has already done a lot of error checking for
 * us. In particular it has checked that we are not page faulting
 * while in kernel mode. Make sure you understand why an
 * unexpected page fault in kernel mode is bad in Weenix. You
 * should probably read the _pt_fault_handler function to get a
 * sense of what it is doing.
 *
 * Before you can do anything you need to find the vmarea that
 * contains the address that was faulted on. Make sure to check
 * the permissions on the area to see if the process has
 * permission to do [cause]. If either of these checks does not
 * pass kill the offending process, setting its exit status to
 * EFAULT (normally we would send the SIGSEGV signal, however
 * Weenix does not support signals).
 *
 * Now it is time to find the correct page (don't forget
 * about shadow objects, especially copy-on-write magic!). Make
 * sure that if the user writes to the page it will be handled
 * correctly.
 *
 * Finally call pt_map to have the new mapping placed into the
 * appropriate page table.
 *
 * @param vaddr the address that was accessed to cause the fault
 *
 * @param cause this is the type of operation on the memory
 *              address which caused the fault, possible values
 *              can be found in pagefault.h
 */
void
handle_pagefault(uintptr_t vaddr, uint32_t cause)
{
    if(USER_MEM_LOW > vaddr || USER_MEM_HIGH <= vaddr){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        do_exit(EFAULT);
    }
    
    uint32_t pn = ADDR_TO_PN(vaddr);
    vmarea_t * area = vmmap_lookup(curproc->p_vmmap, pn);
    if(area == NULL){
		dbg(DBG_PRINT, "(GRADING3D)\n");
            do_exit(EFAULT);
    }
    
    int check = PROT_READ;
    if (cause & FAULT_WRITE) {
		dbg(DBG_PRINT, "(GRADING3D)\n");
        check |= PROT_WRITE;
    }
    if(!addr_perm(curproc, (void*)vaddr, check)){
        dbg(DBG_PRINT, "(GRADING3D)\n");
        do_exit(EFAULT);
    }
        
  if(FAULT_WRITE & cause)
    {
		dbg(DBG_PRINT, "(GRADING3D)\n");
		if(area->vma_flags & MAP_PRIVATE)
		{
			dbg(DBG_PRINT, "(GRADING3D)\n");
			/*if(!(area->vma_obj->mmo_shadowed))			
			{
				dbg(DBG_PRINT, "(GRADING3C74\n");
				mmobj_t *sh_obj;
				sh_obj = shadow_create();
				sh_obj->mmo_un.mmo_bottom_obj = mmobj_bottom_obj(area->vma_obj);
				sh_obj->mmo_un.mmo_bottom_obj->mmo_ops->ref(sh_obj->mmo_un.mmo_bottom_obj);
				sh_obj->mmo_shadowed = area->vma_obj;
				area->vma_obj = sh_obj;
			}*/
		}
	}
    
    pframe_t *temp = NULL;
    if (cause & FAULT_WRITE) {
		
       
        pframe_lookup(area->vma_obj,  ADDR_TO_PN(vaddr)  + (area->vma_off - area->vma_start), 1, &temp);
        
        if (temp == NULL) {
			dbg(DBG_PRINT, "(GRADING3D)\n");
            do_exit(EFAULT);
        }
        
        KASSERT(temp);
        KASSERT(temp->pf_addr);
		dbg(DBG_PRINT, "(GRADING3A 5.a)\n");
    
    
        uintptr_t paddr = pt_virt_to_phys((uintptr_t)temp->pf_addr);
        pt_map(curproc->p_pagedir, (uintptr_t)PAGE_ALIGN_DOWN(vaddr), (uintptr_t)PAGE_ALIGN_DOWN(paddr), PT_PRESENT|PT_USER|PD_WRITE, PT_PRESENT|PT_USER|PT_WRITE);
      
    }  else{
      
      dbg(DBG_PRINT, "(GRADING3D)\n");
        pframe_lookup(area->vma_obj, ADDR_TO_PN(vaddr)  + (area->vma_off - area->vma_start),0, &temp);
        if (temp == NULL) {
			dbg(DBG_PRINT, "(GRADING3D)\n");
            do_exit(EFAULT);
        }

        uintptr_t paddr = pt_virt_to_phys((uintptr_t)temp->pf_addr);
        pt_map(curproc->p_pagedir,(uintptr_t)PAGE_ALIGN_DOWN( vaddr),(uintptr_t)PAGE_ALIGN_DOWN(paddr), PT_PRESENT|PT_USER, PT_PRESENT|PT_USER);
     
     }
}
