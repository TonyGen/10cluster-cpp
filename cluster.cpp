/* */

#include "cluster.h"
#include <set>

std::vector<remote::Host> cluster::clients;
std::vector<remote::Host> cluster::servers;

namespace _cluster {
void setMembers (std::vector<remote::Host> clients, std::vector<remote::Host> servers) {
	cluster::clients = clients;
	cluster::servers = servers;
}
}

static void registerProcedures () {
	REGISTER_PROCEDURE2 (_cluster::setMembers);
}

/** Start listening for network messages so we can be told the set of machines in my cluster by the controller */
void cluster::listen() {
	registerProcedures();
	remote::listen();
}

/** Broadcast the set of machines in the cluster to all the machines so they know about each other */
void cluster::members (std::vector<remote::Host> clients, std::vector<remote::Host> servers) {
	_cluster::setMembers (clients, servers);
	std::set<remote::Host> machines;
	for (unsigned i = 0; i < clients.size(); i++) machines.insert (clients[i]);
	for (unsigned i = 0; i < servers.size(); i++) machines.insert (servers[i]);
	for (std::set<remote::Host>::iterator it = machines.begin(); it != machines.end(); ++it) {
		boost::function0<void> install = boost::bind (PROCEDURE2 (_cluster::setMembers), clients, servers);
		remote::remotely (*it, install);
	}
}

static unsigned nextServerIdx;

/** Return the next cluster server in cycle */
remote::Host cluster::someServer () {
	unsigned c = servers.size();
	if (c == 0) throw std::runtime_error ("no servers in cluster");
	if (nextServerIdx >= c) nextServerIdx = 0;
	return servers [nextServerIdx ++];
}
