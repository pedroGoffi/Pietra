#ifndef PLLLEXERHEADER
#define PLLLEXERHEADER
#include <string>
#include <vector>
#include <cassert>
#include "./srcLocation.hpp"
#include <filesystem>
#define PIETRA_EXTENSION ".pi"

#define DEFINE_ERROR_MSG(_error_kind) \
void _error_kind(SrcLocation loc, const char* fmt, ...);

#define DEFINE_LOG_MSG(_log_kind) \
void _log_kind(SrcLocation loc, const char* fmt, ...);

#define MAKE_ERROR_MSG(_error_kind, _error_message)                              								\
void _error_kind(SrcLocation loc, const char* fmt, ...) {                        								\
    if (loc.name == NULL) {                                                      								\
        loc = loc_builtin;                                                       								\
    }                                                                            								\
    va_list args;                                                                								\
    va_start(args, fmt);                                                         								\
    printf("~%s (line:%d: column:%d): [" _error_message "] ", loc.name, loc.lineNumber, loc.lineOffset); \
    vprintf(fmt, args);                                                          			 					\
    va_end(args);																								\
	exit(1);                                                               	 		  							\
}

#define MAKE_LOG_MSG(_error_kind, _error_message)                              								\
void _error_kind(SrcLocation loc, const char* fmt, ...) {                        								\
    if (loc.name == NULL) {                                                      								\
        loc = loc_builtin;                                                       								\
    }                                                                            								\
    va_list args;                                                                								\
    va_start(args, fmt);                                                         								\
    printf("~%s (line:%d: column:%d): [" _error_message "] ", loc.name, loc.lineNumber, loc.lineOffset); \
    vprintf(fmt, args);                                                          			 					\
    va_end(args);																								\
}

const char* new_keyword;
const char* and_keyword;
const char* or_keyword;
const char* if_keyword;
const char* elif_keyword;
const char* else_keyword;
const char* let_keyword;
const char* const_keyword;
const char* stmt_keyword;
const char* while_keyword;
const char* for_keyword;
const char* fn_keyword;
const char* struct_keyword;
const char* union_keyword;
const char* comptime_keyword;
const char* use_keyword;
const char* extern_keyword;
const char* true_keyword;
const char* false_keyword;
const char* i64_keyword;
const char* int_keyword;
const char* i32_keyword;
const char* i16_keyword;
const char* i8_keyword;
const char* char_keyword;
const char* cstr_keyword;
const char* void_keyword;
const char* f32_keyword;
const char* f64_keyword;
const char* return_keyword;
const char* Self_keyword;
const char* switch_keyword;
const char** intern_table[] = {
	&new_keyword,
	&if_keyword,
	&elif_keyword,
	&else_keyword,
	&let_keyword,
	&const_keyword,
	&stmt_keyword,
	&while_keyword,
	&for_keyword,
	&fn_keyword,
	&struct_keyword,
	&union_keyword,
	&comptime_keyword,
	&use_keyword,
	&extern_keyword,
	&true_keyword,
	&false_keyword,
	&i64_keyword,
	&int_keyword,
	&i32_keyword,
	&i16_keyword,
	&i8_keyword,
	&char_keyword,
	&and_keyword,
	&or_keyword,
	&cstr_keyword,
	&void_keyword,
	&f32_keyword,
	&f64_keyword,
	&return_keyword,
	&Self_keyword,
	&switch_keyword

	// especials
	
};





enum TokenKind {
	TK_eof,
	TK_name,
	TK_int,
	TK_float,
	TK_dqstring,
	TK_sqstring,
	TK_dot,
	TK_tripledot,
	TK_comma,	
	TK_scoperes,
	TK_lparen,
	TK_rparen,
	TK_lbracket,
	TK_rbracket,
	TK_lcurly,
	TK_rcurly,
	TK_question,
	TK_hash,
	TK_decorator,
	TK_colon,
	TK_semicolon,
	TK_minus,
	TK_minusminus,
	TK_minuseq,
	TK_arrow,


	TK_plus,
	TK_pluseq,
	TK_plusplus,

	TK_and,
	TK_andand,
	TK_andeq,

	TK_or,
	TK_oror,
	TK_oreq,

	TK_eq,
	TK_eqeq,
	TK_eqarrow,
	TK_not,
	TK_noteq,

	TK_xor,
	TK_xoreq,

	TK_mod,
	TK_modeq,

	TK_mul,
	TK_muleq,

	TK_div,
	TK_diveq,

	TK_less,
	TK_lesseq,
	TK_greater,
	TK_greatereq,


	TK_shl,
	TK_shleq,
	TK_shr,
	TK_shreq,
	TOKENKIND_COUNT
};

struct Token {
	const char* firstChar;
	const char* lastChar;
	TokenKind   kind;
	const char* name;
	SrcLocation loc;
	union {
		long		i64;
		double		f64;
		const char* string;
	};

    const bool isKind(TokenKind kind) const;

	void print();
};

class LexerStream {
public:
	const char* inputStream;
	const char* eof;
	const char* parsePoint;
public:
	void setStream(const char* inputStream, const char* eof);
};




class Lexer {
private:
	LexerStream				stream;	
	SrcLocation				loc;	
	bool 					parsingTypeSpecTemplate;
public:    
	Token					token;
	std::filesystem::path	contextPath;
private:
	bool isWhiteSpace(const char* p);
	void skipWhiteSpaces();
	
    const bool isIdentifier() const;
    const bool isNumber() const;

	const char* scanString();
	double scanFloat();

	
	int scanInteger();

	int charToInt(char c);
	char charToScape(char c);
	Token lexNextToken();
	const char* advanceChar();
	
public:	
	Lexer(const char* data);
	Lexer(const char* begin, const char* end);
	static Lexer* open_at_file(const char* file_path);
	Token nextToken();
	Token getToken();
	const bool isEof() const;
	const bool expect(TokenKind kind);
	const bool matchKeyword(const char* keyword) const;
	const bool expectKeyword(const char* keyword);
	bool checkTemplate();
	SrcLocation getLocation();
	void endOfparsingTemplateContext();
	void parsingTemplateContext();
	void expectSemiColon();
	const char*& getPtr();

	void setContextPath(std::filesystem::path path); 
};

const char* tokenKindRepr(TokenKind kind);
void error(SrcLocation loc, const char* fmt, ...);
DEFINE_ERROR_MSG(syntax_error);
DEFINE_ERROR_MSG(compiler_error);
DEFINE_ERROR_MSG(resolver_error);
DEFINE_LOG_MSG(compiler_log);
void resolver_log(SrcLocation loc, const char* fmt, ...);
void setDebug(bool state);
#define fatal_error(...) (error(__VA_ARGS__), exit(1))
#define error_here(...) (error(token.pos, __VA_ARGS__))
#define warning_here(...) (error(token.pos, __VA_ARGS__))
#define fatal_error_here(...) (error_here(__VA_ARGS__), exit(1)) // should be abort()

#endif /*PLLLEXERHEADER*/