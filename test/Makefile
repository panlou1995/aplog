server:server.cpp
	g++ -g -fno-omit-frame-pointer server.cpp -o server -I ../include/ -L../  -laplog -lboost_thread -lboost_system -lboost_filesystem  -lpthread -std=c++11

client:client.cpp
	g++ -g -fno-omit-frame-pointer client.cpp -o client -I ../include/ -L../  -laplog -lboost_thread -lboost_system -lboost_filesystem  -lpthread -std=c++11

clean:
	unset LD_PRELOAD
	rm -f test*
	rm -f client
	rm -f server

clean_server:
	rm -f server

clean_client:
	rm -f client
run:
	export LD_PRELOAD=/usr/lib/gcc/x86_64-linux-gnu/5.4.0/libasan.so
	./test

