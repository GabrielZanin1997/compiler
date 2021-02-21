

CC = gcc

FLEX = flex

BISON = bison -d -v

OBJS = main.o util.o symtab.o analyze.o globals.o lex.yy.o scanner.tab.o intermediateCode.o #Add o codigo intermediario dps 

CFLAGS = 

all: cms end

cms: $(OBJS)
	$(CC) $(CFLAGS) -o cms $(OBJS)


main.o: main.c globals.h util.h parse.h scan.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c main.c

# Quando o bison é executado, ele gera scanner.tab.h em tempo de execução, portanto, ele muda o globals.h.
# É por isso que fazemos um toque, para indicar a mudança em globals.h. Do que chamamos de make novamente.
# Assim, tudo o que depende de globals.h (que foi alterado por scanner.tab.h) é recompilado.
scanner.tab.o: scanner.y
	
	$(BISON) scanner.y
	$(CC) -c scanner.tab.c
	touch globals.h 
	make
	

# Mudanças no lexer resultam em mudanças no analisador (.y). Porque lex.yy.c é criado em tempo de execução
# Fazemos um toque em .y e o recompilamos (recompilando também todas as dependências de
# globals.h que depende do .y (scanner.tab.h))
lex.yy.o: scanner.l
	$(FLEX) scanner.l
	$(CC) -c lex.yy.c
	touch scanner.y
	make

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -c globals.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h globals.h symtab.h
	$(CC) $(CFLAGS) -c analyze.c

intermediateCode.o: intermediateCode.c intermediateCode.h globals.h
	$(CC) $(CFLAGS) -c intermediateCode.c

end: 
	mv cms ../


clean:
	rm main.o util.o symtab.o analyze.o globals.o
