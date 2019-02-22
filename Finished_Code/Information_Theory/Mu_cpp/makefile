CPP = clang++
CPPFLAGS =  -Xpreprocessor -fopenmp
LDFLAGS = -lomp
ff : main.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)