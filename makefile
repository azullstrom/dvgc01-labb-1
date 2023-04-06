all:
	gcc -Wall -c keytoktab.c
	gcc -Wall -c parser.c
	gcc -Wall -c driver.c
	gcc -Wall -o parser driver.c parser.c keytoktab.c
	./parser > driverV2.out