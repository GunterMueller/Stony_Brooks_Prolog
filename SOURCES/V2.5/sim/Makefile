CFLAGS = -c -O
# CC = /usr/local/gcc
CC = cc
OBJS = dispatch.o init.o float.o io.o sub_inst.o loader.o main.o parse_oprnd.o dis.o print_inst.o unify.o load_work.o aux.o evalexp.o
sbprolog: $(OBJS) builtin/builtin
	$(CC) -o sbprolog $(OBJS) builtin/builtin
dispatch.o: dispatch.c inst.h
	$(CC)  $(CFLAGS) dispatch.c 
dis.o: dis.c sim.h inst.h
	$(CC)  $(CFLAGS) dis.c 
init.o : init.c sim.h inst.h aux.h
	$(CC)  $(CFLAGS) init.c
float.o : float.c aux.h sim.h
	$(CC)  $(CFLAGS) float.c
io.o : io.c aux.h sim.h 
	$(CC)  $(CFLAGS) io.c 
sub_inst.o : sub_inst.c sim.h aux.h
	$(CC)  $(CFLAGS) sub_inst.c 
loader.o : loader.c sim.h inst.h 
	$(CC)  $(CFLAGS) loader.c 
main.o : main.c simdef.h inst.h aux.h sim.h
	$(CC)  $(CFLAGS) main.c
parse_oprnd.o : parse_oprnd.c inst.h sim.h
	$(CC)  $(CFLAGS) parse_oprnd.c 
print_inst.o : print_inst.c inst.h sim.h
	$(CC)  $(CFLAGS) print_inst.c 
unify.o : unify.c sim.h aux.h
	$(CC)  $(CFLAGS) unify.c 
load_work.o : load_work.c sim.h aux.h
	$(CC)  $(CFLAGS) load_work.c 
aux.o : aux.c
	$(CC)  $(CFLAGS) aux.c 
evalexp.o : evalexp.c
	$(CC)  $(CFLAGS) evalexp.c 
