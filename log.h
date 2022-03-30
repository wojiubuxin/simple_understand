#ifndef LOG_H
#define LOG_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <chrono>
#include <time.h>
#include <sstream>
namespace simple
{
	class log
	{
	public:
		
		~log();
		static log& getlog_instance()
		{
			static log log_instance;
			return log_instance;
		}

		struct log_data {
			std::string filename;
			std::string data;
		};

		template<typename... T >
		void log_print(const std::string& filename, T...args)
		{
			log_data temp;
			temp.filename = filename + "_" + get_now_time_flag() + ".log";

			temp.data = get_now_time_flag(2) + log_pre_end(args...) + "\n";

			//std::cout << temp.data.c_str() << std::endl;

			std::unique_lock<std::mutex>lock(log_mutex);
			pro_deque.push_back(std::move(temp));
			buffer_empty.notify_all();
		}

		template<typename headstr,typename... T>
		std::string log_pre_end(headstr headstr, T...args)
		{
			stringstream ss;
			ss <<'|'<< headstr;
			
			ss << log_pre_end(args...);
			return  ss.str();
		}


		template<typename T>
		std::string log_pre_end(T arg)
		{
			stringstream ss;
			ss << '|'<<arg;
			return ss.str();
		}

	private:
		log();
		std::mutex log_mutex;
		std::condition_variable buffer_empty;
		std::unique_ptr<std::thread>log_write;

		std::string get_now_time_flag(int flag_file_name = 1);
		int time_gap;
		bool run;

		void threadfunc();
		void stop_thread_write();

		//Éú²úµÄ
		std::deque<log_data>pro_deque;
		
		//ÒªÂäµØµÄ
		std::deque<log_data>con_deque;


	};

	#define loger log::getlog_instance().log_print
	#define logger log::getlog_instance().log_print

}
#endif
