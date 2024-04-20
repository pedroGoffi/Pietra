#ifndef LEXER_CPP
#define LEXER_CPP
#include "../include/lexer.hpp"
#include "../include/smallVec.hpp"
#include <asm-generic/errno.h>
#include <cctype>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include "interns.cpp"
using namespace Pietra;

struct StreamInfo {
    const char*  stream;
    Lexer::Token token;
};

const char* stream;
Lexer::Token token;
namespace Pietra::Lexer {
    std::vector<StreamInfo*> streams;
    void stream_snaphot(){
        StreamInfo* si = new StreamInfo {.stream = Core::cstr(stream), .token = token};        
        streams.push_back(si);
    }
    void stream_rewind(){
        auto* si = streams.back();
        streams.pop_back();

        stream = si->stream;
        token  = si->token;
    }
    
}

const char* keyword_run         = cstr("run");
const char* keyword_package     = cstr("package");
const char* keyword_mut         = cstr("mut");
const char* keyword_imut        = cstr("imut");
const char* keyword_if          = cstr("if");
const char* keyword_elif        = cstr("elif");
const char* keyword_else        = cstr("else");
const char* keyword_while       = cstr("while");
const char* keyword_let         = cstr("let");
const char* keyword_const       = cstr("const");
const char* keyword_prep        = cstr("::");
const char* keyword_proc        = cstr("proc");
const char* keyword_as          = cstr("as");
const char* keyword_return      = cstr("return");
const char* keyword_sizeof      = cstr("sizeof");
const char* keyword_struct      = cstr("struct");
const char* keyword_union       = cstr("union");
const char* keyword_enum        = cstr("enum");
const char* keyword_preprocess  = cstr("preprocess");
const char* keyword_use         = cstr("use");
const char* keyword_and         = cstr("and");
const char* keyword_or          = cstr("or");
const char* keyword_not         = cstr("not");
const char* keyword_for         = cstr("for");
const char* keyword_switch      = cstr("switch");
const char* keyword_type        = cstr("type");
const char* keyword_land        = cstr("and");
const char* keyword_lor         = cstr("or");
const char* keyword_case        = cstr("case");
const char* keyword_impl        = cstr("impl");
const char* keyword_default     = cstr("default");
const char* keyword_do          = cstr("do");
const char* keyword_new         = cstr("new");





inline bool Lexer::is_eof(){    
    return token.kind == TK_EOF;
}
void Lexer::init_stream(const char* filename, const char* str){
    token.pos = {0};
    token.pos.filename = filename;
    stream = str;    
    next();    
}
void Lexer::skip_empty(){
    while(std::isspace(*stream)){
        stream++;
    }
}

bool at_comment_begin(){
    return stream[0] == '/' and stream[1] == '/';
}

void Lexer::skip_opt_comment(){    
    if(at_comment_begin()){        
        while(*stream != '\0' and *stream != '\n'){
            stream++;
        }                
    }
    skip_empty();
    if(at_comment_begin()){
        skip_opt_comment();
    }
}


const char* old_pos;
void set_token_start(){
    old_pos = stream;
}
void get_token_offset(Lexer::Token& token){
    Lexer::tokenPos& pos = token.pos;
    
    for(const char* cs = old_pos; cs != stream; cs++){
        char c = *cs;
        if(c == '\n'){
            pos.col = 0;
            pos.line++;
        }
        else {
            pos.col++;
        }
    }
}
void Lexer::next(){    
    Lexer::skip_empty();
    Lexer::skip_opt_comment();
    token.str_start = stream;
    set_token_start();    
    while(isspace(*stream)) stream++;    
    switch(*stream){
        __ALL_CASE_KWDS: 
        {
            token.str_start = stream;
            stream++;
            while(is_keyword(*stream)){
                stream++;
            }
            token.str_end = stream;
            token.name = Core::cstr_range(token.str_start, stream);

            if(token.name == keyword_and){
                token.kind = Lexer::TK_LAND;
            } else if(token.name == keyword_or){
                token.kind = Lexer::TK_LOR;
            } else {
                token.kind = Lexer::TK_NAME;                        
            }
            break;
        }
        
        __ALL_CASE_NUMS:
        {
            token.str_start = stream;
            while(isdigit(*stream)){
                stream++;
            }

            if(*stream == '.' and *(stream + 1) != '.'){ // for range expressions like 'a'...'b'
                stream = token.str_start;
                Lexer::Scanners::scan_float();
            }
            else {                
                stream = token.str_start;
                Lexer::Scanners::scan_int();   
            }
            
            break;
        }
        case '.':
        {
            
            stream++;            
            token.kind = TK_DOT;
            
            if(isdigit(*stream)){
                stream = token.str_start;
                Lexer::Scanners::scan_float();                                
            }   
            else if (*stream == '.'){
                stream++;
                if(*stream == '.'){
                    stream++;
                    token.kind = TK_TRIPLE_DOT;
                }
                else {
                    printf("TOKEN .. is unimplemented.\n");
                    exit(1);
                }
            }            
            token.str_end = stream;
            token.name = Core::cstr_range(token.str_start, token.str_end);
            break;
        }
        #define CASE1(_CHAR, _KIND)  \
            case _CHAR: {                   \
                stream++;                   \
                token.kind  = _KIND;        \
                token.str_end = stream;     \
                token.name  = Core::cstr_range(token.str_start, token.str_end); \
                break;                      \
            }

      

        
        #define CASE2(_C1, _K1, _C2, _K2) \
            case _C1:\
                stream++;\
                if(*stream == _C2){\
                    token.kind = _K2;\
                    stream++;\
                }\
                else {\
                    token.kind = _K1;\
                }\
                token.str_end = stream;\
                token.name  = Core::cstr_range(token.str_start, token.str_end); \
                break;
        #define CASE3(C1, K1, C2, K2, C3, K3)               \
            case C1:                                        \
                stream++;                                   \
                if(*stream == C2){                          \
                    stream++;                               \
                    token.kind = K2;                        \
                }                                           \
                else if (*stream == C3){                    \
                    stream++;                               \
                    token.kind = K3;                        \
                }                                           \
                else {                                      \
                    token.kind = K1;                        \
                }                                           \
                token.str_end = stream;                     \
                token.name = Core::cstr_range(token.str_start, token.str_end);
        CASE1('#', TK_HASH)
        CASE1('(', TK_OPEN_ROUND_BRACES)
        CASE1(')', TK_CLOSE_ROUND_BRACES)
        CASE1('[', TK_OPEN_SQUARED_BRACES)
        CASE1(']', TK_CLOSE_SQUARED_BRACES)
        CASE1('{', TK_OPEN_CURLY_BRACES)
        CASE1('}', TK_CLOSE_CURLY_BRACES)
        CASE1(',', TK_COMMA)
        CASE1(';', TK_DCOMMA)
        CASE1('/', TK_DIV)
        CASE1('*', TK_MULT)
        CASE1('%', TK_MOD)
        CASE1('>', TK_GT)
        CASE1('&', TK_AMPERSAND)
        CASE1('@', TK_NOTE)
        CASE2(':', TK_DDOT, ':', TK_PREP)    
        CASE2('=', TK_EQ, '=', TK_CMPEQ)
        
        CASE2('+', TK_ADD, '+', TK_INC)
        CASE2('-', TK_SUB, '-', TK_DEC);
          
        CASE2('<', TK_LT, '=', TK_LTE)       
        CASE2('!', TK_NOT, '=', TK_NEQ)
        CASE1('|', TK_PIPE)

       
        #undef CASE1
        #undef CASE2

        case '?': {
            stream++;
            token.name = Core::cstr("??");
            token.kind = Lexer::TK_QUESTION;
            
            if(*stream == '?'){
                token.name = Core::cstr("??");
                token.kind = Lexer::TK_DQUESTION;
                stream++;
            }                                    
            break;
        }
        case '\'':
            Lexer::Scanners::scan_char();
            break;
        case '"':            
            Lexer::Scanners::scan_string();
            break;

        
        case '\0': 
        {          
            stream++;  
            token.name = Core::cstr("<EOF>");
            token.kind = Lexer::TK_EOF;            
            break;
        }
        /*
            TODO: warn for undefined tokens
        */        
        default:
        error:
            printf("[ERR]: token.text = %s\nistr = %c\n", token.name, *stream);
            stream++;
    }

    get_token_offset(token);        
}
inline bool Lexer::is_numeric(char c){
    switch(c){
        __ALL_CASE_NUMS: return true;
        default:         return false;
    }
}
inline bool Lexer::is_keyword(char c){
    switch(c){
        __ALL_CASE_KWDS:    return true;
        __ALL_CASE_NUMS:    return true;        
        default:            return false;
    }
}

inline bool Lexer::is_kind(tokenKind kind){
    return token.kind == kind;
}
inline bool Lexer::is_name(const char* name){
    return is_kind(TK_NAME) and token.name == Core::cstr(name);
}

inline bool Lexer::expects_kind(tokenKind kind){
    if(Lexer::is_kind(kind)){
        Lexer::next();
        return true;
    }
    return false;
}
char Lexer::Scanners::scape_to_char(char c){    
    switch(c) {
        case '0':   return '\0';
        case '\'':  return '\'';
        case '"':   return  '"';
        case '\\':  return '\\';
        case 'n':   return '\n';
        case 'r':   return '\r';
        case 't':   return '\t';
        case 'v':   return '\v';
        case 'b':   return '\b';
        case 'a':   return '\a';
        default:    return '\0';
    }
}
void Lexer::Scanners::scan_string(){
    assert(*stream == '"');
    stream++;    
    SVec<char> buff;
    token.str_start = stream;
    while(*stream != '"'){
        
        if(*stream == '\\'){            
            stream++;
            char escape = Scanners::scape_to_char(*stream);
            if(*stream != '0' and escape == 0){
                printf("[SYNTAX-ERROR]: Invalid escape literal: \\%c\n", *stream);
                exit(1);
            }
            
            stream++;
            buff.push(escape);
        }
        else {                
            buff.push(*stream);
            stream++;
        }
    }
    buff.push(0);
    token.kind      = TK_STRING;
    token.str_end   = stream;
    token.name      = Core::cstr(buff.data);    
    assert(*stream == '"');
    stream++;
    
}

void Lexer::Scanners::scan_char(){
    assert(*stream == '\'');
    stream++;    
    SVec<char> buff;
    token.str_start = stream;
    while(*stream != '\''){
        
        if(*stream == '\\'){            
            stream++;
            char escape = Scanners::scape_to_char(*stream);
            if(*stream != '0' and escape == 0){
                printf("[SYNTAX-ERROR]: Invalid escape literal: %c\n", *stream);
                exit(1);
            }
            
            stream++;
            buff.push(escape);
        }
        else {                
            buff.push(*stream);
            stream++;
        }
    }    
    assert(stream and *stream == '\'');
    stream++;
    token.str_end = stream;

    // CHECK FOR STRING LITERAL IN CHAR MODE        
    if(buff.len() == 1){
        // This is a char
        char c = buff.data[0];
        token.kind = TK_INT;        
        token.i64  = (int) c; 
    }
    else {
        // This is a string        
        buff.push(0);
        token.kind      = TK_STRING;        
    }

    token.name      = Core::cstr(buff.data);
    
}

void Lexer::Scanners::scan_int(){
    assert(is_numeric(*stream));    
    int      base = 10;

    
    if(*stream == '0'){
        stream++;    
        if(tolower(*stream) == 'x'){            
            base = 16;
            stream++;
        }        
        else if(tolower(*stream) == 'b'){
            stream++;
            base = 2;
        }
        else if(isdigit(*stream)){
            stream++;
            base = 8;
        }
                
    }

    Lexer::Scanners::scan_int_base(base);
}

int  Lexer::Scanners::char_to_int(char c){
    switch(c){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        
        case 'a': case 'A': return 10; 
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;


        default: return 0;
    }
    
}
void Lexer::Scanners::scan_int_base(int base){
    token.str_start = stream;
    uint64_t val = 0;    
    for(;;){
        int digit = Lexer::Scanners::char_to_int(*stream);        

        if(digit == 0 and *stream != '0'){
            break;
        }

        if(digit >= base){
            printf("[ERR]: digit %c out of range for base %d\n", *stream, base);
            break;
        }

        if(val > (UINT64_MAX - digit)/base){
            printf("[ERR]: integer overflow.\n");
            while(isdigit(*stream)) stream++;            
            val = 0;
            break;
        }

        val = val*base + digit;        
        stream++;
    }
    
    token.str_end = stream;        
    token.name = Core::cstr_range(token.str_start, token.str_end);
    token.kind = tokenKind::TK_INT;
    token.i64  = val;
}
void Lexer::Scanners::scan_float(){
    token.str_start = stream;
    while(isdigit(*stream)) stream++;

    if(*stream == '.'){
        stream++;
    }

    while(isdigit(*stream)){
        stream++;
    }
    if(tolower(*stream) == 'e'){
        stream++;

        if(*stream == '+' or *stream == '-') stream++;

        if(!isdigit(*stream)){
            printf("[ERR]: Expected digit after float literal exponent, found %c\n", *stream);
            exit(1);
        }

        while(isdigit(*stream)) stream++;
    }
    if(tolower(*stream) == 'f') stream++;
    
    token.str_end = stream;
    token.name = Core::cstr_range(
        token.str_start,
        token.str_end
    );
    token.kind = TK_FLOAT;
    token.f64 = strtod(token.str_start, nullptr);

    if(token.f64 == HUGE_VAL or token.f64 == -HUGE_VAL){
        printf("[ERR]: Float literal overflow.\n");
        token.f64 = .0;
    }


}


void Lexer::fprint_token(FILE* file, Token token){
    fprintf(file, "Token(<Token_name: %s", token.name);

    #define CASE(_KIND, TYPE) \
        case _KIND: \
            fprintf(file, ", " TYPE ">");\
            break; 

    switch(token.kind){
        CASE(TK_INT, "Int_i64");
        CASE(TK_FLOAT, "Float_i64");
        CASE(TK_KEYWORD, "Keyword");
        CASE(TK_NAME, "Name");
        
        default:
            fprintf(file, ", Unknown_kind>");

    }
    #undef CASE
    fprintf(file, ")\n");
}
const char* Lexer::tokenKind_repr(tokenKind k){
    switch(k){
        case TK_PIPE:                   return "|";
        case TK_DIV:                    return "/";
        case TK_EOF:                    return "EOF";
        case TK_DOT:                    return ".";
        case TK_DCOMMA:                 return ";";
        case TK_LT:                     return "<";
        case TK_LTE:                    return "<=";        
        case TK_SUB:                    return "-";
        case TK_DEC:                    return "--";
        case TK_GT:                     return ">";
        case TK_ADD:                    return "+";
        case TK_INC:                    return "++";
        case TK_MULT:                   return "*";
        case TK_TRIPLE_DOT:             return "...";
        case TK_OPEN_ROUND_BRACES:      return "(";
        case TK_CLOSE_ROUND_BRACES:     return ")";
        case TK_OPEN_SQUARED_BRACES:    return "[";
        case TK_CLOSE_SQUARED_BRACES:   return "]";
        case TK_OPEN_CURLY_BRACES:      return "{";
        case TK_CLOSE_CURLY_BRACES:     return "}";
        case TK_COMMA:                  return ",";
        case TK_DDOT:                   return ":";
        case TK_PREP:                   return "::";
        case TK_EQ:                     return "=";
        case TK_CMPEQ:                  return "==";
        case TK_NAME:                   return "NAME";
        case TK_KEYWORD:                return "KEYWORD";
        case TK_INT:                    return "INT";
        case TK_FLOAT:                  return "FLOAT";
        case TK_STRING:                 return "STRING";
        case TK_LAND:                   return "and";
        case TK_LOR:                    return "or";
        case TK_NOT:                    return "not";
        case TK_AMPERSAND:              return "&";
        case TK_NEQ:                    return "!=";

        default: 
            printf("repr_TK_KIND: %i\n", k);
            exit(1);            
    }      
}
SVec<const char*> included_packages;
bool is_included(const char* str){
    for(auto& p: included_packages){
        if( p == str) return true;
    }
    return false;
}                
void include_me(const char* str){
    included_packages.push(Core::cstr(str));
}
#endif /*LEXER_CPP*/
