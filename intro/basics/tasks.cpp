#include "tasks.h"

#include <cmath>

using namespace std;

int NOuter::NInner::DoSomething(int lhs, int rhs) {
    int res = lhs + rhs;
    return res;
}

int NOuter::DoSomething(int lhs, int rhs) {
    return lhs-rhs;
}

int NOverload::ProcessTwoArgs(int lhs, int rhs) {
    return lhs + rhs;
}

char NOverload::ProcessTwoArgs(char lhs, char rhs) {
    return max(lhs, rhs);
}

int NOverload::ProcessTwoArgs(int lhs, char rhs) {
    return lhs - rhs;
}

unsigned int NOverflow::WithOverflow(int lhs, int rhs) {
    return lhs  + rhs;
}

uint64_t NOverflow::WithOverflow(int64_t lhs, int64_t rhs) {
    return lhs-rhs;
}

int NLoop::SumInRange(const int lhs, const int rhs) {
    int s = 0;
    for (int i=lhs; i<rhs; ++i){
        s += i;
    }
    return s;
}

int NLoop::CountFixedBitsInRange(const int from, const int to, const int bitsCnt) {
    int k = 0;
    for (int i=from; i<=to; ++i){
        int a = i;
        int count = 0;
        while (a/2!=0) {
            count += a%2;
            a/= 2;
        }
        count+= a%2;
        if (count==bitsCnt){
            ++k;
        }
    }
    return k;
}
//`|(sin(a) / 2 + cos(a)) * (sin(a) / 2 + cos(a)) + tan(a) * atan(a)|`
double NMath::ComputeMathFormula(const double arg) {
    return abs((sin(arg)/2 +cos(arg))*(sin(arg)/2 + cos(arg)) + tan(arg) * atan(arg));
}

bool NMath::IsPositive(int arg) {
    return arg>0;
}

int NRecursion::CalculateFibonacci(const int arg) {
    long long a = 0, b = 1, c;
    if (arg==1){
        return 0;
    }
    else if(arg==1){
        return 1;
    }
    else{
        for(int i=2; i<=arg; ++i){
            c = a+b;
            a = b;
            b = c;
        }
    }
    return c;
}
