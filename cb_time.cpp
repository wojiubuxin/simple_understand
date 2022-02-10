#include "cb_time.h"

using namespace simple;
cb_time::cb_time():run_flag_cb(true)
{
	auto test_ptr = std::bind(&cb_time::threadfunc_cb, this);
	std::unique_ptr<std::thread> temp(new std::thread(test_ptr));
	run_cb = std::move(temp);

}

cb_time::~cb_time()
{
	if (run_flag_cb)
	{
		stop_thread_cb();
	}
}

void cb_time::stop_thread_cb()
{
	{
		std::unique_lock<std::mutex>lock(cb_mutex);
		run_flag_cb = false;
		cb_sleep.notify_all();
	}

	run_cb->join();
}

//�ص�����
void cb_time::threadfunc_cb()
{
	while (run_flag_cb)
	{

		{
			std::unique_lock<std::mutex>lock(cb_mutex);
			
			while (cb_list.size() < 1&& run_flag_cb)
			{
				cb_sleep.wait(lock);
			}

			//����
			int64_t flag_sleep_gap = 600 * 1000;
			bool break_flag_cb = false;

			while (cb_list.size() > 0 && run_flag_cb && break_flag_cb == false)
			{
				for (auto iter = cb_list.begin(); iter != cb_list.end();)
				{
					//��ȡ��ǰʱ�����ʱ���
					int64_t now_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

					if (iter->first <= now_time)
					{
						iter->second.cb();
						cb_list.erase(iter++);
					}
					else
					{
						//�������µȴ�ʱ��
						flag_sleep_gap =  iter->first - now_time;
						break_flag_cb = true;
						//���涼��û��ʱ��ģ���������
						break;
					}
				}

			}
			
			if (run_flag_cb)
			{
				cb_sleep.wait_for(lock, std::chrono::milliseconds(flag_sleep_gap));
			}
			
		}

	}
	
	cb_list.clear();
}

//���붨ʱ���ӿ�
void cb_time::insert_time_cb(double gap, time_func_cb cb, std::string cb_name)
{
	if (cb)
	{
		
		int64_t now_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		time_data cb_data;

		if (gap < 0.001)
		{
			gap = 0.001;
		}
		cb_data.gap = gap;
		cb_data.cb = std::move(cb);
		cb_data.cb_name = cb_name;
		int64_t cb_time = now_time + cb_data.gap * 1000;
		std::unique_lock<std::mutex>lock(cb_mutex);
		cb_list.insert(std::make_pair(cb_time, cb_data));
		cb_sleep.notify_all();
	}
}

//ɾ����ʱ��
void cb_time::del_time_cb(std::string cb_name)
{
	if (cb_name.size()>0)
	{
	
		std::unique_lock<std::mutex>lock(cb_mutex);
		bool flag = false;
		for (auto iter = cb_list.begin(); iter != cb_list.end();)
		{
			std::cout << "test-1 " << iter->second.cb_name.c_str() << " " << cb_name.c_str() << std::endl;
			if (iter->second.cb_name.compare(cb_name) == 0)
			{
				cb_list.erase(iter++);
				flag = true;
			}
			else
			{
				iter++;
			}
		}

		if (flag)
		{
			cb_sleep.notify_all();
		}

	}
}