CUDA_PATH=/usr/local/cuda

all:
	g++ -I${CUDA_PATH}/include -Wall -fPIC -shared -std=c++0x -o hooking.so hooking.cpp -ldl -lcuda 

clean:
	rm -f hooking.so 
