CXX = g++
CXXFLAGS = -g -lrt `pkg-config --libs --cflags gtk+-2.0`

acquire : src/acquire.o src/AppDll.o src/DllDriverApi.o src/DllLibMain.o src/DllLibLoadSvf.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) -o $@ $^
	
sampler : src/NamingConvention.o src/sampler.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) -o $@ $^