#include "text_generator.h"
#include <cstdlib>


int main(int argc, char** argv)
{
    TextGenerator generator;
    generator.init(argc, argv);
    bool result = generator.run();
    
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
