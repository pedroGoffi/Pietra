#ifndef PIETRA_LEXER
#define PIETRA_LEXER

#include "smallVec.hpp"
#include <cstdint>
#include <string>
#define __ALL_CASE_KWDS case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_'
#define __ALL_CASE_NUMS case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'

namespace Pietra::Lexer{    
    enum tokenKind {
        TK_EOF,
        TK_DOT, 
        TK_DCOMMA,
        TK_LT,
        TK_LAND,
        TK_LOR,
        TK_GT,
        TK_ADD,
        TK_AMPERSAND,
        TK_SUB,
        TK_DEC,
        TK_NOT,
        TK_INC,
        TK_MULT,
        TK_NOTE,
        TK_TRIPLE_DOT,
        TK_OPEN_ROUND_BRACES, 
        TK_CLOSE_ROUND_BRACES,
        TK_OPEN_SQUARED_BRACES,
        TK_CLOSE_SQUARED_BRACES,
        TK_OPEN_CURLY_BRACES,
        TK_CLOSE_CURLY_BRACES,        
        TK_COMMA,
        TK_DDOT,
        TK_PREP,
        TK_EQ,
        TK_CMPEQ,        
        TK_NAME = 69,
        TK_KEYWORD,
        TK_INT,
        TK_STAR,
        TK_FLOAT,
        TK_STRING,
    };
    const char* tokenKind_repr(tokenKind k);
    struct Token{    
        tokenKind   kind;            

        const char* str_start;
        const char* str_end;
        const char* name;        
        union {
            int64_t     i64;
            double      f64;
            
        };
    };


    /* 
        Will lex/tokenize the next word
        and assign the information in the token structure
    */
    void next();

    /*
        will skip all empty charactes and uptade the token position
    */
    void skip_empty();

    /**/
    inline bool is_keyword(char c);
    inline bool is_numeric(char c);
    inline bool is_kind(tokenKind kind);
    inline bool expects_kind(tokenKind kind);
    inline bool is_eof();
    void init_stream(const char* str);        
    void lexer_test();

    void fprint_token(FILE* file, Token token);
    void init_lexer_keyword();

}

namespace Pietra::Lexer::Scanners {
    
    void scan_char();
    void scan_string();
    void scan_int();    
    void scan_float();    
    char scape_to_char(char c);
    void scan_int_base(int base);        
    int  char_to_int(char c);
}


#endif /*PIETRA_LEXER*/