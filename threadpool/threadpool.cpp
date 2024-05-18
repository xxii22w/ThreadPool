#include "xthread_pool.h"
#include <iostream>
using namespace std;

class MyTask : public XTask
{
public:
    int Run()
    {
        cout << "================================================" << endl;
        cout << this_thread::get_id() << " Mytask " << name << endl;
        cout << "================================================" << endl;
        for (int i = 0; i < 10; i++)
        {
            if (is_exit())break;
            cout << "." << flush;
            this_thread::sleep_for(500ms);
        }
        return 100;
    }
    std::string name = "";
};

int main()
{
    XThread_Pool pool;
    pool.Init(16);
    pool.Start();

    {
        auto task3 = make_shared<MyTask>();
        task3->name = "test shared 003";
        pool.AddTask(task3);

        auto task4 = make_shared<MyTask>();
        task4->name = "test shared 004";
        pool.AddTask(task4);

        auto re = task4->GetReturn();
        cout << "task4->GetReturn() = " << re << endl;
    }
    this_thread::sleep_for(100ms);
    cout << "task run  count = " << pool.task_run_count() << endl;


    this_thread::sleep_for(1s);
    pool.Stop();
    cout << "task run  count = " << pool.task_run_count() << endl;

    getchar();
    return 0;
}