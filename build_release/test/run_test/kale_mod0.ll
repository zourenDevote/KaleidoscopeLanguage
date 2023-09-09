; ModuleID = 'module0'
source_filename = "module0"
target triple = "x86_64-pc-linux-gnu"

@private_str = private constant [19 x i8] c"Fibonacci(%d) = %d\00"

define i64 @fibonacci(i64 %0, ...) {
entry:
  %1 = icmp sle i64 %0, 2
  br i1 %1, label %if_body, label %if_after

if_body:                                          ; preds = %entry
  ret i64 1

if_after:                                         ; preds = %entry
  %2 = sub nsw i64 %0, 1
  %3 = call i64 (i64, ...) @fibonacci(i64 %2)
  %4 = sub nsw i64 %0, 2
  %5 = call i64 (i64, ...) @fibonacci(i64 %4)
  %6 = add nsw i64 %3, %5
  ret i64 %6
}

define i32 @main(...) {
entry:
  %a = alloca i64, align 8
  store i64 0, ptr %a, align 4
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 4
  store i64 10, ptr %a, align 4
  store i64 1, ptr %i, align 4
  br label %while_cond

while_cond:                                       ; preds = %while_body, %entry
  %0 = load i64, ptr %i, align 4
  %1 = load i64, ptr %a, align 4
  %2 = icmp sle i64 %0, %1
  br i1 %2, label %while_body, label %while_after

while_body:                                       ; preds = %while_cond
  %3 = load i64, ptr %i, align 4
  %4 = load i64, ptr %i, align 4
  %5 = call i64 (i64, ...) @fibonacci(i64 %4)
  call void (ptr, ...) @PrintLn(ptr @private_str, i64 %3, i64 %5)
  %6 = load i64, ptr %i, align 4
  %7 = add nsw i64 %6, 1
  store i64 %7, ptr %i, align 4
  br label %while_cond

while_after:                                      ; preds = %while_cond
  ret i32 0
}

declare void @PrintLn(ptr, ...)
