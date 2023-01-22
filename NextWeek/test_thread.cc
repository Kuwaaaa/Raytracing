// Compiler: MSVC 19.29.30038.1
// C++ Standard: C++17
#include <iostream>
#include <future>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <memory>

using namespace std;

// 这个文件想找到一个可以同时让多个线程一起工作的方法。

void count_big_number() {
	// C++14标准中，可以在数字中间加上单
	// 引号 ' 来分隔数字，使其可读性更强
	for (int i = 0; i <= 10'0000'0000; i++);
}
int main() {
    
	// future<void> fut = async(launch::async, count_big_number);
	// cout << "Please wait" << flush;
	// // 每次等待1秒
	// while (fut.wait_for(chrono::milliseconds(1000)) != future_status::ready)
	// 	cout << '.' << flush;
	// cout << endl << "Finished!" << endl;

	// thread a([]() { cout<<"Hello\n";});
	// a.join();

	// for(int i=0; i<1000000; i++)
	// ;

	// cout <<"sb\n";

	// mutex mut;
	
	// auto func = [&](int& index) {
	// 	lock_guard<mutex> mtx_locker(mut);
	// 	index++;
	// 	cout << index << " ";
	// };

	// int i = 0;
	// for(int j=0;j<100;j++)
	// 	{
	// 		thread a(func, ref(i));
	// 		a.join();
	// 		cout << "(j: " << j << " )"; 
	// 	}

	// cout << "\n\n"<<endl;
	// mut.lock();
	// for(int j=0; j<100000000; j++)
	// ;
	// mut.unlock();
	// cout<<"\n\n###########"<<endl;

	
	mutex mut;
	int sem = 0;
	auto func1 = [&](){
		cout << "func1" << endl;
		lock_guard<mutex> mtx_locker(mut);
		this_thread::sleep_for(chrono::milliseconds(500));

	};

	auto func2 = [](int & sem) {
		sem++;
		cout << "sem: "<< sem << endl;
		this_thread::sleep_for(chrono::seconds(2));
		sem--;
		cout << "sem --" << endl;
	};

	// 想通过用一个线程控制另一堆线程的生存周期
	auto func3 = [&]() {
		for(int i=0; i<5; i++)
		{
			thread t(func1);
			t.join();
		}
	};

	auto start = chrono::high_resolution_clock::now();
	cout << "Titok start " << endl;

	// future<void> fut[5];
	// for(int i=0; i<5; i++)
	// 	fut[i] = async(launch::async, func2, ref(sem));
		
	// while (sem != 0)
	// {
	// 	/* code */
	// }
	

	// for(int i=0; i<5; i++)
	// 	{
	// 		thread t(func2, ref(sem));
	// 		// t.join();
	// 		t.detach();
	// 	}
	// sem+=100;
	
	// do
	// {	
	// 	cout << "thread start "<<endl;
	// 	thread t(func2, ref(sem));
	// 	cout << "join"<<endl;
	// 	t.join();
		
	// } while (sem != 0);

	// thread a(func3);
	// a.join();

	auto end = chrono::high_resolution_clock::now();
	cout << "run time: " << chrono::duration_cast<chrono::microseconds>(end - start).count()<<endl;

	cout << "core : " << thread::hardware_concurrency() << endl;

	return 0;
}