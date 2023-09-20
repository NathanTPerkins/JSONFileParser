#include "JSONParser.h"

json_parser::parser::parser(const char * filename){
    this->filename = new char[strlen(filename) + 1];
    strcpy(this->filename, filename);

    FILE * json_file = fopen(filename, "r");
    if(json_file == NULL){
        throw ERROR_CODES::FILE_NOT_FOUND;
    }
    if(!bracketCheck(json_file)){
        throw ERROR_CODES::FILE_FORMATTING_ERROR;
    }
    int numEntries = 0, longestEntry = 0;
    getFormatData(json_file, &numEntries, &longestEntry);
    this->_longestEntry = longestEntry;
    this->_size = numEntries;
    this->data = new char**[numEntries];
    for(int i = 0; i < this->_size; ++i){
        this->data[i] = new char*[2];
        this->data[i][0] = new char[longestEntry];
        this->data[i][1] = new char[longestEntry];

        memset(this->data[i][0], 0, longestEntry);
        memset(this->data[i][1], 0, longestEntry);
    }

    setData(json_file);

    fclose(json_file);
}

json_parser::parser::parser(const char * filename, int longest_entry_buffer){
    this->filename = new char[strlen(filename) + 1];
    strcpy(this->filename, filename);

    FILE * json_file = fopen(filename, "r");
    if(json_file == NULL){
        throw ERROR_CODES::FILE_NOT_FOUND;
    }
    if(!bracketCheck(json_file)){
        throw ERROR_CODES::FILE_FORMATTING_ERROR;
    }
    int numEntries = 0, longestEntry = 0;
    getFormatData(json_file, &numEntries, &longestEntry);
    this->_longestEntry = longest_entry_buffer;
    this->_size = numEntries;
    this->data = new char**[numEntries];
    for(int i = 0; i < this->_size; ++i){
        this->data[i] = new char*[2];
        this->data[i][0] = new char[longest_entry_buffer];
        this->data[i][1] = new char[longest_entry_buffer];

        memset(this->data[i][0], 0, longest_entry_buffer);
        memset(this->data[i][1], 0, longest_entry_buffer);
    }

    setData(json_file);

    fclose(json_file);
}

json_parser::parser& json_parser::parser::operator=(const parser& p){
    delete [] this->filename;
    for(int i = 0; i < this->_size; ++i){
        delete [] this->data[i][0];
        delete [] this->data[i][1];
        delete [] this->data[i];
    }
    delete [] this->data;

    this->_size = p._size;
    this->_longestEntry = p._longestEntry;

    this->filename = new char[strlen(p.filename) + 1];
    strcpy(this->filename, p.filename);

    this->data = new char**[this->_size];
    for(int i = 0; i < this->_size; ++i){
        this->data[i] = new char*[2];
        this->data[i][0] = new char[this->_longestEntry];
        this->data[i][1] = new char[this->_longestEntry];

        memset(this->data[i][0], 0, this->_longestEntry);
        memset(this->data[i][1], 0, this->_longestEntry);
    }

    for(int i = 0; i < this->_size; ++i){
        strcpy(this->data[i][0], p.data[i][0]);
        strcpy(this->data[i][1], p.data[i][1]);
    }
    return *this;
}

json_parser::parser::parser(const parser& p){

    this->_size = p._size;
    this->_longestEntry = p._longestEntry;

    this->filename = new char[strlen(p.filename) + 1];
    strcpy(this->filename, p.filename);

    this->data = new char**[this->_size];
    for(int i = 0; i < this->_size; ++i){
        this->data[i] = new char*[2];
        this->data[i][0] = new char[this->_longestEntry];
        this->data[i][1] = new char[this->_longestEntry];

        memset(this->data[i][0], 0, this->_longestEntry);
        memset(this->data[i][1], 0, this->_longestEntry);
    }

    for(int i = 0; i < this->_size; ++i){
        strcpy(this->data[i][0], p.data[i][0]);
        strcpy(this->data[i][1], p.data[i][1]);
    }
}

u_int8_t json_parser::parser::bracketCheck(FILE * json_file){
    char c, bracket;
    while((c = getc(json_file)) != EOF){
        if(c == '{'){
            bracket = '{';
        }
        if(c == '}' && bracket != '{'){
            return 0;
        }
    }
    return 1;
}

u_int8_t json_parser::parser::getFormatData(FILE * json_file, int * numEntries, int * longestEntry){
    if(!json_file){
        return 0;
    }
    rewind(json_file);
    char c, pc;
    bool hasKey = false, inBracket = true, inQuotes = false;
    int long_e = 0;
    *numEntries = 0;
    *longestEntry = 0;
    while((c = fgetc(json_file)) != EOF){
        if(c == '\n' || c == 32 || c == '{' || c == '\t'){
            if(inQuotes && c == 32){
                ++long_e;
            }
            continue;
        }else{
            if(c == ':' && long_e > 0){
                hasKey = true;
                inQuotes = false;
            }
            if((c == 44 || c == '}') && hasKey){
                *numEntries += 1;
                hasKey = false;
            }
            if(c == ':' || c == ','){
                if(long_e >= *longestEntry){
                    *longestEntry = long_e;
                }
                long_e = 0;
            }
            else{
                if(c == '\"' && !inQuotes){
                    inQuotes = true;
                    continue;
                }else if(c == '\"' && inQuotes){
                    inQuotes = false;
                    continue;
                }
                if(inQuotes || hasKey){
                    ++long_e;
                }
            }
        }
    }
    return 1;
}

u_int8_t json_parser::parser::setData(FILE *f){
    if(!f){
        return 0;
    }
    rewind(f);
    
    char c, pc;
    bool hasKey = false, inBracket = true, inQuotes = false;
    int long_e = 0;
    int i = 0, j = 0;
    while((c = fgetc(f)) != EOF){
        if(c == '\n' || c == 32 || c == '{' || c == '\t'){
            if(inQuotes && c == 32){
                strncat(this->data[i][j], &c, 1);
                ++long_e;
            }
            continue;
        }else{
            if(c == '}'){
                break;
            }
            if(c == ':' && long_e > 0){
                hasKey = true;
                inQuotes = false;
                ++j;
            }
            if((c == 44 || c == '}') && hasKey){
                ++i;
                j = 0;
                hasKey = false;
            }
            if(c == ':' || c == ','){
                long_e = 0;
            }
            else{
                if(c == '\"' && !inQuotes){
                    inQuotes = true;
                    continue;
                }else if(c == '\"' && inQuotes){
                    inQuotes = false;
                    continue;
                }
                strncat(this->data[i][j], &c, 1);
                ++long_e;

            }
        }
    }

    return 1;
}

u_int8_t json_parser::parser::reload(){
    FILE * json_file = fopen(filename, "r");
    if(json_file == NULL){
        return 0;
    }
    if(!bracketCheck(json_file)){
        return 0;
    }
    int numEntries = 0, longestEntry = 0;
    getFormatData(json_file, &numEntries, &longestEntry);
    this->_longestEntry = longestEntry;
    this->_size = numEntries;
    this->data = new char**[numEntries];
    for(int i = 0; i < this->_size; ++i){
        this->data[i] = new char*[2];
        this->data[i][0] = new char[longestEntry];
        this->data[i][1] = new char[longestEntry];

        memset(this->data[i][0], 0, longestEntry);
        memset(this->data[i][1], 0, longestEntry);
    }

    setData(json_file);

    fclose(json_file);
    return 1;
}

size_t json_parser::parser::size(){
    return this->_size;
}

u_int8_t json_parser::parser::save(const char *filename){
    FILE *file = fopen(filename, "w");
    if(file == NULL){
        return 0;
    }
    fprintf(file, "{");
    for(int i = 0; i < this->_size - 1; ++i){
        fprintf(file, "\n\t\"%s\":\"%s\",", this->data[i][0], this->data[i][1]);
    }
    fprintf(file, "\n\t\"%s\":\"%s\"", this->data[this->_size - 1][0], this->data[this->_size - 1][1]);
    fprintf(file, "\n}");
    fclose(file);
    return 1;
}

u_int8_t json_parser::parser::save(){
    return this->save(this->filename);
}

char *json_parser::parser::operator[](const char *key){
    return this->get(key);
}

char *json_parser::parser::get(const char *key)const{
    for(int i = 0; i < this->_size; ++i){
        if(strcmp(this->data[i][0], key) == 0){
            return this->data[i][1];
        }
    }
    return NULL;
}

u_int8_t json_parser::parser::set(const char*key, char *value){
    for(int i = 0; i < this->_size; ++i){
        if(strcmp(this->data[i][0], key) == 0){
            if(strlen(value) > this->_longestEntry){
                
            }
            strlcpy(this->data[i][1], value, this->_longestEntry);
           return 1;
        }
    }
    return 0;
}

json_parser::parser::~parser(){
    delete [] this->filename;
    for(int i = 0; i < this->_size; ++i){
        delete [] this->data[i][0];
        delete [] this->data[i][1];
        delete [] this->data[i];
    }
    delete [] this->data;
}