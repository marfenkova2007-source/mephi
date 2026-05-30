#include "task.h"

bool isalpha(unsigned char c) {
    const char* bukvi = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
    for (const char* simvol = bukvi; *simvol != '\0'; ++simvol) {
        if (c==*simvol){
            return true;
        }
    }
    return false;
}

unsigned char tolower(unsigned char c) {
    const char* bukvi = "QWERTYUIOPLKJHGFDSAZXCVBNM";
    for (const char* s = bukvi; *s!='\0'; ++s){
        if (c == *s){
            c = c+('a'-'A');
            return c;
        }
    }
    return c;
}

bool is_palindrome(const std::string& s) {
    size_t r = s.size()-1;
    size_t l = 0;
    bool pok = true;
    while (l<r and r>0 and l<s.size()){
        if (isalpha(s[r])){
            if (isalpha(s[l])){
                if (tolower(s[r])!=tolower(s[l])){
                    pok = false;
                    break;
                }
                else{
                    ++l;
                    --r;
                }
            }
            else{
                ++l;
            }
        }
        else{
            --r;
        }
    }
    return pok;
}
