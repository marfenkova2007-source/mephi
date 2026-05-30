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
