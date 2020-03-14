# All Targets
all: splflix

# Tool invocations
# Executable "splflix" depends on the files Action.o, Watchable.o, User.o, Main.o, Session.o
splflix: bin/Action.o bin/Watchable.o bin/User.o bin/Main.o bin/Session.o
	@echo 'Building target: splflix'
	@echo 'Invoking: C++ Linker'
	g++ -o bin/splflix bin/Action.o bin/Watchable.o bin/User.o bin/Main.o bin/Session.o 
	@echo 'Finished building target: splflix'
	@echo ' '

# Depends on the source and header files
bin/Action.o: src/Action.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Action.o src/Action.cpp

# Depends on the source and header files
bin/Watchable.o: src/Watchable.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Watchable.o src/Watchable.cpp

# Depends on the source and header files
bin/User.o: src/User.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/User.o src/User.cpp

# Depends on the source and header files
bin/Main.o: src/Main.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Main.o src/Main.cpp

# Depends on the source and header files
bin/Session.o: src/Session.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Session.o src/Session.cpp




#Clean the build directory
clean:
	rm -f bin/*

