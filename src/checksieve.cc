#include "checksieve.h"
#include "sieve_driver.hh"

#include <utility>

namespace sieve
{

#define LIBCHECKSIEVE_VERSION "1.0-dev"

extern const char *version() {
    return LIBCHECKSIEVE_VERSION;
}

struct parse_result sieve_parse_file( const char *filename, struct parse_options options ) {
    driver driver(std::move(options));
    return driver.parse_file(filename);
}

struct parse_result sieve_parse_string( const char *sieve, struct parse_options options ) {
    driver driver(std::move(options));
    return driver.parse_string(sieve);
}

}
