
#include "cluster.h"
#include <10util/unit.h>
#include <set>
#include <cstdlib> // srand

remote::Module _cluster::module ("cluster", "cluster/cluster.h");

std::vector<remote::Host> cluster::clients;
std::vector<remote::Host> cluster::servers;

/** Clients and servers in cluster, removing duplicates */
std::set<remote::Host> cluster::machines() {
	std::set<remote::Host> machines;
	for (unsigned i = 0; i < clients.size(); i++) machines.insert (clients[i]);
	for (unsigned i = 0; i < servers.size(); i++) machines.insert (servers[i]);
	return machines;
}

void _cluster::setMembers (std::vector<remote::Host> clients, std::vector<remote::Host> servers) {
	cluster::clients = clients;
	cluster::servers = servers;
}

/** Broadcast the set of machines in the cluster to all the machines so they know about each other */
void cluster::members (std::vector<remote::Host> clients, std::vector<remote::Host> servers) {
	_cluster::setMembers (clients, servers);
	std::set<remote::Host> hosts = machines();
	for (std::set<remote::Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
		remote::eval (*it, remote::thunk (MFUN(_cluster,setMembers), clients, servers));
}

void _cluster::load (library::Libname libname) {library::load_ (libname);}

/** Tell all machines in cluster to load given library */
void cluster::load (library::Libname libname) {
	library::load_ (libname);
	std::set<remote::Host> hosts = cluster::machines();
	for (std::set<remote::Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
		remote::eval (*it, thunk (MFUN(_cluster,load), libname));
}

void _cluster::setRandomSeed (int seed) {srand (seed);}

/** Tell all machines in cluster to reset its random number generator with given seed */
void cluster::seedRandom (int seed) {
	std::set<remote::Host> hosts = cluster::machines();
	for (std::set<remote::Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
		remote::eval (*it, thunk (MFUN(_cluster,setRandomSeed), seed));
}

static unsigned nextServerIdx;

/** Return the next cluster server in cycle */
remote::Host cluster::someServer () {
	unsigned c = servers.size();
	if (c == 0) throw std::runtime_error ("no servers in cluster");
	if (nextServerIdx >= c) nextServerIdx = 0;
	return servers [nextServerIdx ++];
}

/** Return the next N cluster servers in cycle */
std::vector<remote::Host> cluster::someServers (unsigned n) {
	return repeat (n, someServer);
}

static unsigned nextClientIdx;

/** Return the next cluster server in cycle */
remote::Host cluster::someClient () {
	unsigned c = clients.size();
	if (c == 0) throw std::runtime_error ("no clients in cluster");
	if (nextClientIdx >= c) nextClientIdx = 0;
	return clients [nextClientIdx ++];
}

/** Return the next N cluster clients in cycle */
std::vector<remote::Host> cluster::someClients (unsigned n) {
	return repeat (n, someClient);
}
