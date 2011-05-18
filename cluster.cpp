
#include "cluster.h"
#include <10util/unit.h>
#include <set>

std::vector<remote::Host> cluster::clients;
std::vector<remote::Host> cluster::servers;

/** Clients and servers in cluster, removing duplicates */
std::set<remote::Host> cluster::machines() {
	std::set<remote::Host> machines;
	for (unsigned i = 0; i < clients.size(); i++) machines.insert (clients[i]);
	for (unsigned i = 0; i < servers.size(); i++) machines.insert (servers[i]);
	return machines;
}

namespace _cluster {
Unit setMembers (std::vector<remote::Host> clients, std::vector<remote::Host> servers) {
	cluster::clients = clients;
	cluster::servers = servers;
	return unit;
}
}

/** Broadcast the set of machines in the cluster to all the machines so they know about each other */
void cluster::members (std::vector<remote::Host> clients, std::vector<remote::Host> servers) {
	_cluster::setMembers (clients, servers);
	std::set<remote::Host> hosts = machines();
	for (std::set<remote::Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
		remote::eval (*it, thunk (FUN(_cluster::setMembers), clients, servers));
}

/** Tell all machines in cluster to load given library */
void cluster::load (library::Libname libname) {
	library::load_ (libname);
	std::set<remote::Host> hosts = cluster::machines();
	for (std::set<remote::Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
		remote::eval (*it, thunk (FUN(library::load_), libname));
}

static void registerProcedures () {
	registerFun (FUN(_cluster::setMembers));
	registerFun (FUN(library::load_));
}

INITIALIZE (
	registerProcedures();
)

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
