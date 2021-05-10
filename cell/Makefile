CC=mpicc
CFLAGS=-I.
DEPS =
OBJ = mpicell2d.o CellAut.o 2DCellAut.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# mpicell: $(OBJ)
# 	$(CC) -o $@ $^ $(CFLAGS)

mpicell2d: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~
