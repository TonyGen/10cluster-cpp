/* */

#ifndef CLUSTERRUN_H_
#define CLUSTERRUN_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace clusterRun {

/** Assumes 'routines' is a map in scope */
#define ROUTINE(routineClass) routines[#routineClass] = boost::shared_ptr<clusterRun::Routine> (new routineClass)

/** A routine has a function that may perform remote procedure calls, and a function that registers procedures as remote. This register function must be executed on every target machine. */
class Routine {
public:
	virtual void registerProcedures () = 0;
	virtual void operator() () = 0;
};

/** Take list of possible routines and command line args and start cluster */
int main (std::map <std::string, boost::shared_ptr<Routine> > routines, int argc, char* argv[]);

}

#endif /* CLUSTERRUN_H_ */
