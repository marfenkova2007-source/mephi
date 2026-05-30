#include "task.h"

void ReverseList(std::list<int>& l) {
    std::list<int> mas;
    auto end = l.end();
    auto begin = l.begin();
    if (begin==end){
        return ;
    }
    while (begin!=end and begin!=--end){
        std::swap(*begin, *end);
        ++begin;
    }
    
}
