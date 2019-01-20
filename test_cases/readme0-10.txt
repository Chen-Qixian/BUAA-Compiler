【测试集说明文档】
- test1 读操作测试
	- 对scanf覆盖测试
	- 先连续输入10个数字有回车，再连续输入8个字符无回车
	- 预期结果按顺序输出上述内容，前缀2_100T
	- 测试了读入到不同位置的情况（全局，局部，数组前、后）
	- 测试了读入类型（int,char)
	- 输出类型覆盖了字符串，常量，局部变量，全局变量（int,char全部情况）
	- 期望输出与读入顺序有关

- test2 赋值，表达式，写操作测试
	- 由于printf中含有表达式，故以赋值为载体对表达式也进行测试。
	- 用到赋值操作，但只检查简单的表达式。
	- 测试期望按照赋值顺序进行输出。
	- 测试覆盖了数组的使用情况检查。
	- 数组下标只检查了只有数字的情况。
	- 期望输出： 
	global_single_int =2  
	global_int_1 = 3  
	global_int_2 = 4  
	global_single_char =_  
	global_char_1 = +  
	global_char_2 = 9  
	local_single_int =100  
	local_int_1 = 101  
	local_int_2 = 102  
	local_single_char =-  
	local_char_1 = * 
	local_char_2 = /  
	a = 1  
	b = 2  
	c = 3  
	d[4] = 4  
	d[5] = 5  
	e[6] = 6  
	f = 7  
	g = 8  
	e[9] = 9  
	ch1 = a  
	ch2 = b  
	ch3 = c  
	ch4[1] = A  
	ch4[2] = B  
	ch5 = d  
	ch6[3] = C  
	ch6[4] = D  
	a1 = 11  
	b1 = 12  
	c1 = 13  
	d1[4] = 14  
	d1[5] = 15  
	e1[16] = 16  
	f1 = 17  
	g1 = 18  
	e1[19] = 19  
	ch7 = e  
	ch8 = f  
	ch9 = g  
	ch10[1] = E  
	ch10[2] = F  
	ch11 = h  
	ch12[13] = G  
	ch12[14] = H

- test3 表达式综合测试
	- 函数调用只检查了最简单的不传参的情况。
	- 期望输出为各个表达式的运算结果
	-  a = 10  
	b = -11  
	c = 10  
	d[0] = 11  
	d[1] = 11 
	d[2] = -1  
	d[3] = 13 
	f1 = 13  
	g1 = 22  
	f = 22  
	g = 22  
	e[0] = 3  
	e[1] = 13 
	e[2] = 6  
	e[3] = 22  
	e1[0] = 98  
	e1[1] = 130

- test4 条件语句测试
	- 覆盖条件的每种情况
	- 测试了循环语句（条件是同上的）
	- 测试了情况语句
	- 期望输出
	# 1  # 3  # 5  # 7  # 9  # 11  # 13  # 15 1 2 3 4 5 6 7 8 9 10 8 6 4 2 # 17  # 19  # 21  # 23 

- test5 函数综合测试
	- 覆盖函数返回值类型
	- 检查多个函数定义及其之间的调用关系
	- 检查递归调用
	- 期望输出
	 | print_num called: 1 x = 4 
	 | print_num called: 2 x = 5 
	 | print_num called: 3 x = 6 
	 | print_num called: 4 x = 7 
	 | print_num called: 5 x = 8 
	 | print_num called: 6 x = 9 
	 | print_num called: 7 x = 10 
	 | print_num called: 8 x = 1 
	 | print_num called: 9 x = 2 
	 | print_num called: 10 x = 3 
	 | print_num called: 11 x = 4 
	 | print_num called: 12 x = 26 
	 | print_num called: 13 x = 8 
	 | print_num called: 14 x = 21 
	 | print_num called: 15 x = 9
	 +
	 q
	 y
	 1
	 120

- test6 递归调用的应用-打印全排列
	- input: 3
	- output: 
				1
				2
				3
				1
				3
				2
				2
				1
				3
				2
				3
				1
				3
				1
				2
				3
				2
				1
- test7 白新宇测试
	- 期望结果
	**** user input : 10
	**** user input : 10
	**** user input : A
	65
	Value Assignment Succeed!  
	Arithmetic Check Succeed!  
	Division Check Passed!  
	Initial Array Test Passed!  
	48
	Beginning IO Test...  
	**** user input : 10
	**** user input : 1
	Number One is:  10
	  
	Number Two is:  49
	  
	The Result is:  59
	  
	Passed If Statement Testing!  
	Do Loop Passed!  
	Switch Logic Check Succeed!  
	Switch Parse Check Succeed!  
	**** user input : 20
	The Sequential Result for Input is:  210
	  
	Recursive Check Succeed!  
	Case Insensitive Passed!

- test 8 刘杰测试
	- 期望结果：
		第一组：
		输入： 0 1 2 3（输入一个数字马上会打印对应输出）
		输出：
		0
		test_recursion begin:
		success!
		*********************
		1
		test_if begin:
		success1!
		success2!
		*********************
		2
		test_while begin:
		success!
		*********************
		3
		test_switch begin:
		success!
		*********************

		第二组：
		输入： 4 yes 5 6 7 8（输入一个数字马上会打印对应输出）
		输出：
		4
		test_scanf_printf begin:
		yesInput is:
		y
		e
		s
		89
		69
		83
		*********************
		5
		test_expression begin:
		success!
		*********************
		6
		test_symbol begin:
		success!
		*********************
		7
		test_relation_operator begin:
		 x<y:true
		 x<=y:true
		 x!=y:true
		 x>y:true
		 x>=y:true
		 x!=y:true
		 x==y:true
		 x>=y:true
		 x<=y:true
		*********************
		8
		test_scope begin:
		the global scope_var = 20
		the local scope_var = 10
		the global scope_var = 20
		success!
		*********************

		第三组：
		输入： 0 1 2 3 4 yes 5 6 7 8（输入一个数字马上会打印对应输出）
		输出：
		前面第一，二组的输出再加上以下输出
		ch=_
		@#$$&*^^^^test end

- test9 任厚兴测试
	- 期望输出：
	input:
	0

	output:
	-1001
	-40
	10
	8
	261
	-190
	30
	11
	-1
	a

	input:
	1

	output:
	-233
	a
	1
	+
	0
	_
	233
	9
	-2
	b

	input:
	2

	output:
	8
	2
	257
	1
	r
	a
	a
	x
	-3
	c

	input:
	3 233 r

	output:
	 !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
	int_test_input = 233
	char_test_input = r
	0
	_

	input:
	4

	output:
	1
	2
	3
	4
	5
	6
	7
	8
	-5
	e

- test10 彭天豪测试
	- 期望输出
	======test1=======
	OUTPUT:
	2
	a
	120
	 !#$&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~
	1
	-4318
	***** input:
	12
	ax123
	*****
	all test:2668	
	======test2=======
	OUTPUT:
	2
	a
	120
	 !#$&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~
	1
	-4318
	***** input:
	0
	pq-1
	*****
	all test:2540
	======test3=======
	OUTPUT:
	2
	a
	120
	 !#$&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~
	1
	-4318
	***** input:
	-100
	_+3
	*****
	all test:2357