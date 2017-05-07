An efficient range query algorithm based on KDTree-Clustering; implemented with open course library `kd-tree++`

test data
============
The data is randomly sampled for testing; the sampling process simulates the data generated within 24 hours, with 1k items for each minute. 
The data item has the following format:

`[time_stamp ip_address cpu_id cpu_usage]`

`time_stamp`: unix format time stamp.  
`ip_address`: randomly generated ip address [`192.168.0.1`, `192.168.255.255`)  
`cpu_id`    : value either 0 or 1  
`cpu usage` : cpu usage within range [0%..100%]

querying
============
list a summary of usage of a specified cpu on a specified pc within a range of time.

command
------------
1. generate data: ./generate_date.sh [path_to_file]  
2. query: ./query.sh [path_to_file]

misc
============
please see doxygen Doc for more details, or `helper` function in `main.cpp` about how to run the code.
To install document, install doxygen and run 'doxygen Doxyfile' in root folder

