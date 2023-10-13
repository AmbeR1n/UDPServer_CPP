all: output reciever sender

output:
	mkdir output	

reciever:
	g++ -O2 -Wall -Wextra -Wshadow -pedantic reciever.cpp ProgressBar.cpp Datagram.cpp -o output/reciever

sender:
	g++ -O2 -Wall -Wextra -Wshadow -pedantic sender.cpp ProgressBar.cpp Datagram.cpp -o output/sender