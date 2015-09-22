TARGET = main
HEADERS = Entry.h ScenarioLoader.h Timer.h
SOURCES = $(HEADERS:%.h=%.cpp) $(TARGET).cpp


$(TARGET): $(HEADERS) $(SOURCES)
	g++ -o $@ $(SOURCES)

clean:
	rm -f *~

distclean:
	rm -f *~
	rm -f $(TARGET)

.PHONY: clean distclean
