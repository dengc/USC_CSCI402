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

#include "globals.h"
#include "errno.h"
#include "types.h"

#include "mm/mm.h"
#include "mm/tlb.h"
#include "mm/mman.h"
#include "mm/page.h"

#include "proc/proc.h"

#include "util/string.h"
#include "util/debug.h"

#include "fs/vnode.h"
#include "fs/vfs.h"
#include "fs/file.h"

#include "vm/vmmap.h"
#include "vm/mmap.h"

/*
 * This function implements the mmap(2) syscall, but only
 * supports the MAP_SHARED, MAP_PRIVATE, MAP_FIXED, and
 * MAP_ANON flags.
 *
 * Add a mapping to the current process's address space.
 * You need to do some error checking; see the ERRORS section
 * of the manpage for the problems you should anticipate.
 * After error checking most of the work of this function is
 * done by vmmap_map(), but remember to clear the TLB.
 */

int
do_mmap(void *addr, size_t len, int prot, int flags,
        int fd, off_t off, void **ret)
	{
		if(!PAGE_ALIGNED(addr) || !PAGE_ALIGNED(off))
          {
			  dbg(DBG_PRINT, "(GRADING3D)\n");
            return -EINVAL;    
		  }

		if(!(flags & MAP_PRIVATE) && !(flags &MAP_SHARED)){
			dbg(DBG_PRINT, "(GRADING3D)\n");
			return -EINVAL;
		}
		
		/*if((flags & MAP_PRIVATE) && (flags & MAP_SHARED)){
			dbg(DBG_PRINT, "(GRADING3C55\n");
			return -EINVAL;
		}*/

		if((flags & MAP_FIXED) && (addr == NULL))	{
			dbg(DBG_PRINT, "(GRADING3D)\n");
			return -EINVAL;
		}
		
		vnode_t *vtemp = NULL;

		uint32_t lopgnum = ADDR_TO_PN(addr);
		uint32_t npages = 0;
		
		if(len%PAGE_SIZE==0){
			dbg(DBG_PRINT, "(GRADING3D)\n");
			npages = len/PAGE_SIZE;
		}
		else{
			dbg(DBG_PRINT, "(GRADING3D)\n");
			npages = len/PAGE_SIZE + 1;
		}
		
		if(!(flags&MAP_ANON))
		{

			dbg(DBG_PRINT, "(GRADING3D)\n");		
			if(fd<0 || fd>=NFILES){
				dbg(DBG_PRINT, "(GRADING3D)\n");
				return -EBADF;
			}
			file_t *temp;

			temp = fget(fd);
			
			if(temp==NULL){
				dbg(DBG_PRINT, "(GRADING3D)\n");
				return -EBADF;	
			}

		if((flags & MAP_SHARED)&&(prot & PROT_WRITE) && !(temp->f_mode & FMODE_WRITE)){
			dbg(DBG_PRINT, "(GRADING3D)\n");
            fput(temp);
            return -EACCES;
        }
        	
			vtemp = temp->f_vnode;			
			fput(temp);
		}
	
		len = (size_t)PAGE_ALIGN_UP(len);		/*changing len to multiple of pages*/
	
		if(len==0 || ((uint32_t)addr+(uint32_t)len>=USER_MEM_HIGH) || ((uint32_t)addr+(uint32_t)off>=USER_MEM_HIGH)) 
		{
			dbg(DBG_PRINT, "(GRADING3D)\n");
			return -EINVAL;
		}
			
		vmarea_t *temp_area;
		
		int errno = vmmap_map(curproc->p_vmmap, vtemp,lopgnum, npages,prot,flags,off,VMMAP_DIR_HILO, &temp_area);
		
		if(errno<0){
			dbg(DBG_PRINT, "(GRADING3D)\n");
			return errno;
		}
					
		tlb_flush_all();	

		*ret = PN_TO_ADDR(temp_area->vma_start);
		
		KASSERT(NULL != curproc->p_pagedir);
		dbg(DBG_PRINT, "(GRADING3A 2.a)\n");
		
		return 0;
		
		/*
        NOT_YET_IMPLEMENTED("VM: do_mmap");
        return -1;*/
}


/*
 * This function implements the munmap(2) syscall.
 *
 * As with do_mmap() it should perform the required error checking,
 * before calling upon vmmap_remove() to do most of the work.
 * Remember to clear the TLB.
 */
int
do_munmap(void *addr, size_t len)
{
	dbg(DBG_PRINT, "(GRADING3D)\n");
     
    if(!PAGE_ALIGNED(addr))
          {
			  dbg(DBG_PRINT, "(GRADING3D)\n");
            return -EINVAL;    
		  }

    
    if((uint32_t)len<=0 || ((uint32_t)addr>=USER_MEM_HIGH) || ((uint32_t)len>=USER_MEM_HIGH) || (uint32_t)addr<=0) 
		{
			dbg(DBG_PRINT, "(GRADING3D)\n");
			return -EINVAL;
		}
  
    int ret_code = vmmap_remove(curproc->p_vmmap, ADDR_TO_PN(addr), ((uint32_t)(PAGE_ALIGN_UP(len))/PAGE_SIZE));
    
    tlb_flush_all();
    
    return 0;
        /*NOT_YET_IMPLEMENTED("VM: do_munmap");
        return -1;*/

}

