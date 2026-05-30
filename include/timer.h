#pragma once

#include <chrono>
#include <print>
#include <string>

class Timer
{
public:
    Timer(const std::string& Message) :
        Message(Message), StartTime(std::chrono::steady_clock::now()) {};
    ~Timer()
    {
        std::println("{}: {}ms.", Message, Elapsed());
    };

    float Elapsed() const
    {
        auto EndTime = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> Duration = EndTime - StartTime;
        return Duration.count();
    }

    float ElapsedPrint() const
    {
        auto Duration = Elapsed();
        std::println("{}: {}ms.", Message, Duration);
        return Duration;
    }

    float ElapsedUpdate()
    {
        auto EndTime = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> Duration = EndTime - StartTime;
        StartTime = EndTime;
        return Duration.count();
    }

    float ElapsedUpdatePrint()
    {
        auto Duration = ElapsedUpdate();
        std::println("{}: {}ms.", Message, Duration);
        return Duration;
    }

private:
    std::string Message;
    std::chrono::time_point<std::chrono::steady_clock> StartTime;
};
