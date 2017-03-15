CFLAGS = -W -Wall -g -pedantic -lpthread

fileCrawler: fileCrawler.o libADTS.a
	gcc $(CFLAGS) -o fileCrawler fileCrawler.o libADTS.a

libADTS.a: tsiterator.o tslinkedlist.o tstreeset.o re.o linkedlist.o treeset.o iterator.o
	rm -f libADTS.a
	ar r libADTS.a tsiterator.o tslinkedlist.o tstreeset.o re.o linkedlist.o treeset.o iterator.o
	ranlib libADTS.a

clean:
	rm -f *.o libADTS.a fileCrawler

tsiterator.o: tsiterator.c tsiterator.h
tslinkedlist.o: tslinkedlist.c tslinkedlist.h tsiterator.h linkedlist.h
tstreeset.o: tstreeset.c tstreeset.h tsiterator.h treeset.h
iterator.o: iterator.c iterator.h
linkedlist.o: linkedlist.c linkedlist.h iterator.h
treeset.o: treeset.c treeset.h iterator.h 
re.o: re.c re.h
fileCrawler.o: fileCrawler.c tsiterator.h tslinkedlist.h tstreeset.h linkedlist.h treeset.h re.h
