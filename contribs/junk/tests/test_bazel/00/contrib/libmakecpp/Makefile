# Specify extensions of files to delete when cleaning
CLEANEXTS   = o a 

# Specify the target file and the install directory
OUTPUTFILE  = libmakecpp.a

# Default target
.PHONY: all
all: $(OUTPUTFILE)

# Build libjohnpaul.a from john.o, paul.o, and johnpaul.o
$(OUTPUTFILE): john.o paul.o johnpaul.o
	ar ru $@ $^
	ranlib $@

# No rule to build john.o, paul.o, and johnpaul.o from .cpp 
# files is required; this is handled by make's database of
# implicit rules

.PHONY: install
install:
	mkdir -p $(INSTALLDIR)/lib
	mkdir -p $(INSTALLDIR)/include
	cp -p $(OUTPUTFILE) $(INSTALLDIR)/lib
	cp -p johnpaul.hpp $(INSTALLDIR)/include

.PHONY: clean 
clean:
	for file in $(CLEANEXTS); do rm -f *.$$file; done
	rm -f $(OUTPUTFILE)

# Indicate dependencies of .cpp files on .hpp files
john.o: john.cpp
paul.o: paul.cpp
johnpaul.o: john.cpp paul.cpp johnpaul.cpp
