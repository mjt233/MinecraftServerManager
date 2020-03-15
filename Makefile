compile : manager.cpp
	g++ manager.cpp -o manager -lpthread -lssl -lcrypto -std=c++11
	@echo "compile complete"
compile-debug : manager.cpp
	g++ -g manager.cpp -o manager -lpthread -lssl -lcrypto -std=c++11
	@echo "compile complete"
clean :
	@rm manager
run :
	@./manager
check-net:
	@netstat -anlp | grep tcp | grep 6636