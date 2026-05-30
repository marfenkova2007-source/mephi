#include "tasks.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

void NPointers::Increment(int *n){
    if (n!=nullptr){
        *n+=1;
    }
    return ;
}

int NPointers::Multiply(int a, int b, bool *flag){
    *flag = false;
    if ((a>0 and b>0) or (a<0 and b<0)){
        if (a*b<0 and flag!=nullptr){
            *flag = true;
        }
    }
    if ((a>0 and b<0) or (a<0 and b>0)){
        if (a*b>0 and flag!=nullptr){
            *flag = true;
        }
    }
    return a*b;
}

int NPointers::ScalarProduct(int const mas1[], int const mas2[], int n){
    int res = 0;
    for (int i=0; i<n; ++i){
        res+= mas1[i] * mas2[i];
    }
    return res;
}

int NPointers::SizeOfMaximumSquareOfCrosses(char const* mas, int n, int m){
    int mm = 0;
    vector<vector<int>> dp(n, vector<int> (m, 0));
    for (int i=0; i<n; ++i){
        for (int j=0; j<m; ++j){
            if (mas[i * m + j]=='+'){
                if (i==0 or j==0){
                    dp[i][j]=1;
                    mm = max(1, mm);
                }
                else{
                    dp[i][j] = min(dp[i-1][j], min(dp[i][j-1], dp[i-1][j-1])) + 1;
                    mm = max(mm, dp[i][j]);
                }
            }
        }
    }
    return mm;
}

long long *NPointers::MultiplyToLongLong(int a, int b){
    long long *c = new long long;
    *c =(long long)a*b;
    return c;
}

int NReferences::MultiplyInplace(int &n, int a){
    n = n*a;
    return n;
}

int NReferences::CompareArraysByAverage(const int mas[], int n, int const m[]){
    float s1 = 0, s2 = 0;
    float k2 = 5.0;
    for (int i=0; i<n; ++i){
        s1+= mas[i];
    }
    for (int i=0; i<5;++i){
        s2+= m[i];
    }
    if (s1/n<s2/k2){
        return -1;
    }
    else{
        if (s1/n == s2/k2){
            return 0;
        }
        else{
            return 1;
        }
    }
}