#include <string>
#include <iostream>
#include "pool.hpp"

void show_usage(std::string const& name)
{
    std::cerr << "Usage: " << name << " <option(s)> CONFIG_FILE"
              << "Options:\n"
              << "\t-h,--help\tShow this help message\n"
              << "\t-c,--check\tCheck for valid pool config file\n"
              << "\t-v,--version\tVersion of NexusPool"
              << std::endl;
}

int main(int argc, char **argv)
{
    nexuspool::Pool pool;

    std::string pool_config_file{"pool.conf"};
    bool run_check = false;
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) 
        {
            show_usage(argv[0]);
            return 0;
        } 
        else if ((arg == "-c") || (arg == "--check")) 
        {
            run_check = true;
        }
        else 
        {
            pool_config_file = argv[i];
        }
    }

    if(run_check)
    {
        if(!pool.check_config(pool_config_file))
        {
            return -1;
        }
    }

    if(!pool.init(pool_config_file))
    {
        return -1;
    }

    pool.run();
  
    return 0;
}
