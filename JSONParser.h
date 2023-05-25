#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <string.h>
#include <stdio.h>

namespace json_parser{

    enum ERROR_CODES{FILE_FORMATTING_ERROR, FILE_NOT_FOUND};

    class parser final{
    private:
    char ***data;
    char * filename;
    size_t size;
    int _longestEntry;

    u_int8_t bracketCheck(FILE *);
    u_int8_t getFormatData(FILE *, int *, int *);
    u_int8_t setData(FILE *);

    public:
    parser(const char *);
    parser() = delete;
    ~parser();
    parser& operator=(const parser&);
    parser(const parser&);

    char * operator[](const char *)const;

    };

};

#endif
