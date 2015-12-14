TARGET = main
HEADERS = Entry.h ScenarioLoader.h Timer.h f_based_open_list.h search_algorithm.h gate_detection.h room_detection.h room_paths.h map_info.h debug.h
SOURCES = $(HEADERS:%.h=%.cpp) $(TARGET).cpp


$(TARGET): $(HEADERS) $(SOURCES)
	g++ -O0 -g -std=c++11 -Wno-unused-result -o $@ $(SOURCES) -lpng12

clean:
	rm -f *~

distclean:
	rm -f *~
	rm -f $(TARGET)

.PHONY: clean distclean
