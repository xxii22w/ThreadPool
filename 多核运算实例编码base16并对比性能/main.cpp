#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <execution>
using namespace std;
using namespace chrono;

static const char base16[] = "0123456789abcdef";

// 就是将一个字节拆分成两个
void Base16Encode(const unsigned char* data, int size, unsigned char* out)
{
	for (int i = 0; i < size; i++)
	{
		unsigned char d = data[i];
		// 0000 0000
		// 1234 5678 >> 4 0000 1234
		// 1234 5678 & 0000 1111 0000 5678
		char a = base16[d >> 4];
		char b = base16[d & 0x0f];
		out[i * 2] = a;
		out[i * 2 + 1] = b;
	}
}

// c++ 多核base16编码
void Base16EncodeThread(const vector<unsigned char>& data, vector<unsigned char>& out)
{
	int size = data.size();
	int th_count = thread::hardware_concurrency();	// 系统支持的线程核心数
	// 切片数组
	int slice_count = size / th_count;	// 余数丢弃
	if (size < th_count)
	{
		th_count = 1;
		slice_count = size;
	}

	// 准备好线程
	vector<thread> ths;
	ths.resize(th_count);

	// 任务分割到每个线程
	for (int i = 0; i < th_count; i++)
	{
		int offset = i * slice_count;
		int count = slice_count;
		// 最后一个线程
		if (th_count > 1 && 1 == th_count - 1)
		{
			count = slice_count + size % th_count;
		}
		ths[i] = thread(Base16Encode, data.data() + offset, count, out.data());
	}
	//等待所有线程处理结束
	for (auto& th : ths)
		th.join();
}

int main()
{
	// 初始化测试数据
	vector<unsigned char> in_data;
	in_data.resize(1024 * 1024 * 20); //20M
	// 初始化数组元素
	for (auto& byte : in_data) {
		byte = static_cast<unsigned char>(rand() % 256); // 初始化数组为随机值
	}
	vector<unsigned char > out_data;
	out_data.resize(in_data.size() * 2);

	// 测试单线程base16编码效率
	{
		cout << "单线程base16开始计算" << endl;
		auto start = system_clock::now();
		Base16Encode(in_data.data(), in_data.size(), out_data.data());
		auto end = system_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		cout << "编码：" << in_data.size() << "字节数据花费"
			<< double(duration.count()) << "毫秒" << endl;

	}

	//测试c++11 多线程Base16编码
	{
		cout << "c++11 多线程Base16编码 开始计算" << endl;
		auto start = system_clock::now();
		Base16EncodeThread(in_data, out_data);
		auto end = system_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		cout << "编码：" << in_data.size() << "字节数据花费"
			<< double(duration.count()) << "毫秒" << endl;
	}

	//测试C++17 多线程base16编码
	{
		cout << "C++17 多线程base16编码 开始计算" << endl;
		auto start = system_clock::now();
		unsigned char* idata = in_data.data();
		unsigned char* odata = out_data.data();
		for_each(std::execution::par, in_data.begin(), in_data.end(),
			[&](auto& d)	// 多线程进入此函数
			{
				char a = base16[(d >> 4)];
				char b = base16[(d & 0x0F)];
				int index = &d - idata;
				odata[index * 2] = a;
				odata[index * 2 + 1] = b;
			}
		);
		auto end = system_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		cout << "编码：" << in_data.size() << "字节数据花费"
			<< double(duration.count()) << "毫秒" << endl;
	}
	getchar();
	return 0;
}