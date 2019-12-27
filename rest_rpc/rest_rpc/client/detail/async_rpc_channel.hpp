#pragma once

namespace timax { namespace rpc
{
	template <typename CodecPolicy>
	class rpc_manager;

	template <typename CodecPolicy>
	class rpc_channel : public std::enable_shared_from_this<rpc_channel<CodecPolicy>>
	{
		enum class status_t
		{
			stopped,
			ready,
			running,
		};

	public:
		using codec_policy = CodecPolicy;
		using rpc_manager_t = rpc_manager<codec_policy>;
		using rpc_call_container_t = rpc_call_container<codec_policy>;
		using context_t = typename rpc_call_container_t::context_t;
		using context_ptr = typename rpc_call_container_t::context_ptr;
		using call_list_t = typename rpc_call_container_t::call_list_t;
		using call_map_t = typename rpc_call_container_t::call_map_t;
			
	public:
		inline rpc_channel(rpc_manager_t& mgr, io_service_t& ios, tcp::endpoint const& endpoint);
		inline ~rpc_channel();
		inline void start();
		inline void call(context_ptr& ctx);
		inline void cancel(context_ptr const& ctx);

	private:
		inline void start_rpc_service();
		inline void stop_rpc_service(error_code error);
		inline void call_impl();
		inline void call_impl1();
		inline void recv_head();
		inline void recv_body();
		inline void call_complete(context_ptr& ctx);
		inline void setup_heartbeat_timer();
		inline void stop_rpc_calls(error_code error);
		inline void set_timeout(context_ptr& ctx);

	private:  // handlers
		inline void handle_send(boost::system::error_code const& error);
		inline void handle_recv_head(boost::system::error_code const& error);
		inline void handle_recv_body(context_ptr ctx, boost::system::error_code const& error);
		inline void handle_recv_body_discard(boost::system::error_code const& error);
		inline void handle_heartbeat(boost::system::error_code const& error);
		inline void handle_timeout(context_ptr ctx, boost::system::error_code const& error);

	private:
		rpc_manager_t&					rpc_mgr_;
		steady_timer_t					hb_timer_;
		async_connection					connection_;
		rpc_call_container_t				calls_;
		std::atomic<status_t>				status_;
		bool								is_write_in_progress_;
		rep_header						head_;
		mutable std::mutex				mutex_;
		call_list_t						to_calls_;
		std::vector<char>					to_discard_message_;
	};
	
	template <typename CodecPolicy>
	class rpc_manager
	{
		template <typename>
		friend class rpc_channel;

	public:
		using codec_policy = CodecPolicy;
		using rpc_channel_t = rpc_channel<codec_policy>;
		using session_map_t = std::map<tcp::endpoint, std::shared_ptr<rpc_channel_t>>;
		using session_ptr = std::shared_ptr<rpc_channel_t>;
		using context_ptr = typename rpc_channel_t::context_ptr;
		using context_t = typename rpc_channel_t::context_t;

	public:
		inline explicit rpc_manager(io_service_t& ios);
		inline void call(context_ptr& ctx);
		inline void cancel(context_ptr const& ctx);
		inline io_service_t& get_io_service();

	private:
		inline session_ptr get_session(tcp::endpoint const& endpoint);
		inline void remove_session(tcp::endpoint const& endpoint);

	private:
		io_service_t&						ios_;
		session_map_t						sessions_;
		mutable std::mutex				mutex_;
	};
} }