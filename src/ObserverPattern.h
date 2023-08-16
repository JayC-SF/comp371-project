#ifndef OBSERVER_PATTERN_H
#define OBSERVER_PATTERN_H
#include <vector>
using namespace std;
class Observer;

class Subject {
    public:
    vector<Observer * > aObservers;
    void Attach(Observer * pObs);
    void Notify();
};

class Observer {
    public:
    virtual void Update(Subject * pSubject) = 0;

};
#endif