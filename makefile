COMPILER=g++
FLAGS=-g -std=c++20
TARGET=main.cpp
OUTPUT_TARGET=./pietra
TEST=test.pi

# COMPILATION

compile:
	$(COMPILER) $(FLAGS) -c `llvm-config --cxxflags` $(TARGET) -o $(OUTPUT_TARGET).o    
	$(COMPILER) $(FLAGS) $(OUTPUT_TARGET).o `llvm-config --ldflags --libs` -lpthread -lncurses -o $(OUTPUT_TARGET)
	rm $(OUTPUT_TARGET).o    
	
	




