# NVIDIA Fabric Manager Partition Tool

c++ tool to perform NVIDIA Fabric Manager operations, extending the functionalities from [NVIDIA sample code](https://docs.nvidia.com/datacenter/tesla/fabric-manager-user-guide/index.html#sample-code).

---
## Non-interactive mode:
```
root@localhost:~/NVIDIA-FabricManager-partition-tool# ./partitioner -h
Usage: ./partitioner [-i <IP>] -o <OP> [-p <ID>]
  -i, --ip <IP>      : IP address of Fabric Manager (default: 127.0.0.1)
  -o, --operation <N>: 0=List, 1=Activate, 2=Deactivate
  -p, --partition <ID>: Partition ID (required for Activate/Deactivate)
Running without options starts interactive mode.
```
```
root@localhost:~/NVIDIA-FabricManager-partition-tool# ./partitioner -o 0
Successfully connected to Fabric Manager at 127.0.0.1
Total supported partitions: 15

Partition ID   Number of GPUs GPU Module ID            Max NVLinks/GPU     STATUS
--------------------------------------------------------------------------------
0              8              1, 2, 3, 4, 5, 6, 7, 8   18                  INACTIVE
1              4              1, 2, 3, 4               18                  INACTIVE
2              4              5, 6, 7, 8               18                  INACTIVE
3              2              1, 3                     18                  INACTIVE
4              2              2, 4                     18                  INACTIVE
5              2              5, 7                     18                  INACTIVE
6              2              6, 8                     18                  ACTIVE
7              1              1                        0                   INACTIVE
8              1              2                        0                   INACTIVE
9              1              3                        0                   INACTIVE
10             1              4                        0                   INACTIVE
11             1              5                        0                   INACTIVE
12             1              6                        0                   INACTIVE
13             1              7                        0                   INACTIVE
14             1              8                        0                   INACTIVE
```
```
root@localhost:~/NVIDIA-FabricManager-partition-tool# ./partitioner -o 0 -f csv
Partition_ID,Num_GPUs,GPU_Module_IDs,Max_NVLinks_per_GPU,Status
0,8,"1 2 3 4 5 6 7 8",18,INACTIVE
1,4,"1 2 3 4",18,INACTIVE
2,4,"5 6 7 8",18,INACTIVE
3,2,"1 3",18,INACTIVE
4,2,"2 4",18,INACTIVE
5,2,"5 7",18,INACTIVE
6,2,"6 8",18,ACTIVE
7,1,"1",0,INACTIVE
8,1,"2",0,INACTIVE
9,1,"3",0,INACTIVE
10,1,"4",0,INACTIVE
11,1,"5",0,INACTIVE
12,1,"6",0,INACTIVE
13,1,"7",0,INACTIVE
14,1,"8",0,INACTIVE
```
```
root@localhost:~/NVIDIA-FabricManager-partition-tool# ./partitioner -o 2 -p 5
Connecting to Fabric Manager at 127.0.0.1...
Successfully connected to Fabric Manager at 127.0.0.1
Successfully sent deactivation request for partition 5
```



## Interactive mode:
<img width="952" height="725" alt="image" src="https://github.com/user-attachments/assets/287d6500-9348-47ea-85a0-f9a325bfadcf" />
