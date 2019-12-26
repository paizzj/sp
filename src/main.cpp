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
    std::cout << "height = " << height << std::endl;
    g_syncer.syncBlocks(height);

    return 0;
}
