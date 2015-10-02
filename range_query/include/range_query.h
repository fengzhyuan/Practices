/** 
 * \file   range_query.h
 * Define class Generator and QueryHandler 
 * \author fengzh
 * \copyright GNU Public License.
 */

#ifndef M_RANGE_QUERY_H_
#define	M_RANGE_QUERY_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>

#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "kdtree++/kdtree.hpp"

using namespace std;

namespace RangeQuery {
    
    /**
     * \class Generator
     *  
     * \brief generate random data;
     * 
     */
class Generator {

public:
    Generator(string path);
    ~Generator() {}
    bool is_open();

private:
    Generator();
    string path_;    /**< log file path */
};

/** kd-tree node structure; applied kdtree++ library; see "kdtree++/kdtree.hpp"
 */
struct KDTreeNode {
    typedef long value_type;
    /** \internal ip/cpu_id/timestamp; used inet_pton() to
     * convert string format IP addreess into long type
     * for kd-tree construction/query
     */
    long val_[3];    
    long  index_;    /**< node index */
    long  usage_;    /**< cpu useage */

    value_type operator [] (size_t n) const {
        return val_[n];
    }
};


/**
 * \class Monitor 
 * Class Monitor simulate a monitoring system with the following functionalities
 * 
 * each kd-tree performs range query and return the combined 
 * query result
 */
class QueryHandler {

public:
    typedef KDTree::KDTree<3, KDTreeNode> tree_type;
    typedef shared_ptr<tree_type> tree_type_ptr;
    QueryHandler(string);
    ~QueryHandler();

    bool is_open();
    void query(KDTreeNode &, KDTreeNode &);
    void query(string);
    bool is_valid_query(string &);
    bool is_valid_time(string &);
    void helper();

private:
    const static size_t  kQuerySize = 6;            /**< correct # of query segments */
    const static size_t kMaxTreeNum = 1e5;      /**< maximum allowed number of kd-trees */
    const static size_t kMaxTreeSize= 1e5;      /**< maximum allowed kd-tree size */
    string path_;                    /**< path of log file */
    vector<string> q_query_;             /**< parsed paramaters from query */
    vector<long>   q_param_;             /**< parsed paramaters from query */
    vector<tree_type_ptr> tree_cluster_; /**< kd-tree clusters */
};

long time2stamp(int yyyy, int mm, int dd, int h, int m);
void stamp2time(time_t epoch, int &yy, int &mm, int &dd, int &h, int &m);
}
#endif	/* M_RANGE_QUERY_H_ */

