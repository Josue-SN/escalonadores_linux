all:
	gcc -o setpriority setpriority.c -lpthread -Wall

clean:
	rm setpriority deadline
