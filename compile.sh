gcc server.c -o server -lenet
gcc client.c -o client -lraylib -lenet -lm -lpthread -ldl
