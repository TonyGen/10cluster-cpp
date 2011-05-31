Install dependent library first:

- [remote](https://github.com/TonyGen/remote-cpp)
- readline

Download and remove '-ccp' suffix:

	git clone git://github.com/TonyGen/cluster-cpp.git cluster
	cd cluster

Build library `libcluster.so` and program `cluster`:

	scons

Install library in `/usr/local/lib`, header files in `/usr/local/include/cluster`, and program in /usr/local/bin:

	sudo scons install

Run programs help to see usage:

	cluster help

Example Run: Execute `test1 (["T", "2"])` expression from `tests` library with access to cluster of 3 hosts. Seed random number generated with 42 on all hosts before executing expression.

	host1> mongoTest host1 worker
	host2> mongoTest host2 worker
	host3> mongoTest host3 run host1,host2,host3 42 tests test1 T 2
