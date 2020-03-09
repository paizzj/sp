#include "common.h"
#include "syncer.h"

int main(int argc,char*argv[])
{
    if (!ParseCmd(argc, argv)) 
    {
        std::cout << "ParseCmd error" << std::endl;
        return 0;
    }

    if (!InitDB())
    {
        std::cout << "InitDB error" << std::endl;
        return 0;
    }

    uint64_t height = 0;
    if (!GetLastHeight(height))
    {   
        std::cout << "GetLastHeight error" << std::endl;
        return 0;
    }

    std::vector<std::string> pools;
    /*
    if (!GetPools(pools))
    {
        std::cout << "GetPools error" << std::endl;
        return 0;
    }
    */
    g_syncer.syncBlocks(height, pools);

    return 0;
}
