all: output server client 

output:
	mkdir -p output

server:
	g++ server.cpp -o output/server

client:
	g++ client.cpp ProgressBar.cpp -o output/client

s_run:
	./output/server

c_run:
	./output/client

