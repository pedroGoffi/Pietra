#ifndef PLLLEXER
#define PLLLEXER
#include "../include/new_lexer.hpp"
#include "../include/srcLocation.hpp"
#include <cstdarg>
#include "interns.cpp"

void initInternKeywords() {

#define DEF_INTERN(x) x##_keyword = cstr(#x)
	DEF_INTERN(new);
	DEF_INTERN(if);
	DEF_INTERN(elif);
	DEF_INTERN(else);
	DEF_INTERN(let);
	DEF_INTERN(const);
	DEF_INTERN(stmt);
	DEF_INTERN(while);
	DEF_INTERN(for);
	DEF_INTERN(fn);
	DEF_INTERN(struct);
	DEF_INTERN(union);
	DEF_INTERN(comptime);
	DEF_INTERN(use);
	DEF_INTERN(extern);
	DEF_INTERN(true);
	DEF_INTERN(false);
	DEF_INTERN(i64);
	DEF_INTERN(int);
	DEF_INTERN(i32);
	DEF_INTERN(i16);
	DEF_INTERN(i8);
	DEF_INTERN(char);
	DEF_INTERN(cstr);
	DEF_INTERN(void);
	DEF_INTERN(f32);
	DEF_INTERN(f64);
	DEF_INTERN(return);
	DEF_INTERN(Self);
#undef DEF_INTERN
}
const char* check_not_intern(const char* str) {
	for(const char** intern: intern_table){
		if (*intern == str) return strf("keyword '%s' is internal.", *intern);
	}
	return nullptr;
}
const char* tokenKindRepr(TokenKind kind) {
	static_assert(TOKENKIND_COUNT == 53, "Need fix here boss");
	switch (kind) {
	case TK_eof:          return "TK_eof";
	case TK_name:         return "TK_name";
	case TK_int:          return "TK_int";
	case TK_float:        return "TK_float";
	case TK_dqstring:     return "TK_dqstring";
	case TK_sqstring:     return "TK_sqstring";
	case TK_dot:          return "TK_dot";
	case TK_tripledot:    return "TK_tripledot";
	case TK_comma:        return "TK_comma";
	case TK_scoperes:     return "TK_scoperes";
	case TK_lparen:       return "TK_lparen";
	case TK_rparen:       return "TK_rparen";
	case TK_lbracket:     return "TK_lbracket";
	case TK_rbracket:     return "TK_rbracket";
	case TK_lcurly:       return "TK_lcurly";
	case TK_rcurly:       return "TK_rcurly";
	case TK_question:     return "TK_question";
	case TK_colon:        return "TK_colon";
	case TK_semicolon:    return "TK_semicolon";
	case TK_minus:        return "TK_minus";
	case TK_minusminus:   return "TK_minusminus";
	case TK_minuseq:      return "TK_minuseq";
	case TK_arrow:        return "TK_arrow";
	case TK_plus:         return "TK_plus";
	case TK_pluseq:       return "TK_pluseq";
	case TK_plusplus:     return "TK_plusplus";
	case TK_and:          return "TK_and";
	case TK_andand:       return "TK_andand";
	case TK_andeq:        return "TK_andeq";
	case TK_or:           return "TK_or";
	case TK_oror:         return "TK_oror";
	case TK_oreq:         return "TK_oreq";
	case TK_eq:           return "TK_eq";
	case TK_eqeq:         return "TK_eqeq";
	case TK_eqarrow:      return "TK_eqarrow";
	case TK_not:          return "TK_not";
	case TK_noteq:        return "TK_noteq";
	case TK_xor:          return "TK_xor";
	case TK_xoreq:        return "TK_xoreq";
	case TK_mod:          return "TK_mod";
	case TK_modeq:        return "TK_modeq";
	case TK_mul:          return "TK_mul";
	case TK_muleq:        return "TK_muleq";
	case TK_div:          return "TK_div";
	case TK_diveq:        return "TK_diveq";
	case TK_less:         return "TK_less";
	case TK_lesseq:       return "TK_lesseq";
	case TK_greater:      return "TK_greater";
	case TK_greatereq:    return "TK_greatereq";
	case TK_shl:          return "TK_shl";
	case TK_shleq:        return "TK_shleq";
	case TK_shr:          return "TK_shr";
	case TK_shreq:        return "TK_shreq";
	}

}
static Token makeToken(PLexer* lexer, const char* fc, const char* lc, TokenKind kind, long i64) {
	return {
		.firstChar = fc,
		.lastChar = lc,
		.kind = kind,
		.name = cstr_range(fc, lc),
		.loc = lexer->getLocation(),
		.i64 = i64
	};
}
static Token makeToken(PLexer* lexer, const char* fc, const char* lc, TokenKind kind, double f64) {
	return {
		.firstChar = fc,
		.lastChar = lc,
		.kind = kind,
		.name = cstr_range(fc, lc),
		.loc = lexer->getLocation(),
		.f64 = f64
	};
}

static Token makeToken(PLexer* lexer, const char* fc, const char* lc, TokenKind kind, const char* string) {
	return {
		.firstChar = fc,
		.lastChar = lc,
		.kind = kind,
		.name = cstr_range(fc, lc),
		.loc = lexer->getLocation(),
		.string = string
	};
}
static Token makeTokenPonctuation(PLexer* lexer, const char* fc, const char* lc, TokenKind kind) {
	return makeToken(lexer, fc, lc, kind, "");	
}
static Token makeIdentifierToken(PLexer* lexer, const char* fc, const char* lc){
	return makeToken(lexer, fc, lc, TK_name, "");
}

static Token makeTokenEof(PLexer* lexer) {
	return makeToken(lexer, nullptr, nullptr, TK_eof, nullptr);	
}
const bool Token::isKind(TokenKind kind) const {
	return this->kind == kind;
}

void Token::print() {
	if(this->isKind(TK_sqstring) or this->isKind(TK_dqstring)) {
		printf("TK_string = '%s'\n", this->name);
	}
	else if (this->isKind(TK_name)) {
		printf("TK_name = '%s'\n", this->name);
	}

	else if (this->isKind(TK_int)) {
		printf("TK_int = %li\n", this->i64);
	}
	else if (this->isKind(TK_float)) {
		printf("TK_float = %lf\n", this->f64);
	}

	else {
		printf("%s\n", tokenKindRepr(this->kind));
	}
}
SrcLocation::SrcLocation() {
	this->name = nullptr;
	this->lineNumber = 0;
	this->lineOffset = 0;
}
SrcLocation::SrcLocation(const char* name, int lineNumber) {
	this->name = name;
	this->lineNumber = lineNumber;
	this->lineOffset = 0;
}
void SrcLocation::advanceLine() {	
	this->lineNumber++;
	this->lineOffset = 0;
}
void SrcLocation::addOffset() { this->lineOffset++; }

void SrcLocation::newContext(const char* filename){
	this->name			= cstr(filename);
	this->lineNumber	= 0;
	this->lineOffset	= 0;
}

void LexerStream::setStream(const char* inputStream, const char* eof)
{
	this->inputStream = inputStream;
	this->parsePoint = this->inputStream;	
	this->eof = eof;
	
}

const bool PLexer::isEof() const {
	return (this->stream.parsePoint == this->stream.eof);
}
const bool PLexer::isIdentifier() const {
	return ((*this->stream.parsePoint >= 'a' and *this->stream.parsePoint <= 'z')
		or	(*this->stream.parsePoint >= 'A' and *this->stream.parsePoint <= 'Z')
		or	(*this->stream.parsePoint == '_'));
}
const bool PLexer::isNumber() const {
	return *this->stream.parsePoint >= '0' and *this->stream.parsePoint <= '9';
}
bool PLexer::isWhiteSpace(const char* p) {	
	return p and (*p == ' ' or *p == '\t' or *p == '\r' or *p == '\n' or *p == '\f');
}
void PLexer::skipWhiteSpaces() {
	for (;;) {		
		while (this->stream.parsePoint != this->stream.eof) {
			int increment = this->isWhiteSpace(this->stream.parsePoint);
			if (increment == 0) break;

			if (this->stream.eof and this->stream.parsePoint + increment > this->stream.eof) {
				printf("TOKEN ERROR. TODO:\n");
				exit(1);
			}
			for (int i = 0; i < increment; i++) this->advanceChar();
			//this->stream.parsePoint += increment;
		}

		// Skip comments
		if (!this->isEof() and this->stream.parsePoint[0] == '/' and this->stream.parsePoint[1] == '/') {

			while (this->stream.parsePoint != this->stream.eof and *this->stream.parsePoint != '\r' and *this->stream.parsePoint != '\n')
				this->advanceChar();
				
			continue;
		}

		if (!this->isEof() and this->stream.parsePoint[0] == '/' and this->stream.parsePoint[0] == '*') {
			const char* start = this->stream.parsePoint;

			this->advanceChar(); this->advanceChar();

			while (this->stream.parsePoint != this->stream.eof and (this->stream.parsePoint[0] != '*' or this->stream.parsePoint[1] != '/')) this->advanceChar();

			if (this->stream.parsePoint == this->stream.eof) {
				printf("TOKEN ERROR. TODO:\n");
				exit(1);
			}

			this->advanceChar(); this->advanceChar();
			continue;
		}

		break;
	}	
}
// line 600
int PLexer::charToInt(char c) {
	switch (c) {
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
char PLexer::charToScape(char c){
	switch (c) {
		case 't': 	return '\t';
		case 'b': 	return '\b';
		case 'n': 	return '\n';
		case 'r': 	return '\r';
		case 'f': 	return '\f';
		case '\'': 	return '\'';
		case '"': 	return '\"';
		case '\\': 	return '\\';
	}
	return -1;
}

int PLexer::scanInteger(){
	assert(this->isNumber());
	int base = 10;
	if (*this->stream.parsePoint == '0') {
		this->advanceChar();
		if (tolower(*this->stream.parsePoint) == 'x') {
			base = 16;
			this->advanceChar();
		}
		else if (tolower(*this->stream.parsePoint) == 'b') {
			this->advanceChar();
			base = 2;
		}
		else if (isdigit(*this->stream.parsePoint)) {
			this->advanceChar();
			base = 8;
		}
	}

	long val = 0;
	for (;;) {
		int digit = this->charToInt(*this->stream.parsePoint);
		if (digit == 0 and *this->stream.parsePoint != '0') {
			break;
		}

		if (digit >= base) {
			printf("[ERROR]: digit %c out of range for base %d\n", *this->stream.parsePoint, base);
			break;
		}

		if (val > (UINT64_MAX - digit) / base) {
			printf("[ERROR]: integer overflow.\n");
			while (isdigit(*this->stream.parsePoint)) this->advanceChar();
			val = 0;
			break;
		}

		val = val * base + digit;
		this->advanceChar();
	}
	
	return val;
}
double PLexer::scanFloat() {
	const char* begin = this->stream.parsePoint;
	while (isdigit(*this->stream.parsePoint)) this->advanceChar();

	if (*this->stream.parsePoint == '.') {
		this->advanceChar();
	}

	while (isdigit(*this->stream.parsePoint)) {
		this->advanceChar();
	}
	if (tolower(*this->stream.parsePoint) == 'e') {
		this->advanceChar();

		if (*this->stream.parsePoint == '+' or *this->stream.parsePoint == '-') this->advanceChar();

		if (!isdigit(*this->stream.parsePoint)) {
			printf("[ERR]: Expected digit after float literal exponent, found %c\n", *this->stream.parsePoint);
			exit(1);
		}

		while (isdigit(*this->stream.parsePoint)) this->advanceChar();
	}
	if (tolower(*this->stream.parsePoint) == 'f') this->advanceChar();
	const char* name = cstr_range(begin, this->stream.parsePoint);	
	double f64 = strtod(name, nullptr);
	
	if (f64 == HUGE_VAL or f64 == -HUGE_VAL) {
		printf("[ERROR]: Float literal overflow.\n");
		f64 = .0;
	}
	return f64;
}
const char* PLexer::scanString() {
	assert((*this->stream.parsePoint == '"' or *this->stream.parsePoint == '\'') and "Expected single quotes or double quotes when scanning a token string");
	
	char delim = *this->stream.parsePoint;
	const char* begin = this->advanceChar();
	std::string buff;
	while (*this->stream.parsePoint != delim) {
		if (*this->stream.parsePoint == '\\') {
			this->advanceChar();
			char escape = this->charToScape(*this->stream.parsePoint);
			if (escape < 0) {
				printf("[ERROR]: invalid espace char %c\n", *this->stream.parsePoint);
				exit(1);
			}			
			buff += escape;			
		}
		else {
			buff += *this->stream.parsePoint;
		}
		this->advanceChar();
	}
	if (this->isEof()) syntax_error(this->getLocation(), "Expected '\"' after string body, got EOF.\n");
	assert(*this->stream.parsePoint == delim);
	buff += '\0';
	this->advanceChar();
	return cstr(buff.begin()._Ptr, buff.end()._Ptr);
}

Token PLexer::getToken() {			
	return this->token;
}
Token PLexer::nextToken() {
	this->token = this->lexNextToken();
	
	return this->token;
}
Token PLexer::lexNextToken() {	
	this->skipWhiteSpaces();
	if (this->isEof()) {
		return makeTokenEof(this);
	}
	const char* begin = this->stream.parsePoint;
	
	switch (*this->stream.parsePoint) {
	default: {
		this->stream.parsePoint;
		if (this->isIdentifier()) {
			do this->advanceChar(); while (!this->isEof() and (this->isIdentifier() or this->isNumber()));
			return makeIdentifierToken(this, begin, this->stream.parsePoint);			
		}

		if (this->isEof()) return makeTokenEof(this);

		printf("ERROR: unkown token starts with: %c\n", *this->stream.parsePoint);
		exit(1);
	}
	
	case ',': {
		this->advanceChar();
		TokenKind kind = TK_comma;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '+': {
		this->advanceChar();
		TokenKind kind = TK_plus;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '+') {
				kind = TK_plusplus;
				this->advanceChar();
			}
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_pluseq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '-': {
		this->advanceChar();
		TokenKind kind = TK_minus;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '-') {
				kind = TK_minusminus;
			}
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_minuseq;
				this->advanceChar();
			}
			if (this->stream.parsePoint[0] == '>') {
				kind = TK_arrow;
				this->advanceChar();
			}
		}
	
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '&': {
		this->advanceChar();
		TokenKind kind = TK_and;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '&') {
				kind = TK_andand;
				this->advanceChar();
			}
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_andeq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '|': {
		this->advanceChar();
		TokenKind kind = TK_or;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '|') {
				kind = TK_oror;
				this->advanceChar();
			}
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_oreq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '=': {
		this->advanceChar();
		TokenKind kind = TK_eq;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_oror;
				this->advanceChar();
			}
			if (this->stream.parsePoint[0] == '>') {
				kind = TK_eqarrow;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '!': {
		this->advanceChar();
		TokenKind kind = TK_not;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_noteq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '^': {
		this->advanceChar();
		TokenKind kind = TK_xor;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_xoreq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '%': {
		this->advanceChar();
		TokenKind kind = TK_mod;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_modeq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '(': {
		this->advanceChar();
		TokenKind kind = TK_lparen;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case ')': {
		this->advanceChar();
		TokenKind kind = TK_rparen;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '[': {
		this->advanceChar();
		TokenKind kind = TK_lbracket;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case ']': {
		this->advanceChar();
		TokenKind kind = TK_rbracket;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '{': {
		this->advanceChar();
		TokenKind kind = TK_lcurly;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '}': {
		this->advanceChar();
		TokenKind kind = TK_rcurly;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '?': {
		this->advanceChar();
		TokenKind kind = TK_question;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case ':': {
		this->advanceChar();
		TokenKind kind = TK_colon;
		if (!this->isEof()) {
			if (*this->stream.parsePoint == ':') {
				this->advanceChar();
				kind = TK_scoperes;
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case ';': {
		this->advanceChar();
		TokenKind kind = TK_semicolon;
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}


	case '.': {
		this->advanceChar();
		TokenKind kind = TK_dot;
		if(!this->isEof() and this->isNumber()){
			this->stream.parsePoint = begin;
			double f64 = this->scanFloat();
			return makeToken(this, begin, this->stream.parsePoint, TK_float, f64);
		}
		if (!this->isEof() and this->stream.parsePoint + 1 != this->stream.eof) {
			if (this->stream.parsePoint[0] == '.' and this->stream.parsePoint[1] == '.') {
				kind = TK_tripledot;
				this->advanceChar();
				this->advanceChar();

			}

		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}

	case '*': {
		this->advanceChar();
		TokenKind kind = TK_mul;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_muleq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '/': {
		this->advanceChar();
		TokenKind kind = TK_div;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				kind = TK_diveq;
				this->advanceChar();
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '<': {
		this->advanceChar();
		TokenKind kind = TK_less;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				this->advanceChar();
				kind = TK_lesseq;
			}

			if (this->stream.parsePoint[0] == '<') {
				this->advanceChar();
				kind = TK_shl;
				if (!this->isEof() and this->stream.parsePoint[0] == '=') {
					this->advanceChar();
					kind = TK_shleq;
				}
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '>': {
		this->advanceChar();
		TokenKind kind = TK_greater;
		if (this->stream.parsePoint != this->stream.eof) {
			if (this->stream.parsePoint[0] == '=') {
				this->advanceChar();
				kind = TK_greatereq;
			}
			
			if (this->stream.parsePoint[0] == '>' and !this->parsingTypeSpecTemplate) {				
				this->advanceChar();
				kind = TK_shr;
				if (!this->isEof() and this->stream.parsePoint[0] == '=') {
					this->advanceChar();
					kind = TK_shreq;
				}
			}
		}
		return makeTokenPonctuation(this, begin, this->stream.parsePoint, kind);
	}
	case '"': {
		const char* string = this->scanString();
		return makeToken(this, begin, this->stream.parsePoint - 1, TK_dqstring, string);
	}
	case '\'': {
		const char* string = this->scanString();
		return makeToken(this, begin, this->stream.parsePoint - 1, TK_sqstring, string);
	}
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
		while (isdigit(*this->stream.parsePoint)) {
			this->advanceChar();
		}
		if (!this->isEof()) {
			if (*this->stream.parsePoint == '.') {
				this->stream.parsePoint = begin;
				double f64 = this->scanFloat();
				return makeToken(this, begin, this->stream.parsePoint, TK_float, f64);
			}
		}
		this->stream.parsePoint = begin;
		long i64 = this->scanInteger();
		return makeToken(this, begin, this->stream.parsePoint - 1, TK_int, i64);
	}
	}
	return {};
}
SrcLocation PLexer::getLocation() { return this->loc; }

const bool PLexer::expect(TokenKind kind){
	if (this->token.isKind(kind)) {
		this->nextToken();
		return true;
	}
	return false;
}
const bool PLexer::matchKeyword(const char* keyword) const {
	return this->token.isKind(TK_name) and this->token.name == keyword;
}
const bool PLexer::expectKeyword(const char* keyword) {
	if (this->matchKeyword(keyword)) {
		this->nextToken();
		return true;
	}
	return false;
}
bool PLexer::checkTemplate() {	
	if (this->stream.parsePoint == this->stream.eof) return false;		
	if (!this->token.isKind(TK_less)) return false;
	
	
	const char* p = this->stream.parsePoint;
	int count = 1;	
	while (p != this->stream.eof and *p != '\n') {		
		if (*p == '<') count++;
		if (*p == '>') {
			count--;
			if (count == 0) return true;
		}
		
		p++;
	}
	
	return count == 0;
}
const char* PLexer::advanceChar() {		
	if (this->stream.parsePoint != this->stream.eof) {
		this->loc.addOffset();

		if (*this->stream.parsePoint == '\n') {						
			this->loc.advanceLine();
		}
	}
	
	return this->stream.parsePoint++;
}

void PLexer::endOfparsingTemplateContext() {
	this->parsingTypeSpecTemplate = false;
}
void PLexer::parsingTemplateContext(){
	this->parsingTypeSpecTemplate = true;
}
void PLexer::expectSemiColon(){
	if (!this->expect(TK_semicolon)) syntax_error(this->getLocation(), "Expected ';'.\n");
}
const char*& PLexer::getPtr() { return this->stream.parsePoint; }
void PLexer::setContextPath(std::filesystem::path path) { 
	this->contextPath = path; 
	this->loc.newContext(path.string().c_str());

}
PLexer::PLexer(const char* data)
{
	this->stream.setStream(data, data + std::strlen(data));
	this->loc = SrcLocation();	
	this->parsingTypeSpecTemplate = false;
	this->nextToken();
}

PLexer::PLexer(const char* begin, const char* end) {
	this->stream.setStream(begin, end);
	this->loc = SrcLocation();
	this->parsingTypeSpecTemplate = false;
	this->nextToken();
}

SrcLocation loc_builtin("<stdin>", 0);
void error(SrcLocation loc, const char* fmt, ...) {
	if (loc.name == nullptr) {
		loc = loc_builtin;
	}
	va_list args;	
	va_start(args, fmt);
	printf("%s\n(%d: %d): error: ", loc.name, loc.lineNumber, loc.lineOffset);
	vprintf(fmt, args);
	printf("\n\n");
	va_end(args);
}
void syntax_error(SrcLocation loc, const char* fmt, ...) {
	if (loc.name == nullptr) {
		loc = loc_builtin;
	}
	va_list args;
	va_start(args, fmt);
	printf("%s\n(%d: %d): [SYNTAX ERROR]: ", loc.name, loc.lineNumber, loc.lineOffset);
	vprintf(fmt, args);
	printf("\n\n");
	va_end(args);
	system("pause");
	exit(1);
}



#ifdef DEV_TEST
void make_test(const char* data){
	Lexer lexer(data);
	while (!lexer.isEof()) {
		Token tok = lexer.nextToken();
		tok.print();
	}

	const char* intern_1 = cstr("new");
	const char* intern_2 = cstr("new");

	if (intern_1 != intern_2) {
		printf("Intern logic error.\n");
		system("pause");
		exit(1);
	}
	printf("---- INTERN ---- \n");
	printf("%s:%p\n", intern_1, intern_1);
	printf("%s:%p\n", intern_2, intern_2);


}
void lexer_test() {
	printf("Lexer test.\n");
	make_test("iAmAName very cool 12.32 10 10 == 1 + 1 !!weqw!");
}
#endif /*DEV_TEST*/
#endif /*PLLLEXER*/