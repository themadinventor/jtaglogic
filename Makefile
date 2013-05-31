CXXFLAGS	+=-O3 -w -c -fpic -I./sdk/include/
LDFLAGS		=-shared -lAnalyzer64 -L./sdk/lib/

all: libAvrJtagAnalyzer.so

libAvrJtagAnalyzer.so: JtagAnalyzer.o JtagAnalyzerResults.o JtagAnalyzerSettings.o JtagSimulationDataGenerator.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o

veryclean: clean
	rm -f libAvrJtagAnalyzer.so
