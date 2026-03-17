
CFLAGS = -c -O
OBJS = builtin.o meta.o file.o init_branch.o tio.o structure.o name.o buffer.o other.o time.o compare.o substuff.o arith.o saverest.o token.o stlookup.o
HS = builtin.h ../sim.h ../aux.h

builtin: $(OBJS) 
	ld -o builtin -r $(OBJS) -lm
builtin.o: $(HS) 
	cc $(CFLAGS) builtin.c
file.o: $(HS) file.c
	cc $(CFLAGS) file.c
substuff.o: $(HS)
	cc $(CFLAGS) substuff.c
buffer.o: $(HS) buffer.c
	cc $(CFLAGS) buffer.c
other.o: $(HS) other.c
	cc $(CFLAGS) other.c
init_branch.o: builtin.h init_branch.c
	cc $(CFLAGS) init_branch.c
meta.o: $(HS) meta.c
	cc $(CFLAGS) meta.c
name.o: $(HS) name.c
	cc $(CFLAGS) name.c
structure.o: $(HS)  structure.c
	cc $(CFLAGS) structure.c
tio.o: $(HS) tio.c
	cc $(CFLAGS) tio.c
time.o: $(HS) time.c
	cc $(CFLAGS) time.c
compare.o: $(HS) compare.c
	cc $(CFLAGS) compare.c
arith.o: arith.c $(HS)
	cc $(CFLAGS) arith.c
saverest.o: saverest.c $(HS)
	cc $(CFLAGS) saverest.c
token.o: $(HS) token.c
	cc $(CFLAGS) token.c
stlookup.o: $(HS) stlookup.c
	cc $(CFLAGS) stlookup.c
