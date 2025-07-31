#pragma once
#include <thread>
#include <vector>
#include <functional>

class TransferThreader
{
private:
    std::vector<std::thread> threads;

public:
    void startThread(std::function<void()> task);
    void joinAll();
};