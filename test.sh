g++ -ggdb `pkg-config --cflags opencv` src/testMethod.cpp -fPIC -shared -o out/libtestMethod.so `pkg-config --libs opencv`
g++ -ggdb `pkg-config --cflags opencv` src/usetestMethod.cpp -Lout/ -ltestMethod -Wl,-rpath=. -o out/usetestMethod `pkg-config --libs opencv`
