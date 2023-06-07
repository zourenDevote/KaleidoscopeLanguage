
#ifdef __CTEST_ENABLE__

#include "parser.h"
#include "test/test.h"
#include <iostream>


static const Token ParseResult[] = {
    tok_def, tok_extern, tok_if, tok_for, tok_while, tok_else, 
    tok_then, tok_in, tok_return, tok_continue, tok_break, tok_struct, 
    tok_switch, tok_case, tok_default, tok_import, tok_id, tok_fnumber, tok_inumber, 
    tok_true, tok_false, tok_literal, tok_bool, tok_char, tok_uchar, 
    tok_short, tok_ushort, tok_int, tok_uint, tok_long, tok_ulong, 
    tok_float, tok_double, tok_add, tok_sub, tok_mul, tok_div, 
    tok_assign, tok_eq, tok_neq, tok_dot, tok_gt, tok_ge, 
    tok_lt, tok_le, tok_not, tok_rh, tok_urh, tok_lh, 
    tok_ulh, tok_or, tok_and, tok_bitor, tok_bitand, tok_bitxor
};


int tokenParserTest() {
    TokenParser *parser = new TokenParser(0);
    int Tok, i = 0;
    Tok = parser->getToken();
    while (Tok != tok_eof)
    {
        if(Tok != ParseResult[i++]) {
            printf("Error to parse token %d\n", ParseResult[i-1]);
            return 1;
        }
        Tok = parser->getToken();      
    }
    return 0;
}

#endif

