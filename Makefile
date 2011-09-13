
CXX = g++
CXXFLAGS = -lpulse -Wall

all: pamixer

pamixer: pamixer.cc pulseaudio.hh
	$(CXX) $(CXXFLAGS) -o $@ pamixer.cc

clean:
	rm -f pamixer

