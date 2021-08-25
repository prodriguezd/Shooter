CC = gcc
CFLAGS = -Wall -Wextra 
DEPS = Info.h Find.h Delete.h Structs.h 
OBJ = Info.o Find.o Delete.o Shooter.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shooter: $(OBJ)
	@echo "Working on main"
	$(CC) -o $@ $^ $(CFLAGS)
	@echo "Done!"

.PHONY: clean

clean:
	@echo "Cleaning up"
	rm -f *.o

