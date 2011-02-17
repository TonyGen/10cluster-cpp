Install dependent library first:

- [remote](https://github.com/TonyGen/remote-cpp)

Download and remove '-ccp' suffix:
	git clone git://github.com/TonyGen/cluster-cpp.git cluster
	cd cluster

Build library `libcluster.a`:
	scons

Install library in `/usr/local/lib` and header files in `/usr/local/include/cluster`:
	sudo scons install
