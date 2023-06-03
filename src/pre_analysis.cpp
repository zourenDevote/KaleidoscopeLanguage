

#include <map>
#include <iostream>
#include <cstring>
#include <regex.h>

#include "pre_analysis.h"
#include "global_variable.h"


/// ----------------------------------------------------------------
/// static variable defined to help pre analysis
static int CurrentIndex = 0;
static FILE *Fp = nullptr;
static ProgramAST *CurProg = nullptr;
static std::map<std::string, ProgramAST *> ProgMap;
static std::vector<std::string> FileNameList;
static regex_t Regex;
static char Pattern[] = "[ ]*import[ ]*\"([a-zA-Z0-9_]+.k)\"[ ]*;";
static std::vector<ProgramAST*> DagCheckStack; 
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
/// @brief init regex expr
static inline bool init() {
    int ret = regcomp(&Regex, Pattern, REG_EXTENDED);
    if(ret) {
        std::cerr << "Can not init regex expr!" << std::endl;
        return false;
    }
    return true; 
}
/// ----------------------------------------------------------------

/// ----------------------------------------------------------------
/// @brief free resources that open
static inline void close() {
    if(Fp)
        fclose(Fp);
    regfree(&Regex);
    InputFileList = FileNameList;
    FileNameList.clear();
    ProgMap.clear();
}
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
static inline ProgramAST* getOrCreateProgAST(const std::string& file) {
    if(ProgMap.find(file) == ProgMap.end()) {
        ProgramAST *prog = new ProgramAST({CurrentIndex, 0, 0});
        FileNameList.push_back(file);
        ProgramList.push_back(prog);
        CurrentIndex++;
        ProgMap.insert({file, prog});
    }
    return ProgMap[file];
}
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
static void getImportFileName(char *s) {
    regmatch_t matches[2];
    int offset = 0, start, end, length;
    char *file = nullptr;
    while(regexec(&Regex, s+offset, 2, matches, 0) == 0) {
        start = matches[1].rm_so;
        end = matches[1].rm_eo;

        if(start != -1 && end != -1) {
            length = end - start;
            file = (char*)malloc((length + 1) * sizeof(char));
            file[length] = '\0'; 
            strncpy(file, s+offset+start, length);
            CurProg->addDependentProg(getOrCreateProgAST(file));
            free(file);
            file = nullptr;
        }

        offset += matches[0].rm_eo;
    }
}
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
static inline void anaFileDepAndCreateProgramAST() {
    static char File[512];
    while((fgets(File, sizeof(File), Fp)) != NULL) {
        getImportFileName(File);
        memset(File, 0, sizeof(File));
    }
}
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
/// @brief This two function checks if there is a circular reference
/// because circular reference can't confirm the compile order.
/// @example t1 import t2, t2 import t3, t3 import t1, there are error
/// import way.
static inline bool depCheck(ProgramAST *prog) {
    for(auto *p : DagCheckStack) {
        if(p == prog) {
            DagCheckStack.push_back(prog);
            return false;
        }
    }

    DagCheckStack.push_back(prog);

    for(auto *p : prog->getDependentProgs()) {
        if(!depCheck(p))
            return false;
    }

    DagCheckStack.pop_back();

    return true;
}


static inline bool depCheck() {
    for(const auto& prog : ProgramList) {
        DagCheckStack.clear();
        if(!depCheck(prog)) return false;
    }
    return true;
}
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
static inline void logRefError() {
    auto it1 = DagCheckStack.begin();
    auto it2 =  DagCheckStack.begin()+1;
    auto end = DagCheckStack.end();
    std::cerr << "There are circular references in the source code and compilation order cannot be determined!" << std::endl;
    while(it2 != end) {
        std::cerr << InputFileList[(*it1)->getLineNo()->FileIndex] << " import " << InputFileList[(*it2)->getLineNo()->FileIndex] << std::endl;
        it1 ++;
        it2 ++;
    }
}
/// ----------------------------------------------------------------


/// ----------------------------------------------------------------
bool preFileDepAnalysis() {
    
    if(!init()) {
        close();
        return false;
    }

    CurrentIndex = 0;

    Fp = nullptr;
    CurProg = nullptr;

    for(const auto& file : InputFileList) {
        Fp = fopen(file.c_str(), "rt+");
        
        if(!Fp) {
            std::cerr << "Can not find this file " << file << "." << std::endl;
            close();
            return false;
        }

        CurProg = getOrCreateProgAST(file);
    
        anaFileDepAndCreateProgramAST();

        fclose(Fp);
        Fp = nullptr;
    }

    close();

    if(!depCheck()) {
        logRefError();
        return false;
    }

    return true;
}
/// ----------------------------------------------------------------



