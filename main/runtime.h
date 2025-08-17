#ifndef SG_RUNTIME_H
#define SG_RUNTIME_H

#include "External/Singleton.h"

#include <atomic>
#include <cstdint>

enum class RuntimeResult
{
    kNone                   = 0,
    kSuccess                = 1,
    kPause                  = 2,
    kQuit                   = 3
};

class Runtime : public Common::Singleton<Runtime> {
friend class Common::Singleton<Runtime>;

private:
    ///////////////////////////////////////////////
    std::atomic<bool> atomic_runing_ = false;
    uint8_t check_runing_framcount_{};
    //////////////////////////////////////////////
private:
    Runtime() = default;
public:
    bool Init();
    void Tick();
    void Quit();
    inline bool ShouldExit() {
        return runing_;
    }

    //Async Use
    void Pause();
private:
    //Main Thread
    bool runing_ = false;
};




#endif