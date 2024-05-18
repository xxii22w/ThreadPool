#include "xmsg_server.h"
#include <iostream>
using namespace std;

void xmsg_server::SendMsg(std::string msg)
{
	unique_lock<mutex> lock(mux_);
	msgs_.push_back(msg);
}

void xmsg_server::Main()
{
	while (!is_exit())
	{
		this_thread::sleep_for(10ms);
		unique_lock<mutex> lock(mux_);
		if (msgs_.empty())
			continue;
		while (!msgs_.empty())
		{
			// 消息处理业务逻辑
			cout << "recv : " << msgs_.front() << endl;
			msgs_.pop_front();
		}
	}
}
