/* Set of machines in my cluster. */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <vector>
#include <util/util.h>
#include <remote/remote.h>

namespace cluster {

extern std::vector<remote::Host> clients;
extern std::vector<remote::Host> servers;

/** Start listening for network messages so we can be told the set of machines in my cluster by the controller */
void listen();

/** Broadcast the set of machines in the cluster to all the machines so they know about each other */
void members (std::vector<remote::Host> clients, std::vector<remote::Host> servers);

/** Return the next cluster server in cycle */
remote::Host someServer();

/** Return the next N cluster servers in cycle */
std::vector<remote::Host> someServers (unsigned n);

/** Return the next cluster client in cycle */
remote::Host someClient();

/** Return the next N cluster clients in cycle */
std::vector<remote::Host> someClients (unsigned n);

/** Start procedure thread on N clients, recycling clients if necessary. Use supervisor thread so if any client threads die, all will dies, and this main thread will receive an exception. The client enumeration is supplied to each client procedure.
 * P type: void P (unsigned), string P.serialize(), string P.toString() */
template <template <typename,typename> class P> std::vector< std::pair< remote::Host, boost::function0<void> > > clientActs (unsigned numClients, P<void,unsigned> procedure) {
	std::vector<remote::Host> hosts = cycle (numClients, cluster::clients);
	std::vector< std::pair< remote::Host, boost::function0<void> > > hostActs;
	for (unsigned i = 0; i < hosts.size(); i++) {
		boost::function0<void> action = boost::bind (procedure, i);
		hostActs.push_back (std::make_pair (hosts[i], action));
	}
	return hostActs;
}

}

#endif /* CLUSTER_H_ */
