CXX=g++
CFLAGS=-Wall -Werror -pedantic
LIBS=-lgmpxx -lgmp

ec: ec.cpp
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f *.o ec
