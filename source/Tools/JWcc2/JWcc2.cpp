#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"

using namespace std;

int main() {
    string source_code = "int main() { return 42; }";

    // 1. Run the Lexer
    Lexer lexer(source_code);
    vector<Token> tokens = lexer.tokenize();

    // 2. Run the Parser
    Parser parser(tokens);
    try
    {
        unique_ptr<ProgramNode> ast = parser.parse_program();
        cout << "Successfully parsed the source into an AST!" << endl;
        // The AST root is ready for optimization or LLVM/ASM code generation.
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}