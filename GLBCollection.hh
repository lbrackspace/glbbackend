#ifndef GLBCollection_HH_
#define GLBCollection_HH_

#include "GlbContainer.hh"
#include <stdio.h>
#include <boost/unordered_map.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>


class GLBNotFoundException : std::runtime_error
{
public:
  GLBNotFoundException() : std::runtime_error("GLB Not Found"){}
};


class GLBExistsException : std::runtime_error
{
public:
  GLBExistsException() : std::runtime_error("GLB Already Exists"){}
};


class GLBCollection
{
private:
  boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> > glbMap;
public:
  GLBCollection();
  void add(std::string key, boost::shared_ptr<GlbContainer>& glb);
  void remove(std::string key);
  boost::shared_ptr<GlbContainer> get(std::string key);
  int size();
  virtual ~GLBCollection();
};

extern GLBCollection glbCollection;

#endif /* GLBCollection_HH_ */
