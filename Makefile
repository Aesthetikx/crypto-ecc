CXX=g++
CFLAGS=-Wall -Werror -pedantic
LIBS=-lgmpxx -lgmp

ec: ec.cpp ec_ops.cpp
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f *.o ec
