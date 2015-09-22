TARGET = main
HEADERS = Entry.h ScenarioLoader.h Timer.h
SOURCES = $(HEADERS:%.h=%.cpp) $(TARGET).cpp


$(TARGET): $(HEADERS) $(SOURCES)
	g++ -o $@ $(SOURCES)

clean:
	@: # nothing to clean

distclean:
	rm -f $(TARGET)

.PHONY: clean distclean
