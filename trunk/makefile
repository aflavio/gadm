CC=g++
CFLAGS=-O3 -fexpensive-optimizations -Wall -ansi -DLINUX -DNDEBUG
SRCS=exp_gadm.cpp exp_gadm_info.cpp exp_indcascade.cpp ga.cpp main.cpp model_gadm.cpp model_indcascade.cpp mt19937ar.cpp pair_reader.cpp tokenizer.cpp utils.cpp
OBJS=$(SRCS:.cpp=.o)

all: gadm

gadm: $(OBJS) 
	$(CC) $(CFLAGS) -lm -s -o gadm $(OBJS)
	rm -f $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) gadm 
