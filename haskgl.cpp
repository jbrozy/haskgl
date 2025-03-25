#include <iostream>

#define FLAG_IMPLEMENTATION
#include "flag.h"
#include "Lexer.h"

void usage(void)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n", flag_program_name());
    flag_print_options(stderr);
}

int main(int argc, char**argv)
{
    // const char* source = "1 + 1 = 2\n";
    const char* source = "abcdef + 1 :: 2abddf\n";
    Lexer lexer{ source };
    Token token = lexer.next();
    while(token.type != End)
    {
        std::cout << token.data << std::endl;
        token = lexer.next();
    }
    return 0;
}
