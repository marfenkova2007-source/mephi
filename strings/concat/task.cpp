#include "task.h"

char* concat(const char* lhs, const char* rhs) {
    int len1 = 0;
    while(*lhs != '\0'){
        ++lhs;
        ++len1;
    }
    int len2=0;
    while (*rhs!= '\0'){
        ++rhs;
        ++len2;
    }
    char* itog = new char[len1 + len2 +1];
    char* shs = itog;
    lhs-= len1;
    rhs-= len2;
    while (*lhs!= '\0'){
        *shs = *lhs;
        ++lhs;
        ++shs;
    }
    while(*rhs != '\0'){
        *shs = *rhs;
        ++rhs;
        ++shs;
    }
    *shs = '\0';
    return itog;
}
