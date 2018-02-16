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
#include "globals.h"
#include "types.h"
#include "errno.h"

#include "util/string.h"
#include "util/printf.h"
#include "util/debug.h"

#include "fs/dirent.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/vfs.h"
#include "fs/vnode.h"

/* This takes a base 'dir', a 'name', its 'len', and a result vnode.
 * Most of the work should be done by the vnode's implementation
 * specific lookup() function.
 *
 * If dir has no lookup(), return -ENOTDIR.
 *
 * Note: returns with the vnode refcount on *result incremented.
 */
int
lookup(vnode_t *dir, const char *name, size_t len, vnode_t **result)
{	
		
	if(len==0){
		dbg(DBG_PRINT,"(GRADING2B)\n");
		*result = dir;
		vref(dir);
		return 0;
	}

	int var;
	KASSERT(NULL != dir);
    KASSERT(NULL != name);
    KASSERT(NULL != result);
    dbg(DBG_PRINT,"(GRADING2A 2.a)\n");
	
		var = dir->vn_ops->lookup(dir,name,len,result);

		if (var >= 0 && (*result)->vn_mode !=S_IFDIR && name[len] == '/') {
			dbg(DBG_PRINT,"(GRADING2B)\n");
			vput(*result);
			return -ENOTDIR;
		}
		return var;

}


/* When successful this function returns data in the following "out"-arguments:
 *  o res_vnode: the vnode of the parent directory of "name"
 *  o name: the `basename' (the element of the pathname)
 *  o namelen: the length of the basename
 *
 * For example: dir_namev("/s5fs/bin/ls", &namelen, &name, NULL,
 * &res_vnode) would put 2 in namelen, "ls" in name, and a pointer to the
 * vnode corresponding to "/s5fs/bin" in res_vnode.
 *
 * The "base" argument defines where we start resolving the path from:
 * A base value of NULL means to use the process's current working directory,
 * curproc->p_cwd.  If pathname[0] == '/', ignore base and start with
 * vfs_root_vn.  dir_namev() should call lookup() to take care of resolving each
 * piece of the pathname.
 *
 * Note: A successful call to this causes vnode refcount on *res_vnode to
 * be incremented.
 */
int
dir_namev(const char *pathname, size_t *namelen, const char **name,
          vnode_t *base, vnode_t **res_vnode)
{
	
	KASSERT(NULL != pathname);
	KASSERT(NULL != namelen);
    KASSERT(NULL != name);
    KASSERT(NULL != res_vnode);
	dbg(DBG_PRINT,"(GRADING2A 2.b)\n");

	unsigned int index = 0;
	vnode_t *tmpnode = NULL;
	*namelen = 0;
	if(pathname[0]=='/'){
		dbg(DBG_PRINT,"(GRADING2B)\n");
		index++;
		tmpnode = vfs_root_vn;
		*res_vnode = vfs_root_vn;
	}
	else if(base==NULL){
		dbg(DBG_PRINT,"(GRADING2B)\n");
		tmpnode = curproc->p_cwd;
		*res_vnode = curproc->p_cwd;
	}

	vref(tmpnode);
	KASSERT(NULL != tmpnode);
	while(index<strlen(pathname)){
		int i = 0;

		while(pathname[index+i]!='/'&&pathname[index+i]!='\0'){
			i++;
		}
		if(i==0 && pathname[index+1] != '\0'){
			dbg(DBG_PRINT,"(GRADING2B)\n");
			index = index+i+1;
			continue;
		}
		if (i > NAME_LEN) {
			dbg(DBG_PRINT,"(GRADING2B)\n");
			vput(tmpnode);
			return -ENAMETOOLONG;
		}
		
		if(pathname[index+i]=='/' && pathname[index+i+1]!='\0'){
			dbg(DBG_PRINT,"(GRADING2B)\n");
			int var = lookup(tmpnode,&pathname[index],i,res_vnode);
			if(var<0){
				dbg(DBG_PRINT,"(GRADING2B)\n");
				vput(tmpnode);
				return var;
			}
			
			
			vput(tmpnode);
			tmpnode = *res_vnode;

			*namelen = i;
			*name = &pathname[index];
			index = index+i+1;

		}
		else if(pathname[index+i]=='\0' || pathname[index+i+1]=='\0' ){
			dbg(DBG_PRINT,"(GRADING2B)\n");
			*namelen = i;
			*name = &pathname[index];
			index = index+i+1;
			
		}
	}
       /* NOT_YET_IMPLEMENTED("VFS: dir_namev");*/
	        return 0;
}



/* This returns in res_vnode the vnode requested by the other parameters.
 * It makes use of dir_namev and lookup to find the specified vnode (if it
 * exists).  flag is right out of the parameters to open(2); see
 * <weenix/fcntl.h>.  If the O_CREAT flag is specified, and the file does
 * not exist call create() in the parent directory vnode.
 *
 * Note: Increments vnode refcount on *res_vnode.
 */
int
open_namev(const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base)		
{
	size_t namelen;		
	int h;
	dbg(DBG_PRINT,"(GRADING2B)\n");
	const char *name;			
	
	vnode_t *temp_res_vnode,*temp_res_vnode2;	
	
	int a = dir_namev(pathname,&namelen,&name,base,&temp_res_vnode);

	if(a<0) {
		dbg(DBG_PRINT,"(GRADING2B)\n");
		return a;
	}
	int b = lookup(temp_res_vnode,name,namelen,&temp_res_vnode2);
		
	if(flag == O_CREAT)
	{
		dbg(DBG_PRINT,"(GRADING2B)\n");
		if(b == -ENOENT){
			KASSERT(NULL != temp_res_vnode->vn_ops->create);	
			dbg(DBG_PRINT,"(GRADING2A 2.c)\n");
			h = temp_res_vnode->vn_ops->create(temp_res_vnode,name,namelen,res_vnode);
			vput(temp_res_vnode);
			return h;
		}
	
	}
	else 
	{
		dbg(DBG_PRINT,"(GRADING2B)\n");
		if(b==0){
			dbg(DBG_PRINT,"(GRADING2B)\n");
			*res_vnode = temp_res_vnode2;
			vput(temp_res_vnode);
			return b;
		}
		else{
			dbg(DBG_PRINT,"(GRADING2B)\n");
			vput(temp_res_vnode);
			return b;	
		}
	}

	vput(temp_res_vnode);	
	return 0;	

        /*NOT_YET_IMPLEMENTED("VFS: open_namev");
        return 0;*/

}

#ifdef __GETCWD__
/* Finds the name of 'entry' in the directory 'dir'. The name is writen
 * to the given buffer. On success 0 is returned. If 'dir' does not
 * contain 'entry' then -ENOENT is returned. If the given buffer cannot
 * hold the result then it is filled with as many characters as possible
 * and a null terminator, -ERANGE is returned.
 *
 * Files can be uniquely identified within a file system by their
 * inode numbers. */
int
lookup_name(vnode_t *dir, vnode_t *entry, char *buf, size_t size)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_name");
        return -ENOENT;
}


/* Used to find the absolute path of the directory 'dir'. Since
 * directories cannot have more than one link there is always
 * a unique solution. The path is writen to the given buffer.
 * On success 0 is returned. On error this function returns a
 * negative error code. See the man page for getcwd(3) for
 * possible errors. Even if an error code is returned the buffer
 * will be filled with a valid string which has some partial
 * information about the wanted path. */
ssize_t
lookup_dirpath(vnode_t *dir, char *buf, size_t osize)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_dirpath");

        return -ENOENT;
}
#endif /* __GETCWD__ */


