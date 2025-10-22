#include <iostream>
#include <string>
#include <cstring>  // Use <cstring> in C++ instead of <string.h>
#include <iomanip>  // For formatting output
#include <limits>   // For clearing input buffer

#include "nv_fm_agent.h"

// --- FIX: Removed all assumed/non-existent functions and types ---
// The compiler confirmed fmGetActiveFabricPartition, fmGetFabricPartitionInfo,
// and fmFabricPartitionState_t do not exist in your header files.


/**
 * @brief Helper function to print a formatted error message.
 */
void printFmError(const char* operation, fmReturn_t fmReturn) {
    std::cout << "Error: Failed to " << operation << ". (Code: " << fmReturn << ")" << std::endl;
}

/**
 * @brief Helper function to clear std::cin buffer on invalid input.
 */
void clearCin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief Prints the main menu.
 * --- FIX: Removed non-existent operations 3 and 4 ---
 */
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
    char hostIpAddress[16] = {0};
    unsigned int operation = 0;
    fmFabricPartitionId_t partitionId = 0;
    fmFabricPartitionList_t partitionList = {0};

    // --- FIX: Removed variables for non-existent features ---
    // fmFabricPartitionId_t activePartitionId = 0;
    // fmFabricPartitionInfo_t partitionInfo = {0};

    // --- Get Connection Info Once ---
    // Default to localhost if user presses Enter
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

    clearCin(); 

    /* Initialize Fabric Manager API interface library */
    fmReturn = fmLibInit();
    if (FM_ST_SUCCESS != fmReturn) {
        std::cout << "Failed to initialize Fabric Manager API interface library." << std::endl;
        return fmReturn;
    }

    /* Connect to Fabric Manager instance */
    fmConnectParams_t connectParams;
    strncpy(connectParams.addressInfo, hostIpAddress, sizeof(connectParams.addressInfo));
    connectParams.addressInfo[sizeof(connectParams.addressInfo) - 1] = '\0'; 
    connectParams.timeoutMs = 1000; 
    connectParams.version = fmConnectParams_version;
    connectParams.addressIsUnixSocket = 0;
    
    fmReturn = fmConnect(&connectParams, &fmHandle);
    if (fmReturn != FM_ST_SUCCESS) {
        printFmError("connect to Fabric Manager", fmReturn);
        fmLibShutdown(); 
        return fmReturn;
    }
    
    std::cout << "Successfully connected to Fabric Manager at " << hostIpAddress << std::endl;


    // --- Main Operation Loop ---
    while (true)
    {
        printMenu();
        if (!(std::cin >> operation)) {
            std::cout << "Invalid input. Please enter a number.\n" << std::endl;
            clearCin();
            continue;
        }
        
        // --- FIX: Changed quit operation from 5 to 3 ---
        if (operation == 3) {
            std::cout << "Exiting." << std::endl;
            break; // Exit loop
        }

        // Get partition ID if needed for the operation
        partitionId = 0; // Reset
        // --- FIX: Only need partitionId for ops 1 and 2 ---
        if (operation == 1 || operation == 2) {
            std::cout << "Input Shared Fabric Partition ID: \n";
            if (!(std::cin >> partitionId)) {
                 std::cout << "Invalid input. Please enter a number.\n" << std::endl;
                 clearCin();
                 continue;
            }

            if (partitionId >= FM_MAX_FABRIC_PARTITIONS) {
                std::cout << "Invalid partition ID (too large)." << std::endl;
                continue;
            }
        }
        std::cout << std::endl; // Formatting

        // --- Operation Switch ---
        switch (operation) {
            case 0: // List Supported Partitions
                partitionList.version = fmFabricPartitionList_version;
                fmReturn = fmGetSupportedFabricPartitions(fmHandle, &partitionList);
                if (fmReturn != FM_ST_SUCCESS) {
                    printFmError("get partition list", fmReturn);
                } else {
                    std::cout << "Total partitions supported: " << partitionList.numPartitions << std::endl;
                    
                    // --- CRITICAL FIX: Use the correct struct members based on compiler errors ---
                    // partitionList.partitionInfo[i].partitionId
                    for (unsigned int i = 0; i < partitionList.numPartitions; ++i) {
                        std::cout << "  - Partition ID: " << partitionList.partitionInfo[i].partitionId << std::endl;
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

            // --- FIX: Removed case 3 and 4 as functions don't exist ---

            default:
                std::cout << "Invalid operation. Please select from the menu." << std::endl;
                break;
        }
        std::cout << std::endl; // Add space before next menu
    }


    /* Clean up */
    fmDisconnect(fmHandle);
    fmLibShutdown();
    
    return FM_ST_SUCCESS; 
}
