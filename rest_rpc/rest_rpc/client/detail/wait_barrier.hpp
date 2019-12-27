#pragma once 

namespace timax { namespace rpc
{
	class result_barrier
	{
	public:
		void wait()
		{
			using namespace std::chrono_literals;
			lock_t locker{ mutex_ };
			cond_var_.wait(locker, [this] { return is_over_; });
		}

		void notify()
		{
			lock_t lock{ mutex_ };
			is_over_ = true;
			cond_var_.notify_one();
		}

	protected:
		bool							is_over_ = false;
		std::mutex					mutex_;
		std::condition_variable		cond_var_;
	};
} }