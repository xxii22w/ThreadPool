#include "xmsg_server.h"
#include <sstream>
using namespace std;

int main()
{
	xmsg_server server;
	server.Start();
	for (int i = 0; i < 10; i++)
	{
		stringstream ss;
		ss << "msg : " << i << endl;
		server.SendMsg(ss.str());
		this_thread::sleep_for(500ms);
	}
	server.Stop();
	return 0;
}