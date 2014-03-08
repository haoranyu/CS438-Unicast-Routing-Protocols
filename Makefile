all: manager distvec linkstt

manager: manager.c
		gcc -w -o manager manager.c
distvec: distvec.c
		gcc -w -o distvec distvec.c
linkstt: linkstt.c
		gcc -w -o linkstt linkstt.c
clean:
		rm manager distvec linkstt