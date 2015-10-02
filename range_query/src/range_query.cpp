/** 
 * \file   range_query.cpp
 * Definition of class member Generator/QueryHandler and other functions
 * \author fengzh
 * \copyright GNU Public License.
 */

#include "range_query.h"

using namespace RangeQuery;
/** used for generating log file with random values
 * 
 */
Generator::Generator(string path) : path_(path) {}

/*!
 * check if the file is successfully created; if true then
 * generate a log file with format:
 * 
 * @return false if open file failed. o.w. return true
 */
bool Generator::is_open() {
   
    FILE *pfile = fopen(path_.c_str(), "w+");
    if (!pfile) {
        cout << "create file failed." << endl;
        return false;
    }
    cout << "generate LOG file:[" << path_ << "]" << endl;
    
    srand(time(NULL));
    
    for (int h = 0; h < 24; ++h) {       // hour
        for (int m = 0; m < 60; ++m) {   // min
            
            long stamp = time2stamp(2012, 12, 12, h, m);
            for (size_t idx = 1; idx <= 1000; ++idx) {   // machine id
                
                size_t i = idx/256, j = idx%256;
                string ip = "192.168." + to_string(i) + "." + to_string(j);
                int u0 = rand()%101, u1 = rand()%101;
                
                fprintf(pfile,"%ld %s 0 %d\n", stamp, ip.c_str(), u0);
                fprintf(pfile,"%ld %s 1 %d\n", stamp, ip.c_str(), u1);
            }
        }
    }
    fclose(pfile);
    cout << endl << "done" << endl;
    return true;
}

/**
 * 
 * @param _str LOG file path to be analyzed
 */
QueryHandler::QueryHandler(string path) : path_(path) { 
    tree_type_ptr new_tree(new tree_type());
    tree_cluster_.push_back(new_tree);
}

QueryHandler::~QueryHandler() {}

/**
 * if open file succeeded then construct kd-trees, each tree
 * is allowed to have the MAX_TREE_SIZE nodes;
 * 
 * @return true if log file is opened
 */
bool QueryHandler::is_open() {
    
    FILE *pfile = fopen(path_.c_str(),"r");
    if (!pfile) {
        cout << "error, cannot open file" << endl;
        return false;
    }
    
    long stamp;
    char ip[INET_ADDRSTRLEN];
    int cid, u;
    
    size_t indx = 0;
    struct sockaddr_in sa;
    vector<KDTreeNode> nodes;
    
    cout << "===parse LOG file===" << endl;
    time_t t_start = clock(), t_end;
    while (fscanf(pfile, "%ld %s %d %d\n", &stamp, ip, &cid, &u) != EOF) {
        
        inet_pton(AF_INET, ip, &sa.sin_addr);
        KDTreeNode node;
        node.index_ = indx++;
        node.val_[0] = (uint32_t) sa.sin_addr.s_addr;    /* ip       */
        node.val_[1] = cid;                              /* cpu id   */
        node.val_[2] = stamp;                            /* timestamp*/
        node.usage_  = u;                                /* usage    */
        nodes.push_back(node);
    }
    fclose(pfile);
    if (nodes.empty()) {
        cout << "invalid file format" << endl;
        return false;
    }
    size_t count = 0; 
    for (size_t i = 0; i < nodes.size(); ++i, ++count) {
        if (count >= kMaxTreeSize) {
            // check cluster size to avoid mem overflow
            if (tree_cluster_.size() >= kMaxTreeNum) {
                cout << "exceed maximum tree cluster size" << endl;
                return false;
            }
            tree_type_ptr new_tree(new tree_type());
            tree_cluster_.push_back(new_tree);
            count = 0;
        }
        tree_cluster_.back()->insert(nodes[i]);        
    }
    
    for (size_t i = 0; i < tree_cluster_.size(); ++i)
        tree_cluster_[i]->optimize();
    
    t_end = clock();
    cout << "[#t:" << tree_cluster_.size() << "]"
         << "elapsed time (s): "
         << (double)(t_end-t_start)/(double)CLOCKS_PER_SEC 
         << endl;
    helper();
    cout << "===done==="<< endl << endl;
    
    return true;
}

/**
 * a function showing how to use the query sentence
 */
void QueryHandler::helper() {
    string info = 
            "Command: 1. QUERY [query] 2. EXIT; commands are CASE SENSITIVE\n"
            "query usages of cpu[cpu_id] of machine[id] within the time range\n"
            "[query]: ip cpu_id yyyy-mm-dd hh:mm yyyy-mm-dd hh:mm"
            ;
    cout << info << endl;
}

/**
 * validate time string from parsed input
 * @param query query string containing time stamp 
 * @return bValid = true if valid
 */
bool QueryHandler::is_valid_time(string &query) {
    stringstream ss(query);
    string str;
    bool m_valid = true;
    
    vector<string> mtime;
    while (getline(ss, str, ' ')) {
        
        stringstream ss1(str);
        while (getline(ss1, str, '-')) {
            mtime.push_back(str);
        }
        
        getline(ss, str, ' ');
        stringstream ss2(str);
        while (getline(ss2, str, ':')) {
            mtime.push_back(str);
        }
    }
    /*
     * check if the length and range of digit is valid
     * use "strptime()" to further validate the time at the next step
     */
    if (mtime.size() == 5 &&
        mtime[0].length() == 4 && mtime[1].length() == 2 && 
        mtime[2].length() == 2 && mtime[3].length() == 2 &&
        mtime[4].length() == 2){
        int year = atoi(mtime[0].c_str());
        int month= atoi(mtime[1].c_str());
        int day  = atoi(mtime[2].c_str());
        int hour = atoi(mtime[3].c_str());
        int mint = atoi(mtime[4].c_str());
        if (!(year > 1900 && month >= 1 && month <= 12 && day >= 1 &&
            day <= 31 && hour >= 0 && hour <= 23 && mint >= 0 && mint <= 59) )
            m_valid = false;
    }
    else {
        m_valid = false;
    }
        
    if (!m_valid) {
        cout << "invalid time format" << endl;
    }
    return m_valid;
}

/**
 * parse input query and return true if its valid
 * @param query input query
 * @return m_valid=true if valid
 */
bool QueryHandler::is_valid_query(string& query) {
    /** assistance string*/
    string m_format = "format: ip cid YYYY-MM-DD HH:MM YYYY-MM-DD HH:MM";
    stringstream ss(query);     /** parse by space */
    string tmp;                 /** parse by space */
    struct sockaddr_in sa;      /** parse ip */
    bool m_valid = true;        /** return value */
    
    q_query_.clear();
    q_param_.clear();
    
    while (getline(ss, tmp, ' '))
        q_query_.push_back(tmp);
    
    /** validate # of params; nQuerySeg is pre-defined as 6 */
    if (q_query_.size() != kQuerySize) {
        cout << "invalid query format. " << m_format << endl;
        return false;
    }
    
    /** validate ip address */
    if (m_valid) {
        m_valid = inet_pton(AF_INET, q_query_[0].c_str(), &sa.sin_addr) == 1;
        if (m_valid) {
            q_param_.push_back( sa.sin_addr.s_addr);
        }
        else {
            cout << "invalid IP address" << m_format << endl;
        }
    }
    
    /** validate cpu id */
    if (m_valid) {
        int cid = atoi(q_query_[1].c_str());
        if (q_query_[1].length() == 1 && (cid == 0 || cid == 1)) {
            q_param_.push_back(cid);
        }
        else {
            m_valid = false;
            cout << "invalid cpu id; should be 1 digits [0 or 1]" << m_format << endl;
        }
    }
    
    /** validate start / end date */
    string tstart = q_query_[2] + " " + q_query_[3];
    string tend   = q_query_[4] + " " + q_query_[5];
    m_valid &= is_valid_time(tstart) && is_valid_time(tend);
    
    if (m_valid) {
        struct tm t_;
        long lt_s = 0, lt_e = 0;    /**< UNIX timestamp */
        if (!strptime(tstart.c_str(), "%Y-%m-%d %H:%M", &t_)) {
            m_valid = false;
            cout << "invalid time format" << m_format << endl;
        }
        else {
            lt_s = time2stamp(t_.tm_year+1900, t_.tm_mon+1, 
                                    t_.tm_mday, t_.tm_hour, t_.tm_min);
            q_param_.push_back(lt_s);
        }

        if (!m_valid || !strptime(tend.c_str(), "%Y-%m-%d %H:%M", &t_)) {
            m_valid = false;
            cout << "invalid time format" << m_format << endl;
        }
        else {
            lt_e = time2stamp(t_.tm_year+1900, t_.tm_mon+1, 
                                    t_.tm_mday, t_.tm_hour, t_.tm_min);
            if(lt_s > lt_e) {
                m_valid = false;
                cout << "invalid time sequence: must be in non-decreasing order" 
                     << m_format << endl;
            }
            else {
                q_param_.push_back(lt_e);
            }
        }
    }
    return m_valid;
}
/**
 * the query sentence should strictly follow the format (length) and
 * should have the right value for each corresponded position (year/month...etc)
 * 
 * @param query string type query 
 */
void QueryHandler::query(string query) {
    
    /** check if query is valid or not */
    if (!is_valid_query(query)) {
        return;
    }

    KDTreeNode low, high;     /** lower/higher bound */
    
    low.val_[0] = high.val_[0] = q_param_[0];    /* ip */
    low.val_[1] = high.val_[1] = q_param_[1];    /* cpu id */
    low.val_[2] = q_param_[2];                   /* time stamp */
    high.val_[2]= q_param_[3];
    
    cout << "CPU" << q_param_[1] << " on " << q_query_[0] << endl;
    this->query(low, high);
}

/**
 * compare queried nodes by timestamp
 * @param node1
 * @param node2
 * @return true if node1.timestamp < node2.timestamp
 */
static bool comp(KDTreeNode node1, KDTreeNode node2) {
    return node1.val_[2] < node2.val_[2];
}

/**
 * query all nodes within the range [lo, hi]
 * @param lo lower_bound of query region
 * @param hi higher_bound of query region
 */
void QueryHandler::query(KDTreeNode& lo, KDTreeNode& hi) {
    
    vector<KDTreeNode> out; /**< result */
    for (size_t i = 0; i < tree_cluster_.size(); ++i) {
        tree_cluster_[i]->find_within_range(lo, hi, back_inserter(out));
    }
    
    /** sort by time stamp */
    std::sort(out.begin(), out.end(), comp);
    
    /** output */
    for (size_t i = 0; i < out.size(); ++i) {
        int YYYY, MM, DD, H, M;
        stamp2time(out[i].val_[2], YYYY, MM, DD, H, M);
        printf("(%04d-%02d-%02d %02d:%02d, %ld%%)", YYYY, MM, DD, H, M, out[i].usage_);
        if (i < out.size()-1) cout << ", ";
    }
    cout << endl;
}

/**
 * convert human readable time format to UNIX timestamp
 * @param year
 * @param month
 * @param day
 * @param hour
 * @param minutes
 * @return UNIX timestamp
 */
long RangeQuery::time2stamp(int year, int month, int day, int hour, int minutes) {
    
    struct tm t;
    time_t epoch;
    t.tm_year = year-1900;
    t.tm_mon  = month - 1;      
    t.tm_mday = day;       
    t.tm_hour = hour;     
    t.tm_min  = minutes;
    t.tm_sec  = 0;
    t.tm_isdst= -1;       
    
    epoch = mktime(&t);
    return epoch;
}

/**
 * reverse time2stamp process;
 * @param epoch
 * @param yy
 * @param mm
 * @param dd
 * @param h
 * @param m
 */
void RangeQuery::stamp2time(time_t epoch, int &yy, int &mm, int &dd, int &h, int &m) {
    struct tm* tinfo = localtime(&epoch); 
    
    yy = tinfo->tm_year + 1900;
    mm = tinfo->tm_mon + 1;
    dd = tinfo->tm_mday;
    h  = tinfo->tm_hour;
    m  = tinfo->tm_min;
}
