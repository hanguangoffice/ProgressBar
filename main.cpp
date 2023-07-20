#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <iomanip>
#include <random>

using namespace std;
using namespace chrono;

int progress = 0;                           // 定义一个全局变量，用于记录任务的进度
int tasks = 1170;                          // 定义一个常量，表示任务的总数
mutex mtx;                                  // 定义一个互斥锁，用于保护进度变量
default_random_engine engine(time(nullptr));// 随机数引擎，基于当前时间初始化
const string hide_cursor = "\x1B[?25l";     // ANSI 控制字符显示/隐藏光标
const string show_cursor = "\x1B[?25h";

// 用于模拟执行一个耗时的任务，并在完成后更新进度变量
void task(int id)
{
    uniform_int_distribution<int> distribution(0, 10000);
    //cout << "Task " << id << " started\n";                   // 输出任务开始的信息
    this_thread::sleep_for(milliseconds(distribution(engine)));// 模拟任务执行，暂停时间
    mtx.lock();                                                // 使用互斥锁保护进度变量
    progress++;                                                // 将进度变量加
    mtx.unlock();                                              // 释放互斥锁
    //cout << "Task " << id << " finished\n";                  // 输出任务结束的信息
}

// 用于显示一个动态的进度条，并根据进度变量的值更新百分比
void bar()
{
    cout << "\n" << hide_cursor;
    float percentage = 0;                                         // 定义一个局部变量，用于记录当前的百分比
    while (percentage < 100)                                  // 循环直到百分比达到100%
    {
        mtx.lock();                                               // 使用互斥锁保护进度变量
        percentage = (static_cast<float>(progress) / tasks) * 100;    // 根据进度变量计算百分比
        mtx.unlock();                                             // 释放互斥锁
        cout << "\r";
        for (int i = 0; i < (int)percentage; i++)                 // 输出方块符号，表示已完成的部分
            cout << "▌";
        for (int i = (int)percentage; i < 100; i++)               // 输出横线符号，表示未完成的部分
            cout << "-";
        cout << fixed << setprecision(1) << percentage << "%";    // 输出百分比和百分号，并保留一位小数点
        cout.flush();                                             // 清空缓冲区，确保输出及时和完整
        this_thread::sleep_for(milliseconds(10));                 // 暂停线程10毫秒，模拟进度条刷新率
        if (percentage == 100)                                    // 如果百分比达到100%，跳出循环
            break;
    }
    cout << show_cursor;
}

int main()
{
    vector<thread> threads;                     // 定义一个向量，用于存储线程对象
    threads.emplace_back(bar);                  // 创建另一个线程，执行bar函数
    for (int i = 1; i <= tasks; i++)            // 循环创建多个线程，执行task函数，并传入不同的参数
        threads.emplace_back(task, i);
    for (auto& t : threads)                     // 循环等待所有线程结束
        t.join();
    cout << "\nAll tasks completed\n";
    return 0;
}
