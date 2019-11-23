#ifndef PSTATE_H
#define PSTATE_H

#include <map>
#include <string>

namespace EBasic {

class PState
{
private:
    unsigned line_cnt;
    unsigned jump_dest;
    bool running;
    std::map<std::string, int> state;

public:
    PState():line_cnt(0), jump_dest(0), running(false) {}
    virtual ~PState() {}

    void setOne(const std::string&, int);
    int getOne(const std::string&);
    void jump(unsigned dest) { jump_dest = dest;}
    void proceed(unsigned next) { line_cnt = next; jump_dest = 0; }

    bool hasJumped() { return jump_dest != 0; }
    unsigned getJumpDest() { return jump_dest; }
    unsigned getLine() { return line_cnt; }
    bool isRunning() { return running; }
    void end() {running = false;}
    const std::map<std::string, int> &getVars() { return state; }

    void start() {running = true;}
    void clear() { line_cnt = 0; running = false; state.clear(); }
};

}

#endif // PSTATE_H
