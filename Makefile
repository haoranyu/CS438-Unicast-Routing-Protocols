all: manager distvec linkstate

manager: manager.cpp
		g++ -w -o manager manager.cpp
distvec: distvec.cpp
		g++ -w -o distvec distvec.cpp
linkstate: linkstate.cpp
		g++ -w -o linkstate linkstate.cpp
clean:
		rm manager distvec linkstate