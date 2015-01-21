default: libh5unixpipe.so libh5unixpipe_cxx.so

PREFIX=/home/ROCQ/clime/common/opt/h5unixpipe

libh5unixpipe.so: h5unixpipe.o minimal_traceback.o
	gcc -shared -Wl,-soname,$(PREFIX)/lib/$@ -o $@ $^

libh5unixpipe_cxx.so: h5unixpipe_cxx.o
	g++ -shared -Wl,-soname,$(PREFIX)/lib/$@ -o $@ $^

%.o: %.c
	gcc -c -g -fPIC $<

%.o: %.cxx
	g++ -c -g -fPIC $<

install:
	cp libh5unixpipe.so libh5unixpipe_cxx.so $(PREFIX)/lib
	cp h5unixpipe.h h5unixpipe_cxx.hxx minimal_traceback.h $(PREFIX)/include

clean:
	rm -f simplewrite tb main *.o *.so
