all: output server client 

output:
	mkdir -p output

server:
	g++ -Wall -Wextra -pedantic server.cpp ProgressBar.cpp -o output/server

client:
	g++ -Wall -Wextra -pedantic client.cpp ProgressBar.cpp -o output/client

s_run:
	./output/server

c_run:
	./output/client

