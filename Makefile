all: manager distvec linkstate

manager: manager.cpp
		g++ -w -pthread -o manager manager.cpp
distvec: distvec.cpp
		g++ -w -pthread -o distvec distvec.cpp
linkstate: linkstate.cpp
		g++ -w -pthread -o linkstate linkstate.cpp
clean:
		rm manager distvec linkstate