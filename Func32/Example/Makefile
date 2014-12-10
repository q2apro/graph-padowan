FUNC32 = ..
BOOST = /usr/include/boost

Test: Example.cpp ../libFunc32.a
	g++ Example.cpp -o Example -L$(FUNC32) -lFunc32 -lgsl -lgslcblas -I$(FUNC32) -I$(BOOST) -gstabs #-fprofile-arcs -ftest-coverage
