all: output server client 
	@echo all

output:
	mkdir -p output

server:
	@echo Building server...
	g++ server.cpp -o output/server

client:
	@echo Building client...
	g++ client.cpp -o output/client

s_run:
	./output/server

c_run:
	./output/client
