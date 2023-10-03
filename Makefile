all: output server client 

output:
	mkdir -p output

server:
	g++ -O2 -Wall -Wextra -Wshadow -pedantic server.cpp ProgressBar.cpp DatagramParser.cpp -o output/server

client:
	g++ -O2 -Wall -Wextra -Wshadow -pedantic client.cpp ProgressBar.cpp DatagramParser.cpp -o output/client

s_run:
	./output/server

c_run:
	./output/client

