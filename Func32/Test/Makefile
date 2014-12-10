FUNC32 = ..
BOOST = /usr/include/boost

Test: Test.cpp ../libFunc32.a
	g++ Test.cpp -o Test -L$(FUNC32) -lFunc32 -lgsl -lgslcblas -I$(FUNC32) -I$(BOOST) -gstabs #-fprofile-arcs -ftest-coverage
