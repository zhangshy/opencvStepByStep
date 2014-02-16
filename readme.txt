设置：
git remote add origin git@github.com:zhangshy/opencvStepByStep.git
ssh-keygen -t rsa -C "your_email@youremail.com"
git remote rm origin

提交：
git push origin master

在src目录下编译测试程序
g++ -ggdb `pkg-config --cflags opencv`  -D_DEBUG testMethod.cpp `pkg-config --libs opencv`
g++ -ggdb `pkg-config --cflags opencv` testMethod.cpp -fPIC -shared -o libtestMethod.so `pkg-config --libs opencv`
g++ -ggdb `pkg-config --cflags opencv` usetestMethod.cpp -L. -ltestMethod -Wl,-rpath=. `pkg-config --libs opencv`
g++ -ggdb `pkg-config --cflags opencv`  -D_DEBUG testMethod.cpp mathTest.cpp `pkg-config --libs opencv`
