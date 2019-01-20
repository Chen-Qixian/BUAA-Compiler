- test11 宋卓洋测试
	- 期望输出
	************************************************
	Start testing recursion:
	take
	1
	from
	a
	to
	b
	take
	2
	from
	a
	to
	c
	take
	1
	from
	b
	to
	c
	take
	3
	from
	a
	to
	b
	take
	1
	from
	c
	to
	a
	take
	2
	from
	c
	to
	b
	take
	1
	from
	a
	to
	b
	take
	4
	from
	a
	to
	c
	take
	1
	from
	b
	to
	c
	take
	2
	from
	b
	to
	a
	take
	1
	from
	c
	to
	a
	take
	3
	from
	b
	to
	c
	take
	1
	from
	a
	to
	b
	take
	2
	from
	a
	to
	c
	take
	1
	from
	b
	to
	c
	************************************************
	Start testing global:
	12345679
	0
	0
	-12345679
	9
	_
	+
	*
	12345679
	-12345679
	9
	*
	0
	1
	1
	2
	3
	1
	1
	2
	6
	2
	a
	A
	z
	Z
	_
	+
	-
	*
	/
	6
	************************************************
	Start testing return:
	22
	a
	b
	c
	_
	************************************************
	Start testing I/O:
	1
	a!@\n#$^&*()Qqaa123[];',./
	1
	a
	1
	a!@\n#$^&*()Qqaa123[];',./
	1
	a
	a1
	!@\n#$^&*()Qqaa123[];',./
	a
	1
	************************************************
	Start testing assign & exp:
	1
	40320
	-2
	1
	3
	a
	_
	a
	_
	903264
	************************************************
	Start testing if & while:
	+
	-
	+
	-
	+
	/
	*
	/
	*
	/
	************************************************
	Start testing switch:
	a
	1
	b
	3
	_
	************************************************
	Start testing parameter:
	INPUT of func_ret_int_1:
	12345679
	-12345679
	A
	Z
	OPERATE of func_ret_int_1:
	-12345679
	12345679
	Z
	A
	-12345679
	************************************************

- test 12 马力测试
	- 期望输出
	（1）输入1：
    c=(a+b)*b+a*b c=21 
    input c and d:
    （a）输入：3 4
        输出：3
    （b）输入：4 3
        输出：2
    （c）输入：4 4
        输出：<空>
     chA-chD ABCD+-*/
    c=(a+b)*b+a*b c=21
    input c and d:
    （a）输入：3 4
        输出：3
    （b）输入：4 3
        输出：2
    （c）输入：4 4
        输出：<空>
    21 F
	（2）输入2：
	    c=(a+b)*b+a*b c=16 
	    input c and d:
	    （a）输入：3 4
	        输出：2
	    （b）输入：4 3
	        输出：3
	    （c）输入：4 4
	        输出：<空>
	     cha-chad abcd+-*/
	    c=(a+b)*b+a*b c=16 
	    input c and d:
	    （a）输入：3 4
	        输出：3
	    （b）输入：4 3
	        输出：2
	    （c）输入：4 4
	        输出：<空>
	    16 F
	（3）输入3：
	    c=(a+b)*b+a*b c=12 
	    input c and d:
	    （a）输入：3 4
	        输出：2
	    （b）输入：4 3
	        输出：2
	    （c）输入：4 4
	        输出：<空>
	     cha-chad abcd+-*/
	    c=(a+b)*b+a*b c=12 
	    input c and d:
	    （a）输入：3 4
	        输出：3
	    （b）输入：4 3
	        输出：2
	    （c）输入：4 4
	        输出：<空>
	    12 F
	（4）输入4：
	    会死循环
	（5）输入5：
	    11 a
	（6）输入其他数字：
	    error！
	    0 a

- test 13 马力测试新
	输入：1
	输出： test_expression c=0 test_expression c=49 test_expression c=98
	输入：2
	输出： test_if output the less one output 2 test_while output a[0] a[1] a[2]:0  0  6 test_switch input one char:a or b 
	    输入：a  
	    输出：a 162

	    输入：b 
	    输出： b 164 

	输入：3
	输出： enter:
	    输入：ab 
	    输出： 65  66

	    输入：mg 
	    输出： 77  71

	输入：4
	输出： test_switch input one char:a or b     
	    输入：a 
	    输出： a 162  F

	     输入：b 
	    输出：  b 164  F

	输入：5
	输出：error input!@#$%%^&/|\{[('')]}~