#include "chain_builder.h"
#include <cstdlib>


int main(int argc, char** argv)
{
    ChainBuilder builder;
    builder.init(argc, argv);
    bool result = builder.run();
    
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
