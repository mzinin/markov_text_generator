CXX = g++
COMPILE_FLAGS = -Wall -Wunused -Wextra -Wshadow -Weffc++ -Wstrict-aliasing -pedantic -Werror -std=c++11 -O3 -c
LINK_FLAGS =

BINARY = ./bin
SOURCE = ./source
OBJECTS = ./obj


all: directories stage_learn stage_use test


directories:
	mkdir -p $(OBJECTS) && mkdir -p $(BINARY)


stage_learn: directories \
             chain_builder.o \
             main_stage_learn.o \
             markov_text_chain.o \
             text_adjuster.o \
             text_downloader.o \
             word_splitter.o
	$(CXX) $(LINK_FLAGS) \
	    $(OBJECTS)/chain_builder.o \
	    $(OBJECTS)/main_stage_learn.o \
	    $(OBJECTS)/markov_text_chain.o \
	    $(OBJECTS)/text_adjuster.o \
	    $(OBJECTS)/text_downloader.o \
	    $(OBJECTS)/word_splitter.o \
	    -o $(BINARY)/stage_learn


stage_use: directories \
           main_stage_use.o \
           markov_text_chain.o \
           text_generator.o
	$(CXX) $(LINK_FLAGS) \
	    $(OBJECTS)/main_stage_use.o \
	    $(OBJECTS)/markov_text_chain.o \
	    $(OBJECTS)/text_generator.o \
	    -o $(BINARY)/stage_use


test: directories \
      main_test.o \
      markov_text_chain.o \
      text_adjuster.o \
      text_downloader.o \
      word_splitter.o
	$(CXX) $(LINK_FLAGS) \
	    $(OBJECTS)/main_test.o \
	    $(OBJECTS)/markov_text_chain.o \
	    $(OBJECTS)/text_adjuster.o \
	    $(OBJECTS)/text_downloader.o \
	    $(OBJECTS)/word_splitter.o \
	    -o $(BINARY)/test


chain_builder.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/chain_builder.cpp -o $(OBJECTS)/chain_builder.o

main_stage_learn.o: 
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/main_stage_learn.cpp -o $(OBJECTS)/main_stage_learn.o

main_stage_use.o: 
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/main_stage_use.cpp -o $(OBJECTS)/main_stage_use.o

main_test.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/main_test.cpp -o $(OBJECTS)/main_test.o

markov_text_chain.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/markov_text_chain.cpp -o $(OBJECTS)/markov_text_chain.o

text_adjuster.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/text_adjuster.cpp -o $(OBJECTS)/text_adjuster.o

text_downloader.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/text_downloader.cpp -o $(OBJECTS)/text_downloader.o

text_generator.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/text_generator.cpp -o $(OBJECTS)/text_generator.o

word_splitter.o:
	$(CXX) $(COMPILE_FLAGS) $(SOURCE)/word_splitter.cpp -o $(OBJECTS)/word_splitter.o

clean:
	@rm -rf $(OBJECTS) & rm -rf $(BINARY)
