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

#include "util/debug.h"

#include "proc/kthread.h"
#include "proc/kmutex.h"

/*
 * IMPORTANT: Mutexes can _NEVER_ be locked or unlocked from an
 * interrupt context. Mutexes are _ONLY_ lock or unlocked from a
 * thread context.
 */

void
kmutex_init(kmutex_t *mtx)
{

	dbg(DBG_PRINT,"(GRADING1D)\n");	

	mtx->km_holder = NULL; 

	sched_queue_init(&(mtx->km_waitq));      

	/*NOT_YET_IMPLEMENTED("PROCS: kmutex_init");*/
}

/*
 * This should block the current thread (by sleeping on the mutex's
 * wait queue) if the mutex is already taken.
 *
 * No thread should ever try to lock a mutex it already has locked.
 */
void
kmutex_lock(kmutex_t *mtx)
{
	KASSERT(curthr&&(curthr != mtx ->km_holder));
	dbg(DBG_PRINT,"(GRADING1A 5.a)\n");

	if(mtx->km_holder == NULL){
		dbg(DBG_PRINT,"(GRADING1D)\n");
		mtx->km_holder = curthr;
	}else{
		dbg(DBG_PRINT,"(GRADING1D)\n");
		sched_sleep_on(&mtx->km_waitq);	
	}
}

/*
 * This should do the same as kmutex_lock, but use a cancellable sleep
 * instead. Also, if you are cancelled while holding mtx, you should unlock mtx.
 */
int
kmutex_lock_cancellable(kmutex_t *mtx)
{
	KASSERT(curthr && (curthr != mtx->km_holder)); /* please use TWO consecutive "conforming dbg() calls" for this */
	dbg(DBG_PRINT, "(GRADING1A 5.b)\n");    
	dbg(DBG_PRINT, "(GRADING1A 5.b)\n");    

	if(mtx->km_holder == NULL){
		dbg(DBG_PRINT,"(GRADING1C)\n");
		mtx->km_holder = curthr;
		return 0;
	}else{
		dbg(DBG_PRINT,"(GRADING1C)\n");
		return sched_cancellable_sleep_on(&mtx->km_waitq);		
	}
}

/*
 * If there are any threads waiting to take a lock on the mutex, one
 * should be woken up and given the lock.
 *
 * Note: This should _NOT_ be a blocking operation!
 *
 * Note: Ensure the new owner of the mutex enters the run queue.
 *
 * Note: Make sure that the thread on the head of the mutex's wait
 * queue becomes the new owner of the mutex.
 *
 * @param mtx the mutex to unlock
 */
void
kmutex_unlock(kmutex_t *mtx)
{
    	KASSERT(curthr && (curthr == mtx->km_holder));
		dbg(DBG_PRINT,"(GRADING1A 5.c)\n");

		mtx->km_holder = NULL;	
		if(!list_empty(&mtx->km_waitq.tq_list)) {
				dbg(DBG_PRINT,"(GRADING1D)\n");
				mtx->km_holder = sched_wakeup_on(&mtx->km_waitq);
		}

		KASSERT(curthr != mtx->km_holder);	
		dbg(DBG_PRINT,"(GRADING1A 5.c)\n");
}

