#include "atom.h"
// Constructor
Atom::Atom() : aCurrentEBO(&aEBO){};

void Atom::BindAttributes() {
    aVAO.Bind();
    aCurrentEBO->Bind();
}
void Atom::UnbindAttributes() {
    aCurrentEBO->Unbind();
    aVAO.Unbind();
}
void Atom::Draw() {
    aCurrentEBO->Draw();
}

Atom::~Atom(){}