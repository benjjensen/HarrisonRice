CXX = g++
CXXFLAGS = -g `pkg-config --libs --cflags gtk+-2.0`

sampler : src/NamingConvention.o src/sampler.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) -o $@ $^
