.PHONY: all run clean

SRC=main.c
TARGET=main.out

all: $(TARGET)
	
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o *.out

$(TARGET): $(SRC)
	gcc -o $(TARGET) $(SRC)