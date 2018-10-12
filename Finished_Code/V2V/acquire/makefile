CXX=g++



all: acquire sampler gpstracker




acquire: src/acquire.o src/AppDll.o src/DllDriverApi.o src/DllLibMain.o src/DllLibLoadSvf.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) -o $@ $^ /usr/lib64/libboost_thread-mt.so

src/acquire.o: src/acquire.cpp
	$(CXX) $(CXXFLAGS) -I/home/adm85/boost/boost_atomic -c -o $@ $^




sampler: src/sampler.o src/NamingConvention.o src/DataCapture.o
	$(CXX) $(CXXFLAGS) `pkg-config --libs --cflags gtk+-2.0` -o $@ $^

src/sampler.o: src/sampler.cpp
	$(CXX) $(CXXFLAGS) `pkg-config --libs --cflags gtk+-2.0` -c -o $@ $^




gpstracker: src/gpstracker.o
	$(CXX) $(CXXFLAGS) -o $@ $^




clean:
	rm src/*.o acquire gpstracker sampler