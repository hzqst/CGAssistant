#pragma once

namespace timax { namespace rpc 
{
	template <typename CodecPolicy>
	class sync_client
	{
		using codec_policy = CodecPolicy;
		using async_client_t = async_client<codec_policy>;
		using async_client_ptr = std::shared_ptr<async_client_t>;
		
	public:
		sync_client()
			: client_(std::make_shared<async_client_t>())
		{
		}

		template <typename Protocol, typename ... Args>
		auto call(duration_t const& duration, tcp::endpoint const& endpoint, Protocol const& protocol, Args&& ... args)
		{
			using result_type = typename Protocol::result_type;
			return call_impl(duration, std::is_void<result_type>{}, endpoint, protocol, std::forward<Args>(args)...);
		}

	private:
		template <typename Protocol, typename ... Args>
		auto call_impl(duration_t const& duration, std::false_type, tcp::endpoint const& endpoint, Protocol const& protocol, Args&& ... args)
		{
			using result_type = typename Protocol::result_type;
			auto task = client_->call(endpoint, protocol, std::forward<Args>(args)...);
			return task.get(duration);
		}

		template <typename Protocol, typename ... Args>
		auto call_impl(duration_t const& duration, std::true_type, tcp::endpoint const& endpoint, Protocol const& protocol, Args&& ... args)
		{
			auto task = client_->call(endpoint, protocol, std::forward<Args>(args)...);
			task.wait(duration);
		}

	private:
		async_client_ptr		client_;
	}; 
} }