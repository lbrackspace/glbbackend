#include "GLBCollection.hh"
#include "GlbContainer.hh"
#include <stdio.h>
#include <boost/unordered_map.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

GLBCollection::GLBCollection() {
    glbMap = boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> >();

}

void GLBCollection::add(std::string cname, boost::shared_ptr<GlbContainer>& glb) {
    { // Scope for glbMapLock
        boost::lock_guard<boost::shared_mutex> mapLock(glbMapMutex);
        boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
        if (it != glbMap.end()) {
            throw GLBExistsException();
        }
        glbMap[cname] = glb;
    } // releasing glbMapLock;
}

void GLBCollection::remove(std::string cname) {
    boost::lock_guard<boost::shared_mutex> mapLock(glbMapMutex);
    boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
    if (it == glbMap.end()) {
        throw GLBNotFoundException();
    }
    glbMap.erase(cname);
}

boost::shared_ptr<GlbContainer> GLBCollection::get(std::string cname) {
    boost::shared_lock<boost::shared_mutex> mapLock(glbMapMutex);
    boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
    if (it == glbMap.end()) {
        throw GLBNotFoundException();
    }
    return glbMap[cname];
}

int size() {
    return glbMap.size();
}

GLBCollection::~GLBCollection() {

}

GLBCollection glbCollection;

