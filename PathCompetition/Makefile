TARGET = main
HEADERS = \
    debug.h \
    Entry.h \
    f_based_open_list.h \
    gate_detection.h \
    map_info.h \
    pair_open_list.h \
    room_detection.h \
    room_paths.h \
    ScenarioLoader.h \
    search_algorithm.h \
    search_algorithm_exits.h \
    search_space.h \
    Timer.h \

SOURCES = $(HEADERS:%.h=%.cpp) $(TARGET).cpp


$(TARGET): $(HEADERS) $(SOURCES)
	g++ -O3 -g -std=c++11 -Wno-unused-result -o $@ $(SOURCES) -lpng12

clean:
	rm -f *~

distclean:
	rm -f *~
	rm -f $(TARGET)

.PHONY: clean distclean
