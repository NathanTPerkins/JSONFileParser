#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <string.h>
#include <stdio.h>

#define ARDUINO_PARSER

#ifdef ARDUINO_PARSER
#include <SD.h>
#include <Arduino.h>
#endif

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
    parser(const char *, int);
    parser() = delete;
    ~parser();
    parser& operator=(const parser&);
    parser(const parser&);

    char * get(const char *)const;
    u_int8_t set(const char *, char *);
    u_int8_t save(const char *);
    u_int8_t save();


    };

    #ifdef ARDUINO_PARSER
    class arduino_parser final{
    private:
    char ***data;
    char * filename;
    size_t size;
    int _longestEntry;

    u_int8_t bracketCheck(File *);
    u_int8_t getFormatData(File *, int *, int *);
    u_int8_t setData(File *);
    void rewind(File*);

    public:
    arduino_parser(const char *);
    arduino_parser(const char *, int);
    arduino_parser() = delete;
    ~arduino_parser();
    arduino_parser& operator=(const arduino_parser&);
    arduino_parser(const arduino_parser&);

    char * get(const char *)const;
    u_int8_t set(const char *, char *);
    u_int8_t save(const char *);
    u_int8_t save();


    };
    #endif

};

#endif
