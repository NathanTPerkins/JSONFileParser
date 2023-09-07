#include "JSONParser.h"

json_parser::arduino_parser::arduino_parser(const char * filename){
    this->filename = new char[strlen(filename) + 1];
    strcpy(this->filename, filename);

    File json_file = SD.open(filename, FILE_READ);
    if(json_file == NULL){
        return;
    }
    if(!bracketCheck(&json_file)){
        return;
    }
    int numEntries = 0, longestEntry = 0;
    getFormatData(&json_file, &numEntries, &longestEntry);
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

    setData(&json_file);

    json_file.close();
}

json_parser::arduino_parser::arduino_parser(const char * filename, int longest_entry_buffer){
    this->filename = new char[strlen(filename) + 1];
    strcpy(this->filename, filename);
    if(!SD.begin()){
        while(true);
    }
    File json_file = SD.open(filename, FILE_READ);
    if(!bracketCheck(&json_file)){
        return;
    }
    int numEntries = 0, longestEntry = 0;
    getFormatData(&json_file, &numEntries, &longestEntry);
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

    setData(&json_file);

    json_file.close();
}

json_parser::arduino_parser& json_parser::arduino_parser::operator=(const arduino_parser& p){
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

json_parser::arduino_parser::arduino_parser(const arduino_parser& p){

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

u_int8_t json_parser::arduino_parser::bracketCheck(File * json_file){
    char c, bracket;
    while((c = json_file->read()) != EOF){
        if(c == '{'){
            bracket = '{';
        }
        if(c == '}' && bracket != '{'){
            return 0;
        }
    }
    return 1;
}

void json_parser::arduino_parser::rewind(File* input_file){
    char filename[strlen(input_file->name()) + 1];
    strcpy(filename, input_file->name());
    input_file->close();
    *input_file = SD.open(filename, FILE_READ);
}

u_int8_t json_parser::arduino_parser::getFormatData(File * json_file, int * numEntries, int * longestEntry){
    if(!json_file){
        return 0;
    }
    rewind(json_file);
    char c, pc;
    bool hasKey = false, inBracket = true, inQuotes = false;
    int long_e = 0;
    *numEntries = 0;
    *longestEntry = 0;
    while((c = json_file->read()) != EOF){
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

u_int8_t json_parser::arduino_parser::setData(File *f){
    if(!f){
        return 0;
    }
    rewind(f);
    
    char c, pc;
    bool hasKey = false, inBracket = true, inQuotes = false;
    int long_e = 0;
    int i = 0, j = 0;
    while((c = f->read()) != EOF){
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

size_t json_parser::arduino_parser::size(){
    return this->_size;
}

u_int8_t json_parser::arduino_parser::save(const char *filename){
    File file = SD.open(filename, FILE_WRITE);
    if(file == NULL){
        return 0;
    }
    file.write("{");
    for(int i = 0; i < this->_size - 1; ++i){
        file.write("\n\t\"");
        file.write(this->data[i][0]);
        file.write("\":\"");
        file.write(this->data[i][1]);
        file.write(",");
    }
    file.write("\n\t\"");
    file.write(this->data[this->_size - 1][0]);
    file.write("\":\"");
    file.write(this->data[this->_size - 1][1]);
    file.write("\n}");
    file.close();
    return 1;
}

u_int8_t json_parser::arduino_parser::save(){
    return this->save(this->filename);
}

char *json_parser::arduino_parser::get(const char *key)const{
    for(int i = 0; i < this->_size; ++i){
        if(strcmp(this->data[i][0], key) == 0){
            return this->data[i][1];
        }
    }
    return NULL;
}

u_int8_t json_parser::arduino_parser::set(const char*key, char *value){
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

json_parser::arduino_parser::~arduino_parser(){
    delete [] this->filename;
    for(int i = 0; i < this->_size; ++i){
        delete [] this->data[i][0];
        delete [] this->data[i][1];
        delete [] this->data[i];
    }
    delete [] this->data;
}