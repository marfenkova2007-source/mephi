#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

string normalize(string_view path) {
    string s;
    if (path.size()==0){
        return ".";
    }
    if (path.size() == 1 || (path.find('.') == string_view::npos && path.find('/') == string_view::npos)){
        return string(path);
    }
    vector<string> slova;
    size_t l = 0;
    size_t r = path.size();
    while (l<r){
        if (path[l]=='/'){
            ++l;
            continue;
        }
        size_t new_right = l;
        while( new_right<r and path[new_right]!='/'){
            ++new_right;
        }
        string slovo = string(path.substr(l, new_right-l));
        //slova.push_back(slovo);
        if (!slovo.empty()) {  
            slova.push_back(slovo);
        }
        l = new_right;
    }
    vector<string> itog;
    int k=0;
    for(int i= slova.size()-1; i>=0; --i){
        if (slova[i]==".."){
            ++k;
        }
        else if(slova[i]=="."){
            continue;
        }
        else{
            if (k==0){
                itog.push_back(slova[i]);
            }
            else{
                --k;
            }
        }
    }
    reverse(itog.begin(), itog.end());
    if (path[0] != '/') {
        while (k > 0) {
            itog.insert(itog.begin(),"..");
            --k;
        }
    }
    //reverse(itog.begin(), itog.end());
    if (path[0]=='/'){
        s+= "/";
    }
    for (size_t i=0; i<itog.size(); ++i){
        if (i!=itog.size()-1){
            s+=itog[i];
            s+= "/";
        }
        else{
            s+= itog[i];
        }
    }
    if (s.size()==0){
        return ".";
    }
    return s;
}
