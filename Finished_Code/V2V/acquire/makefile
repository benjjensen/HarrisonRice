CXX = g++
CXXFLAGS = -I/home/adm85/boost/boost_atomic -g -lrt `pkg-config --libs --cflags gtk+-2.0`

acquire : src/acquire.o src/AppDll.o src/DllDriverApi.o src/DllLibMain.o src/DllLibLoadSvf.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) -o $@ $^ /usr/lib64/libboost_thread-mt.so
	
sampler : src/NamingConvention.o src/sampler.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) -o $@ $^
