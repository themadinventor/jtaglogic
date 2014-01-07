CXXFLAGS	+=-O3 -w -c -fpic -I./sdk/include/
LDFLAGS		=-shared -lAnalyzer64 -L./sdk/lib/

all: libJtagAnalyzer.so

libJtagAnalyzer.so: JtagAnalyzer.o JtagAnalyzerResults.o JtagAnalyzerSettings.o JtagSimulationDataGenerator.o \
			JtagPlainAnalyzer.o JtagAvrAnalyzer.o JtagArmAnalyzer.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o

veryclean: clean
	rm -f libJtagAnalyzer.so
