all:	sen rec

sen:	sender.o
	g++ sender.o -o sen

rec:	recv.o
	g++ recv.o -o rec

sen.o:	 sender.cpp
	g++ -c sender.cpp

rec.o:	recv.cpp
	g++ -c recv.cpp


clean:
	rm -rf *.o sender recv 
