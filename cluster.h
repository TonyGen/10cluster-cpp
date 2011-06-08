/* Register set of machines in my cluster. */

#pragma once

#include <vector>
#include <remote/remote.h>
#include <10util/library.h>

namespace cluster {

enum Role {CLIENT, SERVER, BOTH};

struct Member {
	remote::Host host;
	Role role;
	Member (remote::Host host, Role role) : host(host), role(role) {}
};

/** All machines in cluster including self. Each machine may have a client and/or server role. */
extern std::vector<Member> members;

/** Join cluster of machines where given host is one of them. Join as given role. */
void join (Role, remote::Host);

/** Remove self from cluster of machines */
void leave ();

/** Tell all machines in cluster to reset its random number generator with given seed */
void seedRandom (int seed);

/** Return the next client in cluster cycle */
remote::Host someClient ();

/** Return the next N clients in cluster cycle */
std::vector<remote::Host> someClients (unsigned n);

/** Return the next server in cluster cycle */
remote::Host someServer ();

/** Return the next N servers in cluster cycle */
std::vector<remote::Host> someServers (unsigned n);

}

inline std::ostream& operator<< (std::ostream& out, const cluster::Member& x) {
	out << x.host;
	if (x.role == cluster::CLIENT) out << "/c";
	if (x.role == cluster::SERVER) out << "/s";
	return out;
}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, cluster::Member & x, const unsigned version) {
	ar & x.host;
	ar & x.role;
}

}}

namespace _cluster {

extern remote::Module module;
void newMember (cluster::Member);
void addMember (cluster::Member);
void removeMember (remote::Host);
void setRandomSeed (int seed);

}
