# Kaleidoscope 语言

`kaleidoscope`语言是`llvm cookbook`定义的一个简单的语言，基于这个简单的语言基础上，我们进行了一些扩展，其文法如下

## 文法定义

**关键字(终结符)**

> EXTERN, DEF, DOUBLE, INT, BOOL, VOID, IF, THEN, ELSE, FOR, WHILE, RETURN, ID, DOUBLENUMBER, INTNUMBER, TRUE, FALSE, EOF(特殊字符)

**运算符**

> +，-， *，/，>, <, >=, <=, ==, !=, !

**文法**

> **program** ::= (**vardef** | **funcdef** | **externdef**)* **EOF**
>
> **vardef** ::= ( **INT** | **DOUBLE** | **BOOL** ) **ID** ('[' **expr** ']')* ';'
>
> **funcdef** ::= **DEF** **ID** '(' **paramlist** ')' (':' (**INT** | **DOUBLE** | **VOID** | **BOOL**))? '{' **statements** '}'
>
> **externdef** ::= **EXTERN** (**vardef** | **funcdecl** )
>
> **funcdecl** ::= **ID** '(' **paramlist** ')' ( ':' (**INT** | **DOUBLE** | **VOID** | **BOOL** ))? ';'
>
> **paramlist** ::= ( **param** ( ',' **param** )* )?
>
> **param** ::= (**INT** | **DOUBLE**) **ID**
>
> **statements** ::= ( **statement** )*
>
> **statement** ::= **vardef**
>
> ​					 ::= **ifstmt** 
>
> ​					 ::= **forstmt** 
>
> ​					 ::= **whilestmt** 
>
> ​					 ::= **exprstmt** 
>
> ​					 ::= **assignstmt** 
>
> ​                  ::= **blockstmt**
>
> ​				  ::= **returnstmt**
>
> **returnstmt** ::= **RETURN** (**expr**)?
>
> **blockstmt** ::= '{' **statements** '}'
>
> **ifstmt** ::= **IF** **expr** **THEN**  **statement** (**ELSE** **statement** ) ?
>
> **forstmt** ::= **FOR** (**iddef** '=' **expr**)? ',' （**expr**）? ','  (**expr**)? **IN** '{' **statements** '}'
>
> **whilestmt** ::= **WHILE** **expr** **IN** '{' **statements** '}' 
>
> **exprstmt** ::= **expr** ';'
>
> **assignstmt** ::= **idref** '=' **expr** ';'
>
> **expr** ::= **logicexpr**
>
> **logicexpr** ::= **binexpr1** (('>' | '<' | '>=' | '<=' | '==' | '!=') **binexpr1**)?
>
>**binexpr1** ::= **binexpr2** (('+' | '-') **binexpr2**)?
> 
>**binexpr2** ::= **unaryexpr** (('/'|'*') **unaryexpr**)?
> 
>**unaryexpr** ::= ('-'|'!')* **primaryexpr**
> 
>**primaryexpr** ::= '(' **expr** ')'
> 
>​						 ::= **idref**
> 
>​						 ::= **callexpr**
> 
>​						 ::= **number**
> 
>**idref** ::= **ID** ('[' **expr** ']')*
> 
>**callexpr** ::= **ID** '(' (**expr** (',' **expr**)* )? ')'
> 
>**ID** ::= \[a-zA-Z\]\[0-9_a-zA-Z\]*
> 
>**number** ::= **DOUBLENUMBER**
> 
>​				::= **INTNUMBER**
> 
>​				::= **TRUE**
> 
>​				::= **FALSE**
> 
>**doublenumber** ::= [0-9]+ '.' [0-9]+
> 
>**intnumber** ::= ('0'|\[1-9\]\[0-9\]*)					

注意：

> 在上面的文法描述中，非TOKEN字符用`''`框起来，TOKEN用加粗大写表示，非终结符用加粗小写表示.
>
> 语法糖
>
> **()** : 表示子表达式，表示一个匹配单元
>
> **[]**: 表示范围
>
> ***** : 表示匹配零次或多次
>
> **？**: 表示匹配零次或一次
>
> **+**: 表示匹配一次或多次
>
> **|**: 'a'| 'b' 表示匹配字符a或者字符b

## 测试用例

int a;

a = -10;

a = 0 - 10;

## 项目介绍

```bash
kaleidoscope
	| - CMakeLists.txt 	--> 控制编译的cmake脚本文件
	| - build 			--> 构建目录
	| - include			--> 头文件目录
    	| - token.h		--> token的定义
    	| - parser.h	--> 解析器的定义
    	| - ir_gen.h    --> IR生成的定义
    	| - ast.h	 	--> ast的定义
	| - src				--> 源文件目录
		| - parser.cpp  --> 解析器的实现
		| - ir_gen.cpp	--> IR生成的实现
        | - ast.cpp	    --> ast的实现
        | - main.cpp	--> 主函数
    | - test			--> 测试用例文件夹
    	| ...
```

## 项目编译命令

目前因为项目借助的llvm12，所以在编译的时候

```bash
sudo apt-get install llvm-12
sudo apt-get install clang-12
# 确保有llvm-12 和 clang-12
# 怎么看?
clang-12 --version
llvm-12 --version
```



```bash
$> rm -r build
$> mkdir build | cd build
# 如果你要调试
$> cmake -DCMAKE_BUILD_TYPE=Debug ../
# 如果不调试的话
$> cmake ../
# 构建命令
$> make -j 16
```



## 怎么用

```bash
./Kaleidoscope test.k -o xxx.ll

clang-12 test.ll kaleidoscope_std.c -o test

./test
```

