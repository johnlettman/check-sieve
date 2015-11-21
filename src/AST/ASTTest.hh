#ifndef __ASTTEST_HH__
#define __ASTTEST_HH__


#include "ASTCommand.hh"

namespace sieve
{

class ASTVisitor;

class ASTTest : public ASTCommand {
public:
    ASTTest() : ASTCommand() {}    
    ASTTest(yy::location location);
    ASTTest(std::string name);
    ASTTest(yy::location location, std::string name);
    
    void accept(ASTVisitor &visitor);
    
    std::string value() const { return this->_name; }
    
private:
    std::string _name;
};

} // namespace sieve

#endif
