CXX = g++
CXXFLAGS = -g -lrt

acquire : src/acquire.o src/AppDll.o src/DllDriverApi.o src/DllLibMain.o src/DllLibLoadSvf.o
	$(CXX) $(CXXFLAGS) -o $@ $^
