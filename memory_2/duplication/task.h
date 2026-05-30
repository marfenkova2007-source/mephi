#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <sstream>
#include <optional>
#include <memory>
#include <unordered_map>

using namespace std;

vector<shared_ptr<string>> DeDuplicate(const vector<unique_ptr<string>>& in){
    vector<shared_ptr<string>> out;
    unordered_map<string, shared_ptr<string>> slov;
    for (auto& x: in){
        if (slov.find(*x)==slov.end()){
            slov[*x] = make_shared<string>(*x);
        }
        out.push_back(slov[*x]);
    }
    return out;

}

vector<unique_ptr<string>> Duplicate(const vector<shared_ptr<string>>& in) {
    vector<unique_ptr<string>> out;  
    for (auto& shared_str : in) {
        out.push_back(make_unique<string>(*shared_str));
    }
    return out;
}