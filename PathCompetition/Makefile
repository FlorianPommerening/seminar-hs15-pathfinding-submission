TARGET = main
HEADERS = Entry.h ScenarioLoader.h Timer.h g_based_open_list.h
SOURCES = $(HEADERS:%.h=%.cpp) $(TARGET).cpp


$(TARGET): $(HEADERS) $(SOURCES)
	g++ -o $@ $(SOURCES)

clean:
	rm -f *~

distclean:
	rm -f *~
	rm -f $(TARGET)

.PHONY: clean distclean
