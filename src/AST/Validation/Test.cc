#include <string>

#include "ASTString.hh"
#include "ASTStringList.hh"
#include "ASTVerificationVisitor.hh"
#include "ASTTag.hh"
#include "checksieve.h"
#include "Test.hh"

namespace sieve {

bool validateBareTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    size_t size = test->children().size();

    if (size == 0)
        return true;
    else
        return false;
}

Test::Test() {
    _usage_map["allof"]                 = "allof <tests: test-list>";
    _usage_map["anyof"]                 = "anyof <tests: test-list>";
    _usage_map["exists"]                = "exists [:mime] [:anychild] <header-names: string-list>\n";
    _usage_map["environment"]           =        "environment [COMPARATOR] [MATCH-TYPE] <name: string>                    \n"
                                          "                   <key-list: string-list>                                     \n"
                                          "                   Where name is one of: domain, host, location, name, phase,  \n"
                                          "                                         remote-host, remote-ip, or version    \n";
    _usage_map["header"]                =        "header [:mime] [:anychild] [:regex]                                     \n"
                                          "              [:type / :subtype / :contenttype / :param <params: string-list>] \n"
                                          "              [:comparator <string>]                                           \n"
                                          "              [:is / :contains / :matches]                                     \n"
                                          "              <header-names: string-list> <key-list: string-list>              \n";
    _usage_map["ihave"]                 = "ihave <capabilities: string-list>";
    _usage_map["not"]                   = "not <test1: test>";
    _usage_map["size"]                  = "size <:over / :under> <limit: number>";
    _usage_map["valid_notify_method"]   = "valid_notify_method <notification-uris: string-list>";

    _validation_fn_map["allof"]                 = &Test::_validateHasOnlyTestList;
    _validation_fn_map["anyof"]                 = &Test::_validateHasOnlyTestList;
    _validation_fn_map["environment"]           = &Test::_validateEnvironmentTest;
    _validation_fn_map["exists"]                = &Test::_validateExists;
    _validation_fn_map["header"]                = &Test::_validateHeaderTest;
    _validation_fn_map["ihave"]                 = &Test::_validateIhaveTest;
    _validation_fn_map["not"]                   = &Test::_validateNotTest;
    _validation_fn_map["size"]                  = &Test::_validateSizeTest;
    _validation_fn_map["valid_notify_method"]   = &Test::_validateValidNotifyMethodTest;
}

bool Test::validate(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    
    if (!_validation_fn_map[test->value()]) {
        DEBUGLOG(test->value() + " test is missing validation.")
        return true;
    }

    return (this->*_validation_fn_map[test->value()])(test);
}

std::string Test::usage(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    return "Usage: " + _usage_map[test->value()];
}

//-- Private Members
bool Test::_validateExists(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    size_t size = test->children().size();
    
    if (size < 1 || size > 3)
        return false;
    
    const ASTString *stringChild = dynamic_cast<const ASTString*>(test->children()[size - 1]);
    const ASTStringList *stringListChild = dynamic_cast<const ASTStringList*>(test->children()[size - 1]);
    if (stringChild == NULL && stringListChild == NULL)
        return false;
    
    // If we have more than 1 child, check that they are either :mime or :anychild
    if (size > 1) {
        const ASTTag *firstTag = dynamic_cast<const ASTTag*>(test->children()[0]);
        if (firstTag == NULL)
            return false;
    
        if (firstTag->value() != ":mime" && firstTag->value() != ":anychild")
            return false;
    }
    
    if (size > 2) {
        const ASTTag *secondTag = dynamic_cast<const ASTTag*>(test->children()[1]);
        if (secondTag == NULL)
            return false;
    
        if (secondTag->value() != ":mime" && secondTag->value() != ":anychild")
            return false;
    }
    
    return true;
}

bool Test::_validateHasOnlyTestList(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    std::vector<sieve::ASTNode *> children = test->children();
    
    for (std::vector<ASTNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        const ASTTest *child = dynamic_cast<ASTTest*>(*it);
    
        if (child == NULL)
            return false;
    }
    
    return true;
}

bool Test::_validateNotTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    size_t size = test->children().size();
    
    if (size != 1)
        return false;
    
    return true;
}

bool Test::_validateSizeTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    size_t size = test->children().size();
    
    if (size != 2)
        return false;
    
    const ASTTag *tag = dynamic_cast<const ASTTag*>(test->children()[0]);
    if (tag == NULL || (tag->value() != ":over" && tag->value() != ":under"))
        return false;
    
    return true;
}

bool Test::_validateValidNotifyMethodTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    size_t size = test->children().size();
    
    if (size != 1)
        return false;
    
    if (!nodeIsType<ASTStringList>(test->children()[0]) && !nodeIsType<ASTString>(test->children()[0]))
        return false;
    
    return true;
}

bool Test::_validateHeaderTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    std::vector<ASTNode *> children = node->children();
    size_t size = children.size();
    
    if (size < 2)
        return false;
    
    for (std::vector<ASTNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        const ASTTag *tag = dynamic_cast<const ASTTag*>(*it);
    
        if (tag != NULL) {
            std::string tagValue;
            tagValue = tag->value();
    
            if (tagValue == ":is" ||
                tagValue == ":matches" ||
                tagValue == ":contains" ||
                tagValue == ":comparator" ||
                tagValue == ":mime" ||
                tagValue == ":anychild" ||
                tagValue == ":type" ||
                tagValue == ":subtype" ||
                tagValue == ":contenttype" ||
                tagValue == ":param" ||
                tagValue == ":regex") {
                    continue;
                }
            
            return false;
        }
    } 
    
    return true;
}

bool Test::_validateIhaveTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    std::vector<ASTNode *> children = node->children();
    size_t size = children.size();
    
    if (size != 1)
        return false;
    
    const ASTString *string_child = dynamic_cast<const ASTString*>(children[0]);
    const ASTStringList *stringlist_child = dynamic_cast<const ASTStringList*>(children[0]);
    
    if (string_child == NULL && stringlist_child == NULL)
        return false;
    
    return true;
}

bool Test::_validateEnvironmentTest(const ASTNode *node) {
    const ASTTest *test = dynamic_cast<const ASTTest*>(node);
    std::vector<ASTNode *> children = node->children();
    size_t size = children.size();
    
    if (size < 2)
        return false;
    
    const ASTString *name_node = dynamic_cast<const ASTString*>(children[size-2]);
    const ASTString *key = dynamic_cast<const ASTString*>(children[size-1]);
    const ASTStringList *key_list = dynamic_cast<const ASTStringList*>(children[size-1]);
    
    // TODO: Validation for [COMPARATOR] / [MATCH-TYPE]
    
    if (name_node == NULL || (key_list == NULL && key == NULL))
        return false;
    
    std::string name = name_node->value();
    
    if (name == "domain" ||
        name == "host" ||
        name == "location" ||
        name == "name" ||
        name == "phase" ||
        name == "remote-host" ||
        name == "remote-ip" ||
        name == "version") {
        return true;
    }
    
    // RFC 6785
    // TODO: Fix this cast
    if (((ASTVerificationVisitor *)visitor())->requires_capability("imapsieve") && (
        name == "imap.user" ||
        name == "imap.email" ||
        name == "imap.cause" ||
        name == "imap.mailbox" ||
        name == "imap.changedflags")) {
        return true;
    }
    
    return false;
}

}