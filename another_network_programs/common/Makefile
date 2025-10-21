CXX := g++
CXXFLAGS := -g -c
TARGET := main.out
SRC_DIR := src
OBJ_DIR := obj

$(shell mkdir -p $(OBJ_DIR))

$(TARGET): $(OBJ_DIR)/main.o
	$(CXX) -g $(OBJ_DIR)/main.o -o $(TARGET)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
