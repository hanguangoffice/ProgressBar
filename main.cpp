#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <iomanip>
#include <random>

using namespace std;
using namespace chrono;

int progress = 0;                           // ����һ��ȫ�ֱ��������ڼ�¼����Ľ���
int tasks = 1170;                          // ����һ����������ʾ���������
mutex mtx;                                  // ����һ�������������ڱ������ȱ���
default_random_engine engine(time(nullptr));// ��������棬���ڵ�ǰʱ���ʼ��
const string hide_cursor = "\x1B[?25l";     // ANSI �����ַ���ʾ/���ع��
const string show_cursor = "\x1B[?25h";

// ����ģ��ִ��һ����ʱ�����񣬲�����ɺ���½��ȱ���
void task(int id)
{
    uniform_int_distribution<int> distribution(0, 10000);
    //cout << "Task " << id << " started\n";                   // �������ʼ����Ϣ
    this_thread::sleep_for(milliseconds(distribution(engine)));// ģ������ִ�У���ͣʱ��
    mtx.lock();                                                // ʹ�û������������ȱ���
    progress++;                                                // �����ȱ�����
    mtx.unlock();                                              // �ͷŻ�����
    //cout << "Task " << id << " finished\n";                  // ��������������Ϣ
}

// ������ʾһ����̬�Ľ������������ݽ��ȱ�����ֵ���°ٷֱ�
void bar()
{
    cout << "\n" << hide_cursor;
    float percentage = 0;                                         // ����һ���ֲ����������ڼ�¼��ǰ�İٷֱ�
    while (percentage < 100)                                  // ѭ��ֱ���ٷֱȴﵽ100%
    {
        mtx.lock();                                               // ʹ�û������������ȱ���
        percentage = (static_cast<float>(progress) / tasks) * 100;    // ���ݽ��ȱ�������ٷֱ�
        mtx.unlock();                                             // �ͷŻ�����
        cout << "\r";
        for (int i = 0; i < (int)percentage; i++)                 // ���������ţ���ʾ����ɵĲ���
            cout << "��";
        for (int i = (int)percentage; i < 100; i++)               // ������߷��ţ���ʾδ��ɵĲ���
            cout << "-";
        cout << fixed << setprecision(1) << percentage << "%";    // ����ٷֱȺͰٷֺţ�������һλС����
        cout.flush();                                             // ��ջ�������ȷ�������ʱ������
        this_thread::sleep_for(milliseconds(10));                 // ��ͣ�߳�10���룬ģ�������ˢ����
        if (percentage == 100)                                    // ����ٷֱȴﵽ100%������ѭ��
            break;
    }
    cout << show_cursor;
}

int main()
{
    vector<thread> threads;                     // ����һ�����������ڴ洢�̶߳���
    threads.emplace_back(bar);                  // ������һ���̣߳�ִ��bar����
    for (int i = 1; i <= tasks; i++)            // ѭ����������̣߳�ִ��task�����������벻ͬ�Ĳ���
        threads.emplace_back(task, i);
    for (auto& t : threads)                     // ѭ���ȴ������߳̽���
        t.join();
    cout << "\nAll tasks completed\n";
    return 0;
}
