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


/*
 *  FILE: vfs_syscall.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Wed Apr  8 02:46:19 1998
 *  $Id: vfs_syscall.c,v 1.13 2015/12/15 14:38:24 william Exp $
 */

#include "kernel.h"
#include "errno.h"
#include "globals.h"
#include "fs/vfs.h"
#include "fs/file.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/fcntl.h"
#include "fs/lseek.h"
#include "mm/kmalloc.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/stat.h"
#include "util/debug.h"

/* To read a file:
 *      o fget(fd)
 *      o call its virtual read fs_op
 *      o update f_pos
 *      o fput() it
 *      o return the number of bytes read, or an error
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for reading.
 *      o EISDIR
 *        fd refers to a directory.
 *
 * In all cases, be sure you do not leak file refcounts by returning before
 * you fput() a file that you fget()'ed.
 */
int
do_read(int fd, void *buf, size_t nbytes)
{
    file_t *temp;
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if (fd < 0 || fd >= NFILES){
        dbg(DBG_PRINT,"(GRADING2B)\n");
            return -EBADF;
    }

    temp = fget(fd);

    if(temp == NULL)
        {         
            dbg(DBG_PRINT,"(GRADING2B)\n");
            return -EBADF;
        }

 
    if(!((*temp).f_mode & FMODE_READ)) 
        {
            dbg(DBG_PRINT,"(GRADING2B)\n");
            fput(temp);      
            return -EBADF;
        }

    if(S_ISDIR(temp->f_vnode->vn_mode))
    {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp);            
        return -EISDIR;
    }

    int count;

    
    count = temp->f_vnode->vn_ops->read((*temp).f_vnode,temp->f_pos,buf,nbytes);

    
    
    (*temp).f_pos += count;

    fput(temp);

    return count;

    /*NOT_YET_IMPLEMENTED("VFS: do_read");
        return -1;*/
}

/* Very similar to do_read.  Check f_mode to be sure the file is writable.  If
 * f_mode & FMODE_APPEND, do_lseek() to the end of the file, call the write
 * fs_op, and fput the file.  As always, be mindful of refcount leaks.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for writing.
 */

int
do_write(int fd, const void *buf, size_t nbytes)
{
    
    file_t *temp;
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if (fd < 0 || fd >= NFILES) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    temp = fget(fd);

    if(temp == NULL)
        {       
            dbg(DBG_PRINT,"(GRADING2B)\n");
            return -EBADF;
        }

    if(!(temp->f_mode & FMODE_WRITE))
        {
            dbg(DBG_PRINT,"(GRADING2B)\n");            
            fput(temp);            
            return -EBADF;
        }


    if((*temp).f_mode & FMODE_APPEND)
    {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        do_lseek(fd,0,SEEK_END);
    }

    unsigned int count;

    
    count = temp->f_vnode->vn_ops->write(temp->f_vnode, temp->f_pos, buf, nbytes);
    if (count == nbytes) {
        KASSERT((S_ISCHR(temp->f_vnode->vn_mode)) ||
                                         (S_ISBLK(temp->f_vnode->vn_mode)) ||
                                         ((S_ISREG(temp->f_vnode->vn_mode)) && (temp->f_pos <= temp->f_vnode->vn_len)));
        dbg(DBG_PRINT,"(GRADING2A 3.a)\n");
    }
    
    (*temp).f_pos += (off_t) count;
    
    fput(temp);

    return count;
        /*NOT_YET_IMPLEMENTED("VFS: do_write");
        return -1;*/
}

/*
 * Zero curproc->p_files[fd], and fput() the file. Return 0 on success
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't a valid open file descriptor.
 */

int
do_close(int fd)
{
    file_t *temp;
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if(fd<0 || fd>= NFILES) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    temp = fget(fd);

    if(temp == NULL) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }

    curproc->p_files[fd] = NULL;
    

    fput(temp);
    fput(temp);    

    return 0;
    
        /*NOT_YET_IMPLEMENTED("VFS: do_close");
        return -1;*/
}

/* To dup a file:
 *      o fget(fd) to up fd's refcount
 *      o get_empty_fd()
 *      o point the new fd to the same file_t* as the given fd
 *      o return the new file descriptor
 *
 * Don't fput() the fd unless something goes wrong.  Since we are creating
 * another reference to the file_t*, we want to up the refcount.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't an open file descriptor.
 *      o EMFILE
 *        The process already has the maximum number of file descriptors open
 *        and tried to open a new one.
 */
int
do_dup(int fd)
{
    dbg(DBG_PRINT,"(GRADING2B)\n");
    file_t *temp;
    int newDescriptor;

    if(fd<0 || fd>=NFILES) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    temp = fget(fd);

    if(temp == NULL) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    newDescriptor = get_empty_fd(curproc);

    if(newDescriptor < 0 || newDescriptor >= NFILES){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp);        
        return -EMFILE;
    }

    curproc->p_files[newDescriptor] = temp;
    return newDescriptor;

    /*NOT_YET_IMPLEMENTED("VFS: do_dup");
        return -1;*/
    
}

/* Same as do_dup, but insted of using get_empty_fd() to get the new fd,
 * they give it to us in 'nfd'.  If nfd is in use (and not the same as ofd)
 * do_close() it first.  Then return the new file descriptor.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        ofd isn't an open file descriptor, or nfd is out of the allowed
 *        range for file descriptors.
 */
int
do_dup2(int ofd, int nfd)
{
    file_t *temp,*temp2;
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if(ofd<0 || ofd>=NFILES) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    temp = fget(ofd);

    if(temp == NULL) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    if(ofd == nfd){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp);
        return nfd;
    }

    if(nfd<0 || nfd >= NFILES)
    {        
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp);
        return -EBADF;
    }

    temp2 = fget(nfd);

    if(temp2 != NULL){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp2);
        do_close(nfd);
    }

    curproc->p_files[nfd] = temp;
    
    return nfd;

        
    /*NOT_YET_IMPLEMENTED("VFS: do_dup2");
        return -1;*/
}

/*
 * This routine creates a special file of the type specified by 'mode' at
 * the location specified by 'path'. 'mode' should be one of S_IFCHR or
 * S_IFBLK (you might note that mknod(2) normally allows one to create
 * regular files as well-- for simplicity this is not the case in Weenix).
 * 'devid', as you might expect, is the device identifier of the device
 * that the new special file should represent.
 *
 * You might use a combination of dir_namev, lookup, and the fs-specific
 * mknod (that is, the containing directory's 'mknod' vnode operation).
 * Return the result of the fs-specific mknod, or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        mode requested creation of something other than a device special
 *        file.
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mknod(const char *path, int mode, unsigned devid)
{
    if(strlen(path) > MAXPATHLEN){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -ENAMETOOLONG;
    }
    
    if(mode!=S_IFCHR && mode !=S_IFBLK){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EINVAL;
    }

    vnode_t *res_node = NULL;
    vnode_t *tmp_node = NULL;
    size_t namelen = 0;
    char * name = NULL;
    vnode_t *base = NULL;
    int tmp = dir_namev(path, &namelen,(const char**)&name, base, &res_node);
    if(tmp<0){/*ENOENT and ENOTDIR is returned by  dir_namev*/
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return tmp;
    }    
    tmp = lookup(res_node,name,namelen,&tmp_node);
    if(tmp==0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(tmp_node);
        vput(res_node);
        return -EEXIST;
    }
    KASSERT(NULL != res_node->vn_ops->mknod);
    dbg(DBG_PRINT,"(GRADING2A 3.b)\n");
    int res = res_node->vn_ops->mknod(res_node,name,namelen,mode,devid);
    vput(res_node);
    
    return res;
    /*NOT_YET_IMPLEMENTED("VFS: do_mknod");
        return -1;*/
}
/* Use dir_namev() to find the vnode of the dir we want to make the new
 * directory in.  Then use lookup() to make sure it doesn't already exist.
 * Finally call the dir's mkdir vn_ops. Return what it returns.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mkdir(const char *path)
{
    if(strlen(path)>MAXPATHLEN){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -ENAMETOOLONG;
    }
    vnode_t *res_node = NULL;
    vnode_t *tmp_node = NULL;
    size_t namelen = 0;
    char * name = NULL;
    vnode_t *base = NULL;
    
    int tmp = dir_namev(path,&namelen,(const char**)&name,base,&res_node);

    if(tmp<0){ /*ENOENT and ENOTDIR is returned by dir_namev*/
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return tmp; 
    }

    tmp = lookup(res_node,name,namelen,&tmp_node);
    if(tmp==0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(tmp_node);
        vput(res_node);
        return -EEXIST; 
    }

    KASSERT(NULL != res_node->vn_ops->mkdir);
    dbg(DBG_PRINT,"(GRADING2A 3.c)\n");
    int res = res_node->vn_ops->mkdir(res_node,name,namelen);
    
    vput(res_node);
    
    return res;
        /*NOT_YET_IMPLEMENTED("VFS: do_mkdir"); */

}

/* Use dir_namev() to find the vnode of the directory containing the dir to be
 * removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 * return an error if the dir to be removed does not exist or is not empty, so
 * you don't need to worry about that here. Return the value of the v_op,
 * or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        path has "." as its final component.
 *      o ENOTEMPTY
 *        path has ".." as its final component.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_rmdir(const char *path)
{
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if(strlen(path)>MAXPATHLEN){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -ENAMETOOLONG;
    }
    vnode_t *res_node = NULL;
    vnode_t * tmp_node = NULL;
    size_t namelen = 0;
    char * name = NULL;
    vnode_t *base = NULL;
    
    int tmp = dir_namev(path,&namelen,(const char**)&name,base,&res_node);
    if(tmp<0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return tmp;
    }
    if(strcmp(name,".")==0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(res_node);
        return -EINVAL;
    }
    if(strcmp(name,"..")==0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(res_node);
        return -ENOTEMPTY;
    }

    int errno = lookup(res_node,name,namelen,&tmp_node);
    
    if(errno!=0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(res_node);
        return errno;
    }
    else
    {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        if(!S_ISDIR(tmp_node->vn_mode)){
            dbg(DBG_PRINT,"(GRADING2B)\n");
            vput(tmp_node);            
            vput(res_node);
                
            return -ENOTDIR; 
        }
        else{
            
            KASSERT(NULL != res_node->vn_ops->rmdir);
            dbg(DBG_PRINT,"(GRADING2A 3.d)\n");
            dbg(DBG_PRINT,"(GRADING2A 3.d)\n");
            int res = res_node->vn_ops->rmdir(res_node,name,namelen);
            vput(tmp_node);        
            vput(res_node);                    
            return res;
        }
    }
    /*   NOT_YET_IMPLEMENTED("VFS: do_rmdir");*/
        return 0;
}

/*
 * Same as do_rmdir, but for files.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EISDIR
 *        path refers to a directory.
 *      o ENOENT
 *        A component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_unlink(const char *path)
{
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if(strlen(path)>MAXPATHLEN){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return ENAMETOOLONG;
    }
    vnode_t *res_node = NULL;
    vnode_t * tmp_node = NULL;
    size_t namelen = 0;
    char * name = NULL;
    vnode_t *base = NULL;
    int tmp = dir_namev(path,&namelen,(const char**)&name,base,&res_node);
    if(tmp<0){ 
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return tmp;
    }

    int errno = lookup(res_node,name,namelen,&tmp_node);
    
    if(errno!=0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(res_node);
        return errno;
    }
    else{
        dbg(DBG_PRINT,"(GRADING2B)\n");
        if(S_ISDIR(tmp_node->vn_mode)){
            dbg(DBG_PRINT,"(GRADING2B)\n");
            vput(tmp_node);            
            vput(res_node);
            return -EISDIR; 
        }
        else{
            KASSERT(NULL != res_node->vn_ops->unlink);
            dbg(DBG_PRINT,"(GRADING2A 3.e)\n");
            dbg(DBG_PRINT,"(GRADING2A 3.e)\n");
            int res = res_node->vn_ops->unlink(res_node,name,namelen);
            vput(tmp_node);        
            vput(res_node);                    
            return res;
            }
        }
}

/* To link:
 *      o open_namev(from)
 *      o dir_namev(to)
 *      o call the destination dir's (to) link vn_ops.
 *      o return the result of link, or an error
 *
 * Remember to vput the vnodes returned from open_namev and dir_namev.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        to already exists.
 *      o ENOENT
 *        A directory component in from or to does not exist.
 *      o ENOTDIR
 *        A component used as a directory in from or to is not, in fact, a
 *        directory.
 *      o ENAMETOOLONG
 *        A component of from or to was too long.
 *      o EISDIR
 *        from is a directory.
 */
int
do_link(const char *from, const char *to)
{
    dbg(DBG_PRINT, "(GRADING3D)\n");
    /*if(strlen(from)>MAXPATHLEN||strlen(to)>MAXPATHLEN){
		dbg(DBG_PRINT, "(GRADING3C2\n");
        return -ENAMETOOLONG;
    }*/
    vnode_t *res_node = NULL;
    vnode_t *res_node2 = NULL;
    vnode_t * tmp_node = NULL;
    size_t namelen = 0;
    char * name = NULL;
    vnode_t *base = NULL;    
    int flag = 0;  
    
    int errno = open_namev(from,flag,&res_node,base); 
    
    /*if(errno<0){
		dbg(DBG_PRINT, "(GRADING3C3\n");
        return errno; 
    }*/

    if(S_ISDIR(res_node->vn_mode)){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        vput(res_node);
        return -EISDIR;
    }
    
    errno = dir_namev(to,&namelen,(const char**)&name,base,&res_node2);
    
    if(errno<0){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        vput(res_node);
        return errno; 
    }

    errno = lookup(res_node2,name,namelen,&tmp_node);

    if(errno==0){
		dbg(DBG_PRINT, "(GRADING3D)\n");
        vput(res_node);
        vput(res_node2);
        vput(tmp_node);    
        return -EEXIST;
    }
    else{    
		dbg(DBG_PRINT, "(GRADING3D)\n");    
        int res = res_node2->vn_ops->link(res_node,res_node2,name,namelen);
        vput(res_node);
        vput(res_node2);
        return res;    
    }


/*        NOT_YET_IMPLEMENTED("VFS: do_link");*/
        /*return -1;*/

}
/*      o link newname to oldname
 *      o unlink oldname
 *      o return the value of unlink, or an error
 *
 * Note that this does not provide the same behavior as the
 * Linux system call (if unlink fails then two links to the
 * file could exist).
 */
int
do_rename(const char *oldname, const char *newname)
{
	/*dbg(DBG_PRINT, "(GRADING3C8\n");

    int a = do_link(newname,oldname);

    if(a<0){
		dbg(DBG_PRINT, "(GRADING3C9\n");
        return a;
	}

    return do_unlink(oldname);
	
       NOT_YET_IMPLEMENTED("VFS: do_rename");*/
       return -1;
}

/* Make the named directory the current process's cwd (current working
 * directory).  Don't forget to down the refcount to the old cwd (vput()) and
 * up the refcount to the new cwd (open_namev() or vget()). Return 0 on
 * success.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        path does not exist.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o ENOTDIR
 *        A component of path is not a directory.
 */
int
do_chdir(const char *path)
{
    dbg(DBG_PRINT,"(GRADING2B)\n");
    if(strlen(path)>MAXPATHLEN){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -ENAMETOOLONG;
    }
    /*S_ISDIR(m)*/

    vnode_t* base = NULL;
    
    vnode_t *temp;
    temp = curproc->p_cwd;    

    vnode_t * res_node = NULL;
    int errno = open_namev(path,0,&res_node,base);
    
    if(errno<0){
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return errno;
    }

    if(!S_ISDIR(res_node->vn_mode))
    {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(res_node);
        return -ENOTDIR;
    }
    curproc->p_cwd = res_node;

    if(temp != NULL) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        vput(temp);
    }
    return 0;

    /* Handling Error Cases 

        NOT_YET_IMPLEMENTED("VFS: do_chdir");
        return -1;*/
}

/* Call the readdir fs_op on the given fd, filling in the given dirent_t*.
 * If the readdir fs_op is successful, it will return a positive value which
 * is the number of bytes copied to the dirent_t.  You need to increment the
 * file_t's f_pos by this amount.  As always, be aware of refcounts, check
 * the return value of the fget and the virtual function, and be sure the
 * virtual function exists (is not null) before calling it.
 *
 * Return either 0 or sizeof(dirent_t), or -errno.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        Invalid file descriptor fd.
 *      o ENOTDIR
 *        File descriptor does not refer to a directory.
 */

int
do_getdent(int fd, struct dirent *dirp)
{
    dbg(DBG_PRINT,"(GRADING2B)\n");            
    file_t *temp;                            /* from file.h */
    
    if (fd < 0 || fd >= NFILES) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
            return -EBADF;
    }
    temp = fget(fd);

    if(temp == NULL)
        {            
            dbg(DBG_PRINT,"(GRADING2B)\n");
                return -EBADF;
        }


    if(!(S_ISDIR(temp->f_vnode->vn_mode)))
        {
            dbg(DBG_PRINT,"(GRADING2B)\n");
            fput(temp);            
            return -ENOTDIR;
        }

    int a = 0;

    
    if(temp->f_vnode->vn_ops->readdir) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        a = temp->f_vnode->vn_ops->readdir(temp->f_vnode,temp->f_pos, dirp);            /* from vnode.h */
    }else {
        dbg(DBG_PRINT,"(GRADING2B)\n");
            fput(temp);
            return a;
        }

    if(a>0)
    {        
        dbg(DBG_PRINT,"(GRADING2B)\n");
        temp->f_pos+=a;
        fput(temp);        
            
        return sizeof(*dirp);
            
    }

    

    else
    {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp);            
        return a;
    }
/*    NOT_YET_IMPLEMENTED("VFS: do_getdent");
    return -1;*/
}

/*
 * Modify f_pos according to offset and whence.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not an open file descriptor.
 *      o EINVAL
 *        whence is not one of SEEK_SET, SEEK_CUR, SEEK_END; or the resulting
 *        file offset would be negative.
 */

int
do_lseek(int fd, int offset, int whence)
{
    file_t *temp;
    int b;
    dbg(DBG_PRINT,"(GRADING2B)\n");
    
    if (fd < 0 || fd >= NFILES) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
            return -EBADF;
    }
    temp = fget(fd);

    if(temp==NULL) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        return -EBADF;
    }
    int pos = temp->f_pos;

    if(whence == SEEK_SET) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        temp->f_pos = (off_t) offset + 0;

    } else if(whence == SEEK_CUR) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        temp->f_pos = (off_t) offset + temp->f_pos;

    } else if(whence == SEEK_END) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        temp->f_pos = (off_t) offset + temp->f_vnode->vn_len;

    } else {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        fput(temp);
        return -EINVAL;
    }
    
    if (temp->f_pos >= 0) {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        b = temp->f_pos;
        fput(temp);
        return b;
    } else {
        dbg(DBG_PRINT,"(GRADING2B)\n");
        temp->f_pos = pos;
        fput(temp);
        return -EINVAL;
    }
        /*NOT_YET_IMPLEMENTED("VFS: do_lseek");
        return -1;*/
}

/*
 * Find the vnode associated with the path, and call the stat() vnode operation.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        A component of path does not exist.
 *      o ENOTDIR
 *        A component of the path prefix of path is not a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_stat(const char *path, struct stat *buf)
{
    int val = 0;
        size_t length = 0;
        char *name = NULL;
        vnode_t *parent;
        vnode_t *child;

        dbg(DBG_PRINT,"(GRADING2B)\n");
        /*NOT_YET_IMPLEMENTED("VFS: do_stat");
        return -1;*/
        if (strlen(path) > MAXPATHLEN) {
            dbg(DBG_PRINT,"(GRADING2B)\n");
                return -ENAMETOOLONG;
        }

        if (strlen(path) == 0) {
            dbg(DBG_PRINT,"(GRADING2B)\n");
                return -EINVAL;
        }



        val = dir_namev(path, &length,(const char**) &name, NULL, &parent);
    

        if (val < 0) {
            dbg(DBG_PRINT,"(GRADING2B)\n");
                return val;
        } else {
            dbg(DBG_PRINT,"(GRADING2B)\n");
                val = lookup(parent,(const char*) name, length, &child);
           
                if (val < 0) {
                    dbg(DBG_PRINT,"(GRADING2B)\n");

                        vput(parent);
                        return val;
                } else {
                    KASSERT(NULL != child->vn_ops->stat);
                    dbg(DBG_PRINT,"(GRADING2A 3.f)\n");
                    dbg(DBG_PRINT,"(GRADING2A 3.f)\n");
                    child->vn_ops->stat(child, buf);
                    vput(child);
                }
        }
        vput(parent);
        return val;
}

#ifdef __MOUNTING__
/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutely sure your Weenix is perfect.
 *
 * This is the syscall entry point into vfs for mounting. You will need to
 * create the fs_t struct and populate its fs_dev and fs_type fields before
 * calling vfs's mountfunc(). mountfunc() will use the fields you populated
 * in order to determine which underlying filesystem's mount function should
 * be run, then it will finish setting up the fs_t struct. At this point you
 * have a fully functioning file system, however it is not mounted on the
 * virtual file system, you will need to call vfs_mount to do this.
 *
 * There are lots of things which can go wrong here. Make sure you have good
 * error handling. Remember the fs_dev and fs_type buffers have limited size
 * so you should not write arbitrary length strings to them.
 */
int
do_mount(const char *source, const char *target, const char *type)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_mount");
        return -EINVAL;
}

/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutley sure your Weenix is perfect.
 *
 * This function delegates all of the real work to vfs_umount. You should not worry
 * about freeing the fs_t struct here, that is done in vfs_umount. All this function
 * does is figure out which file system to pass to vfs_umount and do good error
 * checking.
 */
int
do_umount(const char *target)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_umount");
        return -EINVAL;
}
#endif
