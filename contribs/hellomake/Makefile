all : libhellomake.a

libs: libhellomake.a

libhellomake.a : hellomake.o
	ar rcs libhellomake.a hellomake.o

hellomake.o : hellomake.cpp
	clang++ -c hellomake.cpp

clean :
	rm hellomake.o libhellomake.a

install : libhellomake.a
	touch libhellomake.a
