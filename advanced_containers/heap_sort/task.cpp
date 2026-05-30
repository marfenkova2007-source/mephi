#include "task.h"

using namespace std;

vector<int> HeapSort(const vector<int>& v) {
    priority_queue<int> mas;  
    
    for (int i : v) {
        mas.push(i);
    }
    vector<int> res(v.size());
    int i=v.size()-1;
    while (i>=0) {
        res[i] = mas.top();
        mas.pop();
        --i;
    }
    return res;
}
