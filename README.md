Install dependent library first:

- [10remote](https://github.com/TonyGen/10remote-cpp)

Download and remove '-ccp' suffix:

	git clone git://github.com/TonyGen/10cluster-cpp.git 10cluster
	cd 10cluster

Build library `lib10cluster.so`:

	scons

Install library in `/usr/local/lib` and header files in `/usr/local/include/10cluster`:

	sudo scons install
