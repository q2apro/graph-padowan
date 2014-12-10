BOOST = /usr/include/boost
GSL = /usr/include/gsl

SRC= Parser.cpp TBaseFunc.cpp Calc.cpp Differentiate.cpp MakeText.cpp Simplify.cpp Utility.cpp TFunc.cpp TParamFunc.cpp TPolarFunc.cpp Trend.cpp TFuncData.cpp TCustomFunc.cpp TSymbolList.cpp
OBJ= Parser.o TBaseFunc.o Calc.o Differentiate.o MakeText.o Simplify.o Utility.o TFunc.o TParamFunc.o TPolarFunc.o Trend.o TFuncData.o TCustomFunc.o TSymbolList.o

libFunc32.a: $(OBJ)
	ar -crs libFunc32.a $(OBJ)

$(OBJ): $(SRC) Func32.h Func32Impl.h
	g++ -c $(SRC) -I$(BOOST) -I$(GSL) -gstabs+ #-fprofile-arcs -ftest-coverage

.PHONY : clean

clean:
	-rm -f $(OBJ)

