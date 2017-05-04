An efficient range query algorithm based on KDTree-Clustering; implemented with open course library `kd-tree++`

test data
============
The data is randomly sampled for testing in a day; 1000 data items are generated for every 1 minute, each data has the following format:

`[time_stamp ip_address cpu_id cpu_usage]`

`time_stamp`: unix format time stamp.  
`ip_address`: randomly generated ip address startgin from `192.168.0.1`  
`cpu_id`    : value either 0 or 1  
`cpu usage` : cpu usage within range [0%..100%]

querying
============
This demo tries to list a summary of usage of a specified cpu on a specified pc within a range of time.

command
------------
1. generate data: ./generate_date.sh [path_to_file]  
2. query: ./query.sh [path_to_file]

misc
============
please see doxygen Doc for more details.
To install document, install doxygen and run 'doxygen Doxyfile' in root folder

