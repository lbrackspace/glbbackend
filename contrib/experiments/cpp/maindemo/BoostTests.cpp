#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include<boost/test/unit_test.hpp>
#include<boost/shared_ptr.hpp>
#include<Matrix.h>
#include<DemoUtils.h>
#include<ThreadManager.h>
#include<iostream>
#include<string>

BOOST_AUTO_TEST_CASE(nullptr_on_delete) {
    double *dptr = new double[10];
    delete dptr;
    dptr = NULL;
    BOOST_CHECK(dptr == NULL);
}

BOOST_AUTO_TEST_CASE(decrementing_shared_ptr_refferences) {
    shared_ptr<double> dptr(new double);
    *dptr = 3.14159;
    vector< shared_ptr<double> > double_ptrs;
    BOOST_CHECK(dptr.use_count() == 1);
    {
        shared_ptr<double> p1(dptr);
        shared_ptr<double> p2(dptr);
        shared_ptr<double> p3(dptr);
        BOOST_CHECK(dptr.use_count() == 4);
    }
    // Since we exited a code block the ptrs should have unwinded to 1 refcount;
    BOOST_CHECK(dptr.use_count() == 1);

    double_ptrs.push_back(dptr); // Add a reference
    BOOST_CHECK(dptr.use_count() == 2);
    double_ptrs.push_back(dptr); // Add another reference
    BOOST_CHECK(dptr.use_count() == 3);

    // Call the smart_ptr destructors by clearing the vector
    double_ptrs.clear();
    // This should return ref counts back to 1
    BOOST_CHECK(dptr.use_count() == 1);
    // When this code block exits dptr will have 0 references and will be
    // freed. To bad I don't have a way to test this
}