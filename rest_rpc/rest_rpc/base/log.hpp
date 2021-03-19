#pragma once
#include <spdlog/spdlog.h>

namespace timax
{
	class log
	{
	public:
		static log& get()
		{
			static log _log;
			return _log;
		}

		bool init(const std::string& file_name)
		{
			try
			{
				log_ = spdlog::rotating_logger_mt("logger", file_name, 1024 * 1024 * 50, 2);
				console_log_ = spdlog::stdout_logger_mt("console");
			}
			catch (std::exception&)
			{
				return false;
			}
			catch (...)
			{
				return false;
			}

			return true;
		}

		std::shared_ptr<spdlog::logger> get_log()
		{
			return log_;
		}

		std::shared_ptr<spdlog::logger> get_console_log()
		{
			return console_log_;
		}
	private:
		log() = default;
		log(const log&) = delete;
		log(log&&) = delete;

		std::shared_ptr<spdlog::logger> log_;
		std::shared_ptr<spdlog::logger> console_log_;
	};


	template<typename... Args>
	static inline void SPD_LOG_TRACE(const char* fmt, const Args&... args)
	{
		log::get().get_log()->trace(fmt, args...);
	}

	template<typename... Args>
	static inline void SPD_LOG_INFO(const char* fmt, const Args&... args)
	{
		log::get().get_log()->info(fmt, args...);
	}

	//template<typename... Args>
	//static inline void SPD_LOG_NOTICE(const char* fmt, const Args&... args)
	//{
	//	log::get().get_log()->notice(fmt, args...);
	//}

	template<typename... Args>
	static inline void SPD_LOG_WARN(const char* fmt, const Args&... args)
	{
		log::get().get_log()->warn(fmt, args...);
	}

	template<typename... Args>
	static inline void SPD_LOG_ERROR(const char* fmt, const Args&... args)
	{
		log::get().get_log()->error(fmt, args...);
	}

	template<typename... Args>
	static inline void SPD_LOG_CRITICAL(const char* fmt, const Args&... args)
	{
		log::get().get_log()->critical(fmt, args...);
	}

	//template<typename... Args>
	//static inline void SPD_LOG_ALERT(const char* fmt, const Args&... args)
	//{
	//	log::get().get_log()->alert(fmt, args...);
	//}

	//template<typename... Args>
	//static inline void SPD_LOG_EMERG(const char* fmt, const Args&... args)
	//{
	//	log::get().get_log()->emerg(fmt, args...);
	//}

	template<typename... Args>
	static inline void SPD_LOG_DEBUG(const char* fmt, const Args&... args)
	{
#ifdef NDEBUG

#else
		log::get().get_log()->set_level(spdlog::level::debug);
		log::get().get_log()->debug(fmt, args...);

		log::get().get_console_log()->set_level(spdlog::level::debug);
		log::get().get_console_log()->debug(fmt, args...);
#endif
	}
}