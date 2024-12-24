#ifndef PIETRA_SRC_LOCATION
#define PIETRA_SRC_LOCATION
#include "../include/srcLocation.hpp"
#include "./interns.cpp"

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


#endif /*PIETRA_SRC_LOCATION*/