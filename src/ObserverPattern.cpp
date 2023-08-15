#include "ObserverPattern.h"

void Subject::Attach(Observer *pObs) {
    aObservers.push_back(pObs);
}

void Subject::Notify() {
    for (int i = 0; i<aObservers.size(); i++) {
        aObservers[i]->Update(this);
    }
}