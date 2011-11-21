include Makefile.in

.PHONY: all exe doc clean realclean

exe: sph.x 
doc: main.pdf derivation.pdf
all: exe doc

# =======

sph.x: sph.o params.o state.o interact.o leapfrog.o io_bin.o timing.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

sph.o: sph.c params.h state.h interact.h leapfrog.h io.h timing.h

params.o: params.c params.h
state.o: state.c state.h
interact.o: interact.c interact.h state.h params.h
leapfrog.o: leapfrog.c leapfrog.h state.h params.h
io_txt.o: io_txt.c io.h
io_bin.o: io_bin.c io.h

%.o: %.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $<

io_bin.o: io_bin.c
	$(CC) -c $(CFLAGS) $<

# =======
main.pdf: main.tex codes.tex
derivation.pdf: derivation.tex check_derivation.tex

codes.tex: params.h state.h interact.c leapfrog.c sph.c params.c io_bin.c
	dsbweb -o $@ -c $^

check_derivation.tex: check_derivation.m
	dsbweb -o $@ -m $^

%.pdf: %.tex
	pdflatex $<
	pdflatex $<

# =======
view: 
	java -jar ../jbouncy/Bouncy.jar run.out

# =======
tgz: realclean
	(cd ..; tar -czf sph.tgz sph)

clean:
	rm -f *~ *.o
	rm -f codes.tex check_derivation.tex
	rm -f main.log main.aux main.out main.toc
	rm -f derivation.log derivation.aux derivation.out

realclean: clean
	rm -f main.pdf derivation.pdf *.x run.out
