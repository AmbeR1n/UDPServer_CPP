all: output receiver sender

output:
	mkdir output

check: sender_check receiver_check

receiver_check:
	g++ -g -Wall -Wextra -Wshadow -pedantic receiver.cpp ProgressBar.cpp Datagram.cpp -o output/receiver

sender_check:
	g++ -g -Wall -Wextra -Wshadow -pedantic main_sender.cpp ProgressBar.cpp Datagram.cpp Sender.cpp -o output/sender

receiver:
	g++ receiver.cpp ProgressBar.cpp Datagram.cpp -o output/receiver

sender:
	g++ main_sender.cpp ProgressBar.cpp Datagram.cpp Sender.cpp -o output/sender