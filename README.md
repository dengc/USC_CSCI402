# USC_CSCI402
CSCI 402 Operating Systems
Documentation for Kernel Assignment 3
=====================================

+------------------------+
| BUILD & RUN (Required) |
+------------------------+

Comments: 

1. compile: 
	(a) "make clean"
	(b) "make"

2. run: 
	(a)"./weenix -n"

3. User Space shell is initiated.

4. Commands from grading guidelines can be used to test.

+-----------------------+
| USER SHELL (Required) |
+-----------------------+

Is the following statement correct about your submission (please replace
    "(Comments?)" with either "yes" or "no")?  "We run /sbin/init from the
    kernel init process in our submission": yes

+--------------------+
| GRADING (Required) |
+--------------------+

(A.1) In mm/pframe.c:
    (a) In pframe_get(): 2 out of 2 pt
    (b) In pframe_pin(): 2 out of 1 pt
    (c) In pframe_unpin(): 1 out of 1 pt

(A.2) In vm/mmap.c:
    (a) In do_mmap(): 2 out of 2 pts

(A.3) In vm/vmmap.c:
    (a) In vmmap_destroy(): 2 out of 2 pts
    (b) In vmmap_insert(): 4 out of 4 pts
    (c) In vmmap_lookup(): 2 out of 2 pt
    (d) In vmmap_map(): 3 out of 3 pts
    (e) In vmmap_is_range_empty(): 2 out of 2 pt

(A.4) In vm/anon.c:
    (a) In anon_init(): 1 out of 1 pt
    (b) In anon_ref(): 1 out of 1 pt
    (c) In anon_put(): 1 out of 1 pt
    (d) In anon_fillpage(): 1 out of 1 pt

(A.5) In vm/pagefault.c:
    (a) In handle_pagefault(): 2 out of 2 pts

(A.6) In vm/shadow.c:
    (a) In shadow_init(): 1 out of 1 pt
    (b) In shadow_ref(): 1 out of 1 pt
    (c) In shadow_put(): 1 out of 1 pts
    (d) In shadow_lookuppage(): 2 out of 2 pts
    (e) In shadow_fillpage(): 2 out of 2 pts

(A.7) In proc/fork.c:
    (a) In do_fork(): 6 out of 6 pts

(A.8) In proc/kthread.c:
    (a) In kthread_clone(): 2 out of 2 pts

(B.1) /usr/bin/hello (3 out of 3 pts)
(B.2) /usr/bin/args ab cde fghi j (2 out of 2 pts)
(B.3) /bin/uname -a (2 out of 2 pts)
(B.4) /bin/stat /README (1 out of 1 pt)
(B.5) /bin/stat /usr (1 out of 1 pt)
(B.6) /bin/ls /usr/bin (1 out of 1 pt)
(B.7) /usr/bin/fork-and-wait (5 out of 5 pts)

(C.1) help (1 out of 1 pt)
(C.2) echo hello (1 out of 1 pt)
(C.3) cat /README (1 out of 1 pt)
(C.4) /bin/ls (1 out of 1 pt)
(C.5) segfault (1 out of 1 pt)

(D.1) /usr/bin/vfstest (6 out of 6 pts)
(D.2) /usr/bin/memtest (6 out of 6 pts)
(D.3) /usr/bin/eatmem (6 out of 6 pts)
(D.4) /usr/bin/forkbomb (6 out of 6 pts)
(D.5) /usr/bin/stress (6 out of 6 pts)

(E.1) /usr/bin/vfstest (1 out of 1 pt)
(E.2) /usr/bin/memtest (1 out of 1 pt)
(E.3) /usr/bin/eatmem (1 out of 1 pt)
(E.4) /usr/bin/forkbomb (1 out of 1 pt)
(E.5) /usr/bin/stress (1 out of 1 pt)

(F) Self-checks: (10 out of 10 pts)
    Comments: none is needed

Missing required section(s) in README file (vm-README.txt): No
Submitted binary file : No
Submitted extra (unmodified) file : No
Wrong file location in submission : No
Not properly indentify which dbg() printout is for which item in the grading guidelines : No (Properly Identified)
Cannot compile : No (Compile Successfully)
Compiler warnings : No warnings
"make clean" : perfect clean
Kernel panic : never
Kernel hangs : no
Cannot halt kernel cleanly : it is halting cleanly

+---------------------------------+
| BUGS / TESTS TO SKIP (Required) |
+---------------------------------+

Is there are any tests in the standard test suite that you know that it's not
working and you don't want the grader to run it at all so you won't get extra
deductions, please list them here.  (Of course, if the grader won't run these
tests, you will not get plus points for them.)

Comments: No bug

+--------------------------------------+
| CONTRIBUTION FROM MEMBERS (Required) |
+--------------------------------------+

1)  Names and USC e-mail addresses of team members: 
	Liping She : 	lipingsh@usc.edu
	Akshay Kumar : 	kumaraks@usc.edu
	Mingyang Chai : chaiming@usc.edu
	Zihao Wang: 	zihaowan@usc.edu
	
2)  Is the following statement correct about your submission (please replace
        "(Comments?)" with either "yes" or "no", and if the answer is "no",
        please list percentages for each team member)?  "Each team member
        contributed equally in this assignment": Yes

+------------------+
| OTHER (Optional) |
+------------------+

Comments on deviation from spec (you will still lose points, but it's better to let the grader know): No
General comments on design decisions: No

We run the program perfectly under DYNAMIC = 1, please check that.



