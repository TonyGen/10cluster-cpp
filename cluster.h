/* Set of machines in my cluster. */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <set>
#include <vector>
#include <10util/util.h>
#include <remote/remote.h>

namespace cluster {

/** Client machines in cluster. A machine may be both a client and server machine */
extern std::vector<remote::Host> clients;

/** Server machines in cluster. A machine may be both a client and server machine */
extern std::vector<remote::Host> servers;

/** All machines in cluster (clients and servers), removing duplicates */
std::set<remote::Host> machines();

/** Start listening for network messages so we can be told the set of machines in my cluster by the controller.
 * Return listener thread, which you may terminate */
boost::shared_ptr <boost::thread> listen (unsigned short port = remote::DefaultPort);

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

}

#endif /* CLUSTER_H_ */
