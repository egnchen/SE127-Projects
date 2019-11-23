#include "pstate.h"

namespace EBasic {

int PState::getOne(const std::string &n)
{
    if(state.find(n) == state.end())
        throw std::runtime_error("Variable " + n + " not declared.");
    return state[n];
}

void PState::setOne(const std::string &n, int v)
{
    state[n] = v;
}
}
