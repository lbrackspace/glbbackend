#include "GlbContainer.h"

#include<boost/unordered_map.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/thread.hpp>
#include<string>

boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> > glbMap;
boost::shared_mutex glbMapLock;
