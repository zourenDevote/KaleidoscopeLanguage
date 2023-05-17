
#ifndef KALEIDOSCOPE_TOKEN
#define KALEIDOSCOPE_TOKEN

enum Token {
  tok_eof = 1,             // --> 终结符 EOF

  // commands
  tok_def = 2,             // --> 终结符 def
  tok_extern = 3,          // --> 终结符 extern

  // primary
  tok_id = 4,              // --> 终结符 identifier
  tok_double_number = 5,   // --> 终结符 浮点数字常量
  tok_int_number = 6,      // --> 终结符 整型数字常量

  // control
  tok_if = 7,              // --> 终结符 if
  tok_then = 8,            // --> 终结符 then
  tok_else = 9,            // --> 终结符 else
  tok_for = 10,            // --> 终结符 for
  tok_while = 11,          // --> 终结符 while
  tok_return = 12,         // --> 终结符 return
  tok_in = 13,             // --> 终结符 in

  // type
  tok_double = 14,         // --> 终结符 double
  tok_int = 15,            // --> 终结符 int
  tok_bool = 16,           // --> 终结符 bool
  tok_true = 17,
  tok_false = 18,

  // operator
  tok_plus = 19,                 // --> +
  tok_sub = 20,                  // --> -
  tok_mul = 21,                  // --> *
  tok_div = 22,                  // --> /
  tok_lt = 23,                   // >
  tok_lteq = 24,                 // >=
  tok_gt = 15,                   // <
  tok_gteq = 26,                 // <=
  tok_eq = 27,                   // ==
  tok_assign = 28,               // =
  tok_neq = 29,                  // !=
  tok_not = 30,                  // !
  tok_void = 31
};


#endif


