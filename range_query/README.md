efficient range querying based on kdt-cluster, implemented with kd-tree++ open course library
data
============
sampled data; each line with format [time_stamp ip_address cpu_id cpu_usage]
time stamp is in unix format. 
ip address is random value
cpu id has value either 0 or 1
cpu usage taks value in range of [0%..100%]

query
============
the goal of this demo is to list a summary of usage of a specified cpu on a specified pc within a range of time.
command
============
1. generate data: ./g.sh [path_to_file] \n
2. query: ./q.sh [path_to_file]
