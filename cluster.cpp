
#include "cluster.h"
#include <10util/unit.h>
#include <set>
#include <cstdlib> // srand

remote::Module _cluster::module ("cluster", "cluster/cluster.h");

/** All machines in cluster including self. Each machine may have a client and/or server role. */
std::vector<cluster::Member> cluster::members;

void _cluster::addMember (cluster::Member newMember) {
	for (std::vector<cluster::Member>::iterator m = cluster::members.begin(); m != cluster::members.end(); ++m)
		if (m->host == newMember.host) {m->role = newMember.role; return;}
	cluster::members.push_back (newMember);
}

void _cluster::removeMember (remote::Host host) {
	for (std::vector<cluster::Member>::iterator m = cluster::members.begin(); m != cluster::members.end(); ++m)
		if (m->host == host) {cluster::members.erase (m); return;}
}

/** Add new member to cluster, tell new member all existing members plus itself, and tell existing members about new member */
void _cluster::newMember (cluster::Member newMember) {
	addMember (newMember); // Add member now so when adding again below it won't mess up iterator
	for (std::vector<cluster::Member>::iterator m = cluster::members.begin(); m != cluster::members.end(); ++m) {
		remote::eval (m->host, remote::thunk (MFUN(_cluster,addMember), newMember));
		remote::eval (newMember.host, remote::thunk (MFUN(_cluster,addMember), *m));
	}
}

/** Join cluster of machines where given host is one of them. Join as given role. */
void cluster::join (Role role, remote::Host host) {
	remote::eval (host, remote::thunk (MFUN(_cluster,newMember), Member (remote::thisHost(), role)));
}

/** Join cluster where I am the first one or others have already joined to me so I know who to tell */
void cluster::join (Role role) {
	join (role, remote::thisHost());
}

/** Remove self from cluster of machines */
void leave () {
	for (std::vector<cluster::Member>::iterator m = cluster::members.begin(); m != cluster::members.end(); ++m)
		remote::eval (m->host, remote::thunk (MFUN(_cluster,removeMember), remote::thisHost()));
	cluster::members.clear();
}

void _cluster::setRandomSeed (int seed) {srand (seed);}

/** Tell all machines in cluster to reset its random number generator with given seed */
void cluster::seedRandom (int seed) {
	for (std::vector<cluster::Member>::iterator m = cluster::members.begin(); m != cluster::members.end(); ++m)
		remote::eval (m->host, thunk (MFUN(_cluster,setRandomSeed), seed));
}

/** Increment i wrapping when reaching size */
static unsigned nextWrap (unsigned size, volatile unsigned * i) {
	(*i) ++;
	if ((*i) >= size) *i = 0;
	return *i;
}

static volatile unsigned nextServerIdx = -1;

/** Return the next cluster server in cycle */
remote::Host cluster::someServer () {
	if (members.empty()) throw std::runtime_error ("no cluster");
	for (unsigned i = 0; i < members.size(); i++) {
		Member m = members [nextWrap (members.size(), &nextServerIdx)];
		if (m.role == SERVER || m.role == BOTH) return m.host;
	}
	throw std::runtime_error ("no servers in cluster");
}

/** Return the next N cluster servers in cycle */
std::vector<remote::Host> cluster::someServers (unsigned n) {
	return repeat (n, someServer);
}

static volatile unsigned nextClientIdx = -1;

/** Return the next cluster server in cycle */
remote::Host cluster::someClient () {
	if (members.empty()) throw std::runtime_error ("no cluster");
	for (unsigned i = 0; i < members.size(); i++) {
		Member m = members [nextWrap (members.size(), &nextClientIdx)];
		if (m.role == CLIENT || m.role == BOTH) return m.host;
	}
	throw std::runtime_error ("no clients in cluster");
}

/** Return the next N cluster clients in cycle */
std::vector<remote::Host> cluster::someClients (unsigned n) {
	return repeat (n, someClient);
}
