#ifndef CB_TIME_H
#define CB_TIME_H
#include <map>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <string>
namespace simple
{

	class cb_time
	{
	public:
		typedef std::function<void()> time_func_cb;

		~cb_time();
		static cb_time& get_cb_time_instance()
		{
			static cb_time cb_time_instance;
			return cb_time_instance;
		}
		void insert_time_cb(double gap, time_func_cb cb,  std::string cb_name);
		void del_time_cb(std::string cb_name);
	private:
		cb_time();
		struct time_data {
			double gap; //Ҫ�����������λ,����0.5����5��
			time_func_cb cb;
			std::string cb_name;
		};

		//target_timeҪִ�е�ʱ���
		std::multimap<int64_t, time_data> cb_list;

		//����ϵͳ���̵߳�������ʱ��
		std::unique_ptr<std::thread>run_cb;

		std::mutex cb_mutex;
		std::condition_variable cb_sleep;

		bool run_flag_cb;

		void threadfunc_cb();
		void stop_thread_cb();

	};

	//#չ������������ַ������,����ɾ��,�����ⲿҲ���ù��Ĵ��ַ�������
	#define SetTimeCB(t, f) cb_time::get_cb_time_instance().insert_time_cb(t, f , #f)
	#define KillTimeCB(f) cb_time::get_cb_time_instance().del_time_cb(#f)
}

#endif