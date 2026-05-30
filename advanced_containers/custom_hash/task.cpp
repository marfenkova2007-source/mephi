#include "task.h"

using namespace std;

void PopulateHashMap(
    unordered_map<SuperKey, string>& hashMap,
    const vector<pair<SuperKey, string>>& toAdd
) {
    for (size_t i = 0; i<toAdd.size(); ++i){
        hashMap[toAdd[i].first]= toAdd[i].second;
    }
}
