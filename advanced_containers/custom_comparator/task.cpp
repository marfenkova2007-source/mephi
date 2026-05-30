#include "task.h"

bool operator <(const SuperKey& lhs, const SuperKey& rhs) {
    if (lhs.StrPart!=rhs.StrPart){
        return lhs.StrPart<rhs.StrPart;
    }
    if (lhs.IntPart!=rhs.IntPart){
        return lhs.IntPart<rhs.IntPart;
    }
    return lhs.FloatPart<rhs.FloatPart;
}

void PopulateMap(
    std::map<SuperKey, std::string>& map,
    const std::vector<std::pair<SuperKey, std::string>>& toAdd
) {
    for (size_t i = 0; i<toAdd.size();++i){
        map[toAdd[i].first] = toAdd[i].second;
    }
}
