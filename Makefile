CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS  = -lsqlite3

SRC = src/main.cpp \
      src/Customer.cpp \
      src/Order.cpp \
      src/Database.cpp \
      src/FoodDeliveryApp.cpp

TARGET = food_delivery

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) quickbite.db

run: all
	./$(TARGET)
