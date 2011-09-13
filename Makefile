
CXX = g++
CXXFLAGS = -lpulse -lboost_program_options -Wall

all: pamixer

pamixer: pamixer.cc pulseaudio.hh
	$(CXX) $(CXXFLAGS) -o $@ pamixer.cc

clean:
	rm -f pamixer

