#pragma once

namespace timax { namespace rpc 
{
	template <typename CodecPolicy>
	struct rpc_context
	{
		using codec_policy = CodecPolicy;
		using buffer_t = typename codec_policy::buffer_type;
		using success_function_t = std::function<void(char const*, size_t)>;
		using on_error_function_t = std::function<void(exception const&)>;
		using asio_buffers = std::vector<boost::asio::const_buffer>;

		rpc_context(
			io_service_t& ios,
			tcp::endpoint const& endpoint,
			uint64_t hash,
			buffer_t&& request)
			: timer(ios)
			, timeout(duration_t::max())
			, endpoint(endpoint)
			, req(std::move(request))
			, head(0, 0, static_cast<uint32_t>(req.size()), hash)
			, buffer({ 
				boost::asio::buffer(&head, sizeof(head)), 
				boost::asio::buffer(req) })
			, is_over(false)
		{
		}

		rpc_context(
			io_service_t& ios,
			tcp::endpoint const& endpoint,
			uint64_t hash,
			std::string const& t,
			buffer_t&& request)
			: timer(ios)
			, timeout(duration_t::max())
			, endpoint(endpoint)
			, req(std::move(request))
			, topic(t)
			, head(0, 0, static_cast<uint32_t>(req.size() + topic.length() + 1), hash)
			, buffer({ 
				boost::asio::buffer(&head, sizeof(head)), 
				boost::asio::buffer(topic.c_str(), topic.length() + 1),
				boost::asio::buffer(req) })
			, is_over(false)
		{
		}

		explicit rpc_context(io_service_t& ios)
			: timer(ios)
			, timeout(duration_t::max())
			, buffer({ boost::asio::buffer(&head, sizeof(head)) })
			, is_over(false)
		{
		}

		req_header& get_head()
		{
			return head;
		}

		decltype(auto) get_send_message() const
		{
			return buffer;
		}

		auto get_recv_message(size_t size)
		{
			rep.resize(size);
			return boost::asio::buffer(rep);
		}

		void ok()
		{
			if (!is_over)
			{
				is_over = true;

				if (on_ok)
					on_ok(rep.data(), rep.size());

				if (nullptr != barrier)
					barrier->notify();
			}
			
		}

		void error()
		{
			codec_policy cp{};
			auto recv_error = cp.template unpack<exception>(rep.data(), rep.size());
			err = std::move(recv_error);

			post_error();
		}

		void error(error_code errcode, std::string const& message = "")
		{
			err.set_code(errcode);
			if (!message.empty())
			{
				err.set_message(message);
			}

			post_error();
		}

		void post_error()
		{
			if (!is_over)
			{
				is_over = true;

				if (on_error)
					on_error(err);

				if (nullptr != barrier)
					barrier->notify();
			}
			
		}

		void create_barrier()
		{
			if (nullptr == barrier)
				barrier.reset(new result_barrier{});
		}

		void wait()
		{
			if(nullptr != barrier)
				barrier->wait();

			if (err)
				throw err;
		}

		steady_timer_t						timer;
		steady_timer_t::duration				timeout;
		tcp::endpoint							endpoint;
		buffer_t								req;					// request buffer
		std::vector<char>						rep;					// response buffer
		std::string							topic;
		req_header							head;
		asio_buffers							buffer;
		exception							err;
		success_function_t					on_ok;
		on_error_function_t					on_error;
		bool									is_over;
		std::unique_ptr<result_barrier>		barrier;
	};

	template <typename CodecPolicy>
	class rpc_call_container
	{
	public:
		using codec_policy = CodecPolicy;
		using context_t = rpc_context<codec_policy>;
		using context_ptr = std::shared_ptr<context_t>;
		using call_map_t = std::map<uint32_t, context_ptr>;
		using call_list_t = std::list<context_ptr>;

	public:
		explicit rpc_call_container(size_t max_size = MAX_QUEUE_SIZE)
			: call_id_(0)
			, max_size_(max_size)
		{
		}

		bool push_call(context_ptr& ctx)
		{
			if (call_map_.size() >= max_size_)
				return false;

			push_call_response(ctx);
			call_list_.push_back(ctx);
			return true;
		}

		void push_call_response(context_ptr& ctx)
		{
			if (ctx->req.size() > 0)
			{
				auto call_id = ++call_id_;
				ctx->get_head().id = call_id;
				call_map_.emplace(call_id, ctx);
			}
		}

		void task_calls_from_list(call_list_t& to_calls)
		{
			to_calls = std::move(call_list_);
		}

		bool call_list_empty() const
		{
			return call_list_.empty();
		}

		context_ptr get_call_from_map(uint32_t call_id)
		{
			auto itr = call_map_.find(call_id);
			if (call_map_.end() != itr)
			{
				context_ptr ctx = itr->second;
				call_map_.erase(itr);
				return ctx;
			}
			return nullptr;
		}

		void remove_call_from_map(uint32_t call_id)
		{
			auto itr = call_map_.find(call_id);
			if(call_map_.end() != itr)
				call_map_.erase(itr);
		}

		void task_calls_from_map(call_map_t& call_map)
		{
			call_map = std::move(call_map_);
		}

		size_t get_call_list_size() const
		{
			return call_list_.size();
		}

		size_t get_call_map_size() const
		{
			return call_map_.size();
		}

	private:
		call_map_t				call_map_;
		call_list_t				call_list_;
		uint32_t					call_id_;
		size_t					max_size_;
	};
} }