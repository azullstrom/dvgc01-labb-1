p2:
	gcc -Wall -c keytoktab.c
	gcc -Wall -c parser.c
	gcc -Wall -c driver.c
	gcc -Wall -o parser driver.c parser.c keytoktab.c
	./parser > driverV2.out
p3test:
	gcc -Wall -c Dlexer.c
	gcc -Wall -c lexer.c
	gcc -Wall -o tlexer Dlexer.c lexer.c keytoktab.c
	./tlexer < testok1.pas > Dlexer.out
p3:
	gcc -Wall -c Dlexer.c
	gcc -Wall -c lexer.c
	gcc -Wall -o parser driver.c parser.c keytoktab.c lexer.c
	./parser < testok1.pas > DriverV3.out
p4test:
	gcc -Wall -c Dsymtab.c
	gcc -Wall -c symtab.c
	gcc -Wall -o tsymtab Dsymtab.c symtab.c keytoktab.c
	./tsymtab > Dsymtab.out
p4:
	gcc -Wall -o parser driver.c parser.c keytoktab.c lexer.c symtab.c
	./parser < testok1.pas > DriverV4.out
p5test:
	gcc -Wall -c Doptab.c
	gcc -Wall -c optab.c
	gcc -Wall -o toptab Doptab.c optab.c keytoktab.c
	./toptab > Doptab.out
p5:
	gcc -Wall -o parser driver.c parser.c keytoktab.c lexer.c symtab.c optab.c
	./parser < testok1.pas > DriverV5.out