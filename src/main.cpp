#include <iostream>
#include <fstream>
#include <string>

int main (int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: paracl <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    std::ifstream file (filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::cout << "This place for ParaCL interpreter..." << std::endl;

    return 0;
}
