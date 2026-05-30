#pragma once

namespace NPointers {
    /* To implement:
    Increment
    Multiply
    ScalarProduct
    SizeOfMaximumSquareOfCrosses
    MultiplyToLongLong
    */
    void Increment(int *n);
    int Multiply(int a, int b, bool *flag);
    int ScalarProduct(int const mas1[], int const mas2[], int n);
    int SizeOfMaximumSquareOfCrosses(char const* mas, int n, int m);
    long long* MultiplyToLongLong(int a, int b);
}

namespace NReferences {
    /* To implement:
    MultiplyInplace
    CompareArraysByAverage
    */
    int MultiplyInplace(int &n, int a);
    int CompareArraysByAverage(const int mas[], int n, int const m[]);
}
