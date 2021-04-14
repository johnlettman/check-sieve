#ifndef __VALIDATION_H__
#define __VALIDATION_H__

#include "ASTNode.hh"
#include "ASTVisitor.hh"

namespace sieve {

typedef bool (*ValidationFunctionPtr)(const ASTNode *);

template <class T>
bool nodeIsType(const ASTNode *node) {
    const T *t_node = dynamic_cast<const T*>(node);
    return t_node != NULL;
}

class Validator {
public:
    Validator() { }
    ~Validator() {}
    
    void set_visitor( const ASTVisitor *v ) { _visitor = v; }
    
    virtual bool validate( const ASTNode * ) =0;
    virtual std::string usage( const ASTNode * ) =0;
    
    const ASTVisitor *visitor() { return _visitor; }

private:
    const ASTVisitor *_visitor;
};

typedef bool (Validator::* ValidationFunctionPtr2)(const ASTNode *);

}

#endif
