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

#include "kernel.h"
#include "errno.h"
#include "globals.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "proc/proc.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "fs/vfs_syscall.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/mmobj.h"
#include "mm/tlb.h"

static slab_allocator_t *vmmap_allocator;
static slab_allocator_t *vmarea_allocator;

void
vmmap_init(void)
{
        vmmap_allocator = slab_allocator_create("vmmap", sizeof(vmmap_t));
        KASSERT(NULL != vmmap_allocator && "failed to create vmmap allocator!");
        vmarea_allocator = slab_allocator_create("vmarea", sizeof(vmarea_t));
        KASSERT(NULL != vmarea_allocator && "failed to create vmarea allocator!");
}

vmarea_t *
vmarea_alloc(void)
{
        vmarea_t *newvma = (vmarea_t *) slab_obj_alloc(vmarea_allocator);
        if (newvma) {
                newvma->vma_vmmap = NULL;
        }
        return newvma;
}

void
vmarea_free(vmarea_t *vma)
{
        KASSERT(NULL != vma);
        slab_obj_free(vmarea_allocator, vma);
}

/* a debugging routine: dumps the mappings of the given address space. */
size_t
vmmap_mapping_info(const void *vmmap, char *buf, size_t osize)
{
        KASSERT(0 < osize);
        KASSERT(NULL != buf);
        KASSERT(NULL != vmmap);

        vmmap_t *map = (vmmap_t *)vmmap;
        vmarea_t *vma;
        ssize_t size = (ssize_t)osize;

        int len = snprintf(buf, size, "%21s %5s %7s %8s %10s %12s\n",
                           "VADDR RANGE", "PROT", "FLAGS", "MMOBJ", "OFFSET",
                           "VFN RANGE");

        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
                size -= len;
                buf += len;
                if (0 >= size) {
                        goto end;
                }

                len = snprintf(buf, size,
                               "%#.8x-%#.8x  %c%c%c  %7s 0x%p %#.5x %#.5x-%#.5x\n",
                               vma->vma_start << PAGE_SHIFT,
                               vma->vma_end << PAGE_SHIFT,
                               (vma->vma_prot & PROT_READ ? 'r' : '-'),
                               (vma->vma_prot & PROT_WRITE ? 'w' : '-'),
                               (vma->vma_prot & PROT_EXEC ? 'x' : '-'),
                               (vma->vma_flags & MAP_SHARED ? " SHARED" : "PRIVATE"),
                               vma->vma_obj, vma->vma_off, vma->vma_start, vma->vma_end);
        } list_iterate_end();

end:
        if (size <= 0) {
                size = osize;
                buf[osize - 1] = '\0';
        }
        /*
        KASSERT(0 <= size);
        if (0 == size) {
                size++;
                buf--;
                buf[0] = '\0';
        }
        */
        return osize - size;
}

/* Create a new vmmap, which has no vmareas and does
 * not refer to a process. */
vmmap_t *
vmmap_create(void)
{
    
		dbg(DBG_PRINT, "(GRADING3D)\n");
		
        vmmap_t *temp; 
        
        temp = (vmmap_t*)slab_obj_alloc(vmmap_allocator);
        
        list_init(&(temp->vmm_list));
        
        temp->vmm_proc = NULL;
        
        return temp;
        /*
        NOT_YET_IMPLEMENTED("VM: vmmap_create");
        return NULL;*/
}

/* Removes all vmareas from the address space and frees the
 * vmmap struct. */
void
vmmap_destroy(vmmap_t *map)
{
		KASSERT(NULL != map);
		dbg(DBG_PRINT, "(GRADING3A 3.a)\n");
	
        vmarea_t * tmp;
        list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink){
            if(tmp!=NULL)
            {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                if(tmp->vma_obj!=NULL){
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    tmp->vma_obj->mmo_ops->put(tmp->vma_obj);
				}
                
                if(list_link_is_linked(&(tmp->vma_plink)))
                {
					dbg(DBG_PRINT, "(GRADING3D)\n");    
                    list_remove(&(tmp->vma_plink));
				}
                    
                if(list_link_is_linked(&(tmp->vma_olink))) 
				{
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    list_remove(&(tmp->vma_olink));    
                }
                    
                vmarea_free(tmp);
            }
        }list_iterate_end();
        slab_obj_free(vmmap_allocator,map);
        /*NOT_YET_IMPLEMENTED("VM: vmmap_destroy");*/
}

/* Add a vmarea to an address space. Assumes (i.e. asserts to some extent)
 * the vmarea is valid.  This involves finding where to put it in the list
 * of VM areas, and adding it. Don't forget to set the vma_vmmap for the
 * area. */
void
vmmap_insert(vmmap_t *map, vmarea_t *newvma)
{
	
		KASSERT(NULL != map && NULL != newvma);
		KASSERT(NULL == newvma->vma_vmmap);
		KASSERT(newvma->vma_start < newvma->vma_end);
		KASSERT(ADDR_TO_PN(USER_MEM_LOW) <= newvma->vma_start && ADDR_TO_PN(USER_MEM_HIGH) >= newvma->vma_end);
		dbg(DBG_PRINT, "(GRADING3A 3.b)\n");
	
        vmarea_t *temp = NULL;
        vmarea_t *tmp;
        
        
        
        list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink){
            if(tmp->vma_start >= newvma->vma_end) {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                temp = tmp;
                list_insert_before(&(temp->vma_plink), &(newvma->vma_plink));
                newvma->vma_vmmap = map;
                return;
            }
        }list_iterate_end();
        
            list_insert_tail(&(map->vmm_list),&(newvma->vma_plink));
        
        newvma->vma_vmmap = map;
       /* NOT_YET_IMPLEMENTED("VM: vmmap_insert");*/
}

/* Find a contiguous range of free virtual pages of length npages in
 * the given address space. Returns starting vfn for the range,
 * without altering the map. Returns -1 if no such range exists.
 *
 * Your algorithm should be first fit. If dir is VMMAP_DIR_HILO, you
 * should find a gap as high in the address space as possible; if dir
 * is VMMAP_DIR_LOHI, the gap should be as low as possible. */
int
vmmap_find_range(vmmap_t *map, uint32_t npages, int dir)
{
        vmarea_t *tmp;
        uint32_t temp_num = ADDR_TO_PN(USER_MEM_LOW);

        /*if (dir == VMMAP_DIR_LOHI) 
        {    
			dbg(DBG_PRINT, "(GRADING3C97\n");
            list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink)
            {
                if(tmp->vma_start - temp_num >= npages) 
                {
					dbg(DBG_PRINT, "(GRADING3C98\n");
                    return temp_num;
                }
                temp_num = tmp->vma_end;
            }list_iterate_end();
            
            if (ADDR_TO_PN(USER_MEM_HIGH) - temp_num >= npages) 
            {
				dbg(DBG_PRINT, "(GRADING3C99\n");
                return temp_num;
            }
        } */
        
        if(dir == VMMAP_DIR_HILO) {
            
            dbg(DBG_PRINT, "(GRADING3D)\n");
            uint32_t tmp_num = 0;
            
            list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink){
                if(tmp->vma_start - temp_num >= npages) {
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    tmp_num = tmp->vma_start - npages;
                }
                temp_num = tmp->vma_end;
            }list_iterate_end();
            if (ADDR_TO_PN(USER_MEM_HIGH) - temp_num >= npages) {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                tmp_num = ADDR_TO_PN(USER_MEM_HIGH) - npages;
            }
            if (tmp_num != 0) {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                return tmp_num;
            }
        } 
        /*NOT_YET_IMPLEMENTED("VM: vmmap_find_range");*/
        return -1;
}

/* Find the vm_area that vfn lies in. Simply scan the address space
 * looking for a vma whose range covers vfn. If the page is unmapped,
 * return NULL. */
vmarea_t *
vmmap_lookup(vmmap_t *map, uint32_t vfn)
{
		KASSERT(NULL != map);
		dbg(DBG_PRINT, "(GRADING3A 3.c)\n");
	
        vmarea_t *tmp;
        list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink){
            if(tmp->vma_start <= vfn&&tmp->vma_end>vfn) {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                return tmp;
            }
        }list_iterate_end();
        return NULL;
        /*
        NOT_YET_IMPLEMENTED("VM: vmmap_lookup");
        return NULL;
        */
}

/* Allocates a new vmmap containing a new vmarea for each area in the
 * given map. The areas should have no mmobjs set yet. Returns pointer
 * to the new vmmap on success, NULL on failure. This function is
 * called when implementing fork(2). */
vmmap_t *
vmmap_clone(vmmap_t *map)
{
        /*if(map == NULL) {
			dbg(DBG_PRINT, "(GRADING3C105\n");
            return NULL;
        }*/
        
        dbg(DBG_PRINT, "(GRADING3D)\n");
        vmmap_t *temp;
        temp = vmmap_create();
        temp->vmm_proc = map->vmm_proc;
        vmarea_t *tmp;
        
        list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink){
            if(tmp!=NULL) {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                vmarea_t *obj = vmarea_alloc();
                obj->vma_start = tmp->vma_start;
                obj->vma_end = tmp->vma_end;
                obj->vma_flags = tmp->vma_flags;
                obj->vma_obj = tmp->vma_obj;
                obj->vma_off = tmp->vma_off;
                obj->vma_prot = tmp->vma_prot;
                obj->vma_vmmap = temp; 
                list_init(&(obj->vma_olink));
                list_init(&(obj->vma_plink));
                list_insert_tail(&(temp->vmm_list),&(obj->vma_plink));
                tmp->vma_obj->mmo_ops->ref(tmp->vma_obj);
                if(tmp->vma_flags & MAP_PRIVATE){
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    mmobj_t *tmpsh = shadow_create();
                    tmpsh->mmo_shadowed = tmp->vma_obj;
                    tmpsh->mmo_un.mmo_bottom_obj = mmobj_bottom_obj(tmp->vma_obj);
                    obj->vma_obj = tmpsh;
                    tmpsh->mmo_un.mmo_bottom_obj->mmo_ops->ref(tmpsh->mmo_un.mmo_bottom_obj);
                    tmpsh->mmo_shadowed = tmp->vma_obj;
                    
                    mmobj_t *tmpsh2 = shadow_create();
                    tmpsh2->mmo_shadowed = tmp->vma_obj;
                    tmpsh2->mmo_un.mmo_bottom_obj = mmobj_bottom_obj(tmp->vma_obj);
                    
                    tmpsh2->mmo_un.mmo_bottom_obj->mmo_ops->ref(tmpsh2->mmo_un.mmo_bottom_obj);
                    tmpsh2->mmo_shadowed = tmp->vma_obj;
                    tmp->vma_obj = tmpsh2;
                }
                    
                
                
            }
        }list_iterate_end();
                
        return temp;
        
    /*    NOT_YET_IMPLEMENTED("VM: vmmap_clone");
        return NULL;*/
}

/* Insert a mapping into the map starting at lopage for npages pages.
 * If lopage is zero, we will find a range of virtual addresses in the
 * process that is big enough, by using vmmap_find_range with the same
 * dir argument.  If lopage is non-zero and the specified region
 * contains another mapping that mapping should be unmapped.
 *
 * If file is NULL an anon mmobj will be used to create a mapping
 * of 0's.  If file is non-null that vnode's file will be mapped in
 * for the given range.  Use the vnode's mmap operation to get the
 * mmobj for the file; do not assume it is file->vn_obj. Make sure all
 * of the area's fields except for vma_obj have been set before
 * calling mmap.
 *
 * If MAP_PRIVATE is specified set up a shadow object for the mmobj.
 *
 * All of the input to this function should be valid (KASSERT!).
 * See mmap(2) for for description of legal input.
 * Note that off should be page aligned.
 *
 * Be very careful about the order operations are performed in here. Some
 * operation are impossible to undo and should be saved until there
 * is no chance of failure.
 *
 * If 'new' is non-NULL a pointer to the new vmarea_t should be stored in it.
 */
int
vmmap_map(vmmap_t *map, vnode_t *file, uint32_t lopage, uint32_t npages,
          int prot, int flags, off_t off, int dir, vmarea_t **new)
{
	
		   KASSERT(NULL != map);
		   KASSERT(0 < npages);
           KASSERT((MAP_SHARED & flags) || (MAP_PRIVATE & flags));
           KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_LOW) <= lopage));
           KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_HIGH) >= (lopage + npages)));
           KASSERT(PAGE_ALIGNED(off));
           dbg(DBG_PRINT, "(GRADING3A 3.d)\n");
           
		
  vmarea_t *tmp = NULL;
    int startpn = -1;
    if(lopage==0){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        startpn = vmmap_find_range(map,npages,dir);
        if(startpn<0){
			dbg(DBG_PRINT, "(GRADING3D)\n");
            return startpn;
        }
        else{
			dbg(DBG_PRINT, "(GRADING3D)\n");
            tmp = vmarea_alloc();
            tmp->vma_start = startpn;
            tmp->vma_end = startpn + npages; 
            tmp->vma_prot = prot;
            tmp->vma_off = ADDR_TO_PN(off);
            tmp->vma_flags = flags;
            list_init(&(tmp->vma_plink));
            list_init(&(tmp->vma_olink));
            vmmap_insert(map, tmp);
        }
    }
    else{
		dbg(DBG_PRINT, "(GRADING3D)\n");
        if(!vmmap_is_range_empty(map, lopage, npages)){
			dbg(DBG_PRINT, "(GRADING3D)\n");
            vmmap_remove(map, lopage, npages);
        }
        tmp = vmarea_alloc();
        tmp->vma_start = lopage;
        tmp->vma_end = lopage + npages;
        tmp->vma_prot = prot;
        tmp->vma_off = ADDR_TO_PN(off);
        tmp->vma_flags = flags;
        list_init(&(tmp->vma_plink));
        list_init(&(tmp->vma_olink));
        vmmap_insert(map, tmp);
    }
    mmobj_t *tmpmm = NULL;
    if(file==NULL){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        tmpmm = anon_create();
        if(tmpmm!=NULL){
			dbg(DBG_PRINT, "(GRADING3D)\n");
            tmp->vma_obj = tmpmm;
            list_insert_tail(&tmpmm->mmo_un.mmo_vmas,&tmp->vma_olink);
        } 
        /*else{
			dbg(DBG_PRINT, "(GRADING3C116\n");
            vmarea_free(tmp);
            return -1;
        }*/
    }
    else{
        dbg(DBG_PRINT, "(GRADING3D)\n");
        int errno = file->vn_ops->mmap(file,tmp,&tmpmm);
        if(errno>=0){
			dbg(DBG_PRINT, "(GRADING3D)\n");
            tmp->vma_obj = tmpmm;
            list_insert_tail(&tmpmm->mmo_un.mmo_vmas,&tmp->vma_olink);
        }
        /*else{
			dbg(DBG_PRINT, "(GRADING3C119\n");
            vmarea_free(tmp);
            return -1;
        }*/
    }
   
   if(tmp->vma_flags & MAP_PRIVATE){
	   dbg(DBG_PRINT, "(GRADING3D)\n");
        mmobj_t *tmpsh = shadow_create();
             tmpsh->mmo_shadowed = tmpmm;
             tmpsh->mmo_un.mmo_bottom_obj = mmobj_bottom_obj(tmpmm);
             tmp->vma_obj = tmpsh;
             
             tmpsh->mmo_un.mmo_bottom_obj->mmo_ops->ref(tmpsh->mmo_un.mmo_bottom_obj);
    }
    
    if(new !=NULL){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        *new = tmp;
    }
    return 0;
       /* NOT_YET_IMPLEMENTED("VM: vmmap_map");
        return -1;*/
}

/*
 * We have no guarantee that the region of the address space being
 * unmapped will play nicely with our list of vmareas.
 *
 * You must iterate over each vmarea that is partially or wholly covered
 * by the address range [addr ... addr+len). The vm-area will fall into one
 * of four cases, as illustrated below:
 *
 * key:
 *          [             ]   Existing VM Area
 *        *******             Region to be unmapped
 *
 * Case 1:  [   ******    ]
 * The region to be unmapped lies completely inside the vmarea. We need to
 * split the old vmarea into two vmareas. be sure to increment the
 * reference count to the file associated with the vmarea.
 *
 * Case 2:  [      *******]**
 * The region overlaps the end of the vmarea. Just shorten the length of
 * the mapping.
 *
 * Case 3: *[*****        ]
 * The region overlaps the beginning of the vmarea. Move the beginning of
 * the mapping (remember to update vma_off), and shorten its length.
 *
 * Case 4: *[*************]**
 * The region completely contains the vmarea. Remove the vmarea from the
 * list.
 */
int
vmmap_remove(vmmap_t *map, uint32_t lopage, uint32_t npages)
{
	
		dbg(DBG_PRINT, "(GRADING3D)\n");

        vmarea_t *tmp;
        list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink)
        {
            
            if((tmp->vma_start < lopage) && (tmp->vma_end > lopage+npages))
            {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                vmarea_t *obj;
                obj = vmarea_alloc();    /*Case 1*/
                
                obj->vma_start = tmp->vma_start;
                obj->vma_off = tmp->vma_off;
                obj->vma_prot = tmp->vma_prot;
                obj->vma_flags = tmp->vma_flags;
                obj->vma_vmmap = tmp->vma_vmmap;
                obj->vma_obj = tmp->vma_obj;
                
                list_init(&(obj->vma_plink));
                list_init(&(obj->vma_olink));
                
                obj->vma_end = lopage;    
                tmp->vma_off += lopage+npages - tmp->vma_start;
                tmp->vma_start = lopage+npages;        
                 
                obj->vma_obj->mmo_ops->ref(obj->vma_obj);
                
                if(list_link_is_linked(&(tmp->vma_plink))){
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    list_insert_before(&(tmp->vma_plink),&(obj->vma_plink));     
				}
                
                if(list_link_is_linked(&(tmp->vma_olink))){
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    list_insert_before(&(tmp->vma_olink),&(obj->vma_olink));
                }
                
            }
            
            else if((tmp->vma_start < lopage) && (tmp->vma_end <= lopage+npages) && tmp->vma_end>lopage) 
            {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                tmp->vma_end = lopage;    /*Case 2*/
            }
            
            else if((tmp->vma_start >= lopage) && (tmp->vma_start < lopage+npages) && tmp->vma_end>lopage+npages)
            {                
				dbg(DBG_PRINT, "(GRADING3D)\n");
                tmp->vma_off += lopage+npages - tmp->vma_start;
                tmp->vma_start = lopage+npages;    /*Case 3*/
            }
            
            else if((tmp->vma_start >= lopage) && (tmp->vma_end <= lopage+npages))
            {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                
                tmp->vma_obj->mmo_ops->put(tmp->vma_obj);    
                if(list_link_is_linked(&(tmp->vma_plink))) {
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    list_remove(&(tmp->vma_plink));    /*Case 4*/
				}
                if(list_link_is_linked(&(tmp->vma_olink))){
					dbg(DBG_PRINT, "(GRADING3D)\n");
                    list_remove(&(tmp->vma_olink));
                    }
                vmarea_free(tmp);
            }
            
            
        }list_iterate_end();
 
		tlb_flush_all();
		pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(lopage),(uintptr_t)PN_TO_ADDR(lopage + npages));

 
        return 0;

/*        NOT_YET_IMPLEMENTED("VM: vmmap_remove");
        return -1;*/
}

/*
 * Returns 1 if the given address space has no mappings for the
 * given range, 0 otherwise.
 */
int
vmmap_is_range_empty(vmmap_t *map, uint32_t startvfn, uint32_t npages)
{
		uint32_t endvfn = startvfn+npages;
		KASSERT((startvfn < endvfn) && (ADDR_TO_PN(USER_MEM_LOW) <= startvfn) && (ADDR_TO_PN(USER_MEM_HIGH) >= endvfn));
		dbg(DBG_PRINT, "(GRADING3A 3.e)\n");
		
        vmarea_t *tmp;
        list_iterate_begin(&(map->vmm_list),tmp,vmarea_t,vma_plink){
            if((tmp->vma_start >= startvfn + npages) || (tmp->vma_end <= startvfn)) {
                dbg(DBG_PRINT, "(GRADING3D)\n");
            } else {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                return 0;
            }
        }list_iterate_end();
        
        /*NOT_YET_IMPLEMENTED("VM: vmmap_is_range_empty");*/
        return 1;
}

/* Read into 'buf' from the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do so, you will want to find the vmareas
 * to read from, then find the pframes within those vmareas corresponding
 * to the virtual addresses you want to read, and then read from the
 * physical memory that pframe points to. You should not check permissions
 * of the areas. Assume (KASSERT) that all the areas you are accessing exist.
 * Returns 0 on success, -errno on error.
 */
int
vmmap_read(vmmap_t *map, const void *vaddr, void *buf, size_t count)
{
        uint32_t tempFN = ADDR_TO_PN(vaddr);
        uint32_t nRead = 0;
        uint32_t tempPN = 0;
        uint32_t offset = PAGE_OFFSET(vaddr);
        
        pframe_t *tempFrame = NULL;
        
        dbg(DBG_PRINT, "(GRADING3D)\n");
        
        while (count > 0) {
            vmarea_t *tempArea = vmmap_lookup(map, tempFN);
            tempPN = tempFN + tempArea->vma_off - tempArea->vma_start;
            int lookup = tempArea->vma_obj->mmo_ops->lookuppage(tempArea->vma_obj, tempPN, 0, &tempFrame);
          /*  if (lookup < 0) {
				dbg(DBG_PRINT, "(GRADING3C133\n");
                return lookup;
            }*/
            uint32_t avail = PAGE_SIZE-offset;
            if (avail>=count) {
				dbg(DBG_PRINT, "(GRADING3D)\n");
                memcpy((void *)((uint32_t)buf+nRead), (void *)((uint32_t)tempFrame->pf_addr+offset), count);
                nRead += count;
                count = 0;
            }
            else{
				dbg(DBG_PRINT, "(GRADING3D)\n");
                memcpy((void *)((uint32_t)buf+nRead),(void *)((uint32_t)tempFrame->pf_addr+offset), avail);
                nRead += avail;
                count -= avail;
            }
            offset = 0;
            tempFN++;
            
        }
        return 0;
        /*
        NOT_YET_IMPLEMENTED("VM: vmmap_read");
        return 0;
        */
}

/* Write from 'buf' into the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do this, you will need to find the correct
 * vmareas to write into, then find the correct pframes within those vmareas,
 * and finally write into the physical addresses that those pframes correspond
 * to. You should not check permissions of the areas you use. Assume (KASSERT)
 * that all the areas you are accessing exist. Remember to dirty pages!
 * Returns 0 on success, -errno on error.
 */
int
vmmap_write(vmmap_t *map, void *vaddr, const void *buf, size_t count)
{
    uint32_t tempFN = ADDR_TO_PN(vaddr);
       uint32_t nWrite = 0;
       uint32_t tempPN = 0;
       uint32_t offset = PAGE_OFFSET(vaddr);
        
        pframe_t *tempFrame = NULL;
        dbg(DBG_PRINT, "(GRADING3D)\n");
        
        while (count > 0) {
            vmarea_t *tempArea = vmmap_lookup(map, tempFN);
            tempPN = tempFN + tempArea->vma_off - tempArea->vma_start;
            int lookup = tempArea->vma_obj->mmo_ops->lookuppage(tempArea->vma_obj, tempPN, 0, &tempFrame);
            /*if (lookup < 0) {
				dbg(DBG_PRINT, "(GRADING3C138\n");
                return lookup;
            }*/
            uint32_t avail = PAGE_SIZE-offset;
            if (avail>=count) {
   
				dbg(DBG_PRINT, "(GRADING3D)\n");
                memcpy((void *)((uint32_t)tempFrame->pf_addr+offset), (void *)((uint32_t)buf+nWrite), count);
                nWrite += count;
                count = 0;
            }
            else{
   
				dbg(DBG_PRINT, "(GRADING3D)\n");
                memcpy((void *)((uint32_t)tempFrame->pf_addr+offset), (void *)((uint32_t)buf+nWrite), avail);
                nWrite += avail;
                count -= avail;
            }
            offset = 0;
            tempFN++;
            
        }
        return 0;
        /*
        NOT_YET_IMPLEMENTED("VM: vmmap_write");
        return 0;
        */
}
