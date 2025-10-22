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
root@localhost:~/NVIDIA-FabricManager-partition-tool# ./partitioner -o 2 -p 5
Connecting to Fabric Manager at 127.0.0.1...
Successfully connected to Fabric Manager at 127.0.0.1
Successfully sent deactivation request for partition 5
```



## Interactive mode:
<img width="952" height="725" alt="image" src="https://github.com/user-attachments/assets/287d6500-9348-47ea-85a0-f9a325bfadcf" />
