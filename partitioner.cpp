#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sstream>

// Assuming this header defines all necessary FM types (fmReturn_t, fmFabricPartitionInfo_t, etc.)
#include "nv_fm_agent.h" 

void printFmError(const char* operation, fmReturn_t fmReturn) {
    std::cout << "Error: Failed to " << operation << ". (Code: " << fmReturn << ")" << std::endl;
}

void clearCin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void printMenu() {
    std::cout << "------------------------------------------\n";
    std::cout << "Fabric Manager Partition Operations:\n";
    std::cout << "  0 - List Supported Partitions\n";
    std::cout << "  1 - Activate a Partition\n";
    std::cout << "  2 - Deactivate a Partition\n";
    std::cout << "  3 - Quit\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Enter operation: ";
}


int main(int argc, char **argv)
{
    fmReturn_t fmReturn;
    fmHandle_t fmHandle = NULL;
    // Set size based on FM_DEVICE_PCI_BUS_ID_BUFFER_SIZE (which is 32) or just 16 for IP
    char hostIpAddress[16] = {0}; 
    unsigned int operation = 0;
    fmFabricPartitionId_t partitionId = 0;
    fmFabricPartitionList_t partitionList = {0};

    std::cout << "Please input an IP address to connect to (default: 127.0.0.1). Press Enter to accept default: ";
    std::string buffer;
    std::getline(std::cin, buffer);

    if (buffer.empty()) {
        strncpy(hostIpAddress, "127.0.0.1", sizeof(hostIpAddress) - 1);
        hostIpAddress[sizeof(hostIpAddress) - 1] = '\0';
    } else {
        if (buffer.length() >= sizeof(hostIpAddress)) {
            std::cout << "Invalid IP address (too long).\n" << std::endl;
            return FM_ST_BADPARAM; 
        }
        strncpy(hostIpAddress, buffer.c_str(), sizeof(hostIpAddress) - 1);
        hostIpAddress[sizeof(hostIpAddress) - 1] = '\0';
    }

    /* Initialize Fabric Manager API interface library */
    fmReturn = fmLibInit();
    if (FM_ST_SUCCESS != fmReturn) {
        std::cout << "Failed to initialize Fabric Manager API interface library." << std::endl;
        return fmReturn;
    }

    fmConnectParams_t connectParams = {0}; 
    connectParams.version = fmConnectParams_version; 
    connectParams.timeoutMs = 1000; 
    connectParams.addressIsUnixSocket = 0;
    strncpy(connectParams.addressInfo, hostIpAddress, sizeof(connectParams.addressInfo));
    connectParams.addressInfo[sizeof(connectParams.addressInfo) - 1] = '\0'; 
    
    fmReturn = fmConnect(&connectParams, &fmHandle);
    if (fmReturn != FM_ST_SUCCESS) {
        printFmError("connect to Fabric Manager", fmReturn);
        fmLibShutdown(); 
        return fmReturn;
    }
    
    std::cout << "Successfully connected to Fabric Manager at " << hostIpAddress << std::endl;

    while (true)
    {
        printMenu();
        if (!(std::cin >> operation)) {
            std::cout << "Invalid input. Please enter a number.\n" << std::endl;
            clearCin(); 
            continue;
        }
        
        if (operation == 3) {
            std::cout << "Exiting." << std::endl;
            break; 
        }

        partitionId = 0; 
        if (operation == 1 || operation == 2) {
            std::cout << "Input Shared Fabric Partition ID: \n";
            if (!(std::cin >> partitionId)) {
                 std::cout << "Invalid input. Please enter a number.\n" << std::endl;
                 clearCin(); 
                 continue;
            }

            if (partitionId >= FM_MAX_FABRIC_PARTITIONS) {
                std::cout << "Invalid partition ID (must be < " << FM_MAX_FABRIC_PARTITIONS << ")." << std::endl;
                clearCin();
                continue;
            }
        }
        std::cout << std::endl; 

        switch (operation) {
            case 0: // List Supported Partitions
                partitionList.version = fmFabricPartitionList_version;
                fmReturn = fmGetSupportedFabricPartitions(fmHandle, &partitionList);
                
                if (fmReturn != FM_ST_SUCCESS) {
                    printFmError("get partition list", fmReturn);
                } else {
                    std::cout << "Total supported partitions: " << partitionList.numPartitions << "\n" << std::endl;
                    
                    // --- Display Table Header matching the image ---
                    std::cout << std::left << std::setw(15) << "Partition ID"
                              << std::left << std::setw(15) << "N of GPUs"
                              << std::left << std::setw(25) << "Module IDs"
                              << std::left << "Max NVLinks per GPU" << std::endl;
                    std::cout << "----------------------------------------------------------------" << std::endl;
                    
                    // --- Display Partition Details ---
                    for (unsigned int i = 0; i < partitionList.numPartitions; ++i) {
                        const auto& info = partitionList.partitionInfo[i];
                        
                        std::stringstream gpu_list_ss;
                        unsigned int max_nvlinks = 0; 
                        bool first_gpu = true;
                        
                        // Build GPU List string AND capture the max_nvlinks value
                        for (unsigned int j = 0; j < info.numGpus; ++j) {
                            if (!first_gpu) {
                                gpu_list_ss << ", ";
                            }
                            // Accessing the physicalId (GPU ID) from the sub-structure
                            gpu_list_ss << info.gpuInfo[j].physicalId;
                            
                            if (j == 0) {
                                // Use the max value from the first GPU in the partition
                                max_nvlinks = info.gpuInfo[j].maxNumNvLinks; 
                            }
                            first_gpu = false;
                        }

                        std::string gpu_list = gpu_list_ss.str();
                        if (gpu_list.empty()) {
                            gpu_list = "N/A";
                        }

                        // Display the row using formatting and the retrieved data
                        std::cout << std::left << std::setw(15) << info.partitionId
                                  << std::left << std::setw(15) << info.numGpus
                                  << std::left << std::setw(25) << gpu_list
                                  << std::left << max_nvlinks 
                                  // Optional: Display the status on a new line or at the end
                                  << " (" << (info.isActive ? "ACTIVE" : "INACTIVE") << ")"
                                  << std::endl;
                    }
                }
                break;

            case 1: // Activate a Partition
                fmReturn = fmActivateFabricPartition(fmHandle, partitionId);
                if (fmReturn == FM_ST_SUCCESS) {
                    std::cout << "Successfully sent activation request for partition " << partitionId << std::endl;
                } else {
                    printFmError("activate partition", fmReturn);
                }
                break;

            case 2: // Deactivate a Partition
                fmReturn = fmDeactivateFabricPartition(fmHandle, partitionId);
                if (fmReturn == FM_ST_SUCCESS) {
                    std::cout << "Successfully sent deactivation request for partition " << partitionId << std::endl;
                } else {
                    printFmError("deactivate partition", fmReturn);
                }
                break;

            default:
                std::cout << "Invalid operation. Please select from the menu." << std::endl;
                break;
        }
        std::cout << std::endl; 
    }

    /* Clean */
    fmDisconnect(fmHandle);
    fmLibShutdown();
    
    return FM_ST_SUCCESS; 
}