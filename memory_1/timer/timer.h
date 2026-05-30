#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

class Timer {
private:
    steady_clock::time_point timer_time;
public:
    Timer(milliseconds time){
        auto now = steady_clock::now();
        timer_time = now + time;

    }
    bool Expired() const{
        auto now = steady_clock::now();
        if (now<timer_time){
            return false;
        }
        else{
            return true;
        }


    };
};

class TimeMeasurer {
private:
    steady_clock::time_point start;
    ostream* output;
public:
    TimeMeasurer(ostream& write = cout){
        output = &write;
        start = steady_clock::now();
    }
    ~TimeMeasurer(){
        auto end = steady_clock::now();
        auto otvet = end - start;
        milliseconds itog = duration_cast<milliseconds>(otvet);
        *output << "Elapsed time: " << itog.count()<<endl;
    }

    
};
