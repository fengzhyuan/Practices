/** 
 * \file   main.cpp
 * Initialize context and parse query
 * \author fengzh
 * \copyright GNU Public License.
 */

#include "range_query.h"

/*
 * 
 */
int main(int argc, char** argv) {
    setvbuf(stdout,0,_IONBF,0);
   
    string type = "", path = "";
    
    if(argc < 3) {
        cout << "too few arguments" << endl;
        return -1;
    }
    type = string(argv[1]);
    path = string(argv[2]);
    bool m_exit = false;
    
    shared_ptr<RangeQuery::QueryHandler> query_handler;  
   
    if (type == "EXIT") {
        m_exit = true;
    }
    if (type == "GENERATE") {
        shared_ptr<RangeQuery::Generator> m_generator(new RangeQuery::Generator(path));        
        if (!m_generator->is_open()) {
            cout << "generate log file failed" << endl;
        }
        return 0;
    }
    else if (type == "QUERY") {
        if (!query_handler.get()) {
            shared_ptr<RangeQuery::QueryHandler> m_handler(new RangeQuery::QueryHandler(path));            
            if (!m_handler->is_open()) {
                cout << "open file failed" << endl;
                return -1;
            }
            query_handler = m_handler;
        }
        while (!m_exit) {
            string input, query;
            getline(cin, input);
            
            stringstream ss(input);
            getline(ss, type, ' ');
            
            string tmp;
            while (getline( ss, tmp, ' '))
                query += tmp + " ";
            
            if (!query.empty()) {
                query.erase(query.end() - 1);
            }
            if (type == "EXIT") { 
                m_exit = true; 
                break;
            }
            else if (type == "QUERY" && !query.empty()) {
                query_handler->query(query);
            }
            
        }
    }
    cout << "bye" << endl;
    return 0;
}


