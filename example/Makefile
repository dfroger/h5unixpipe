default: libh5unixpipe.so

PREFIX=/home/ROCQ/clime/common/opt/h5unixpipe

libh5unixpipe.so: h5unixpipe.o minimal_traceback.o
	gcc -shared -Wl,-soname,$(PREFIX)/lib/$@ -o $@ $^

tb: minimal_traceback.o test_minimal_traceback.o
	gcc -o $@ $^

simplewrite: simplewrite.o
	gcc -o $@ $< -lhdf5

install:
	cp libh5unixpipe.so $(PREFIX)/lib
	cp h5unixpipe.h minimal_traceback.h $(PREFIX)/include

main: main.o 
	gcc -o $@ -g $^ -lhdf5_hl -lhdf5 -lh5unixpipe

%.o: %.c
	gcc -c -g $<

clean:
	rm -f simplewrite tb main *.o
