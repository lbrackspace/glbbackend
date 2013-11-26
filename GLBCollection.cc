#include "GLBCollection.hh"
#include "GlbContainer.hh"
#include <stdio.h>
#include <boost/unordered_map.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

GLBCollection::GLBCollection()
{
  glbMap = boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> >();

}

void GLBCollection::add(std::string cname, boost::shared_ptr<GlbContainer>& glb)
{
  { // Scope for glbMapLock
      boost::lock_guard<boost::shared_mutex> mapLock(glbMapMutex);
      boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
      if (it != glbMap.end()) {
          throw GLBNotFoundException();
      }
      glbMap[cname] = glb;
  } // releasing glbMapLock;
}

void GLBCollection::remove(std::string key)
{
  glbMap.erase(key);
}

boost::shared_ptr<GlbContainer> GLBCollection::get(std::string key)
{
  return glbMap[key];
}

int size(){
  return glbMap.size();
}

GLBCollection::~GLBCollection()
{

}

GLBCollection glbCollection;

