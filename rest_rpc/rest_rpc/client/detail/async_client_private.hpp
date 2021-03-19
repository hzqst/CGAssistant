#pragma once

namespace timax { namespace rpc 
{
	template <typename CodecPolicy>
	inline auto make_rpc_context(io_service_t& ios, tcp::endpoint const& endpoint,
		uint64_t name, typename CodecPolicy::buffer_type&& buffer)
	{
		using context_t = rpc_context<CodecPolicy>;
		return std::make_shared<context_t>(ios, endpoint, name, std::move(buffer));
	}

	template <typename CodecPolicy>
	inline auto make_rpc_context(io_service_t& ios, tcp::endpoint const& endpoint,
		uint64_t name, std::string const& topic, typename CodecPolicy::buffer_type&& buffer)
	{
		using context_t = rpc_context<CodecPolicy>;
		return std::make_shared<context_t>(ios, endpoint, name, topic, std::move(buffer));
	}

	template <typename CodecPolicy, typename Protocol, typename ... Args>
	inline auto make_rpc_context(io_service_t& ios, tcp::endpoint const& endpoint, 
		CodecPolicy const& cp, Protocol const& protocol, Args&& ... args)
	{
		auto buffer = protocol.pack_args(cp, std::forward<Args>(args)...);
		return make_rpc_context<CodecPolicy>(ios, endpoint, protocol.name(), std::move(buffer));
	}

	template <typename CodecPolicy, typename Protocol, typename ... Args>
	inline auto make_rpc_context(io_service_t& ios, tcp::endpoint const& endpoint, CodecPolicy const& cp, 
		uint64_t hash, Protocol const& protocol, Args&& ... args)
	{
		auto buffer = protocol.pack_args(cp, std::forward<Args>(args)...);
		return make_rpc_context<CodecPolicy>(ios, endpoint, hash, protocol.topic(), std::move(buffer));
	}

	template <typename CodecPolicy>
	class async_client_private
	{
	public:
		using codec_policy = CodecPolicy;
		using context_t = rpc_context<codec_policy>;
		using context_ptr = std::shared_ptr<context_t>;
		using rpc_manager_t = rpc_manager<codec_policy>;
		using sub_manager_t = sub_manager<codec_policy>;
		using hash_engine = hash<bkdr_hash<std::string>, uint64_t>;

	public:
		async_client_private(io_service_t& ios)
			: ios_(ios)
			, rpc_manager_(ios)
			, sub_manager_(ios)
			, pub_hash_(hash_(PUB))
		{
		}

		io_service_t& get_io_service()
		{
			return ios_;
		}

		rpc_manager_t& get_rpc_manager()
		{
			return rpc_manager_;
		}

		sub_manager_t& get_sub_manager()
		{
			return sub_manager_;
		}

		codec_policy get_codec_policy() const
		{
			return {};
		}

		void call(context_ptr& context)
		{
			rpc_manager_.call(context);
		}

		void cancel(context_ptr const& context)
		{
			rpc_manager_.cancel(context);
		}

		template <typename Protocol, typename ... Args>
		auto make_rpc_context(tcp::endpoint const& endpoint, Protocol const& protocol, Args&& ... args)
		{
			codec_policy cp{};
			return rpc::make_rpc_context(ios_, endpoint, cp, protocol, std::forward<Args>(args)...);
		}

		template <typename Protocol, typename ... Args>
		auto make_pub_context(tcp::endpoint const& endpoint, Protocol const& protocol, Args&& ... args)
		{
			codec_policy cp{};
			return rpc::make_rpc_context(ios_, endpoint, cp, pub_hash_, protocol, std::forward<Args>(args)...);
		}

		template <typename Protocol, typename Func>
		void sub(tcp::endpoint const& endpoint, Protocol const& protocol, Func&& func)
		{
			sub_manager_.sub(endpoint, protocol, std::forward<Func>(func));
		}

		template <typename Protocol, typename Func, typename EFunc>
		void sub(tcp::endpoint const& endpoint, Protocol const& protocol, Func&& func, EFunc&& efunc)
		{
			sub_manager_.sub(endpoint, protocol, std::forward<Func>(func), std::forward<EFunc>(efunc));
		}

		uint64_t hash(std::string const& topic) const
		{
			return hash_(topic);
		}

	private:
		io_service_t&				ios_;
		rpc_manager_t				rpc_manager_;
		sub_manager_t				sub_manager_;
		hash_engine				hash_;
		uint64_t					pub_hash_;
	};

	template <typename CodecPolicy>
	class rpc_task
	{
	public:
		using codec_policy = CodecPolicy;
		using client_private_t = async_client_private<codec_policy>;
		using context_ptr = typename client_private_t::context_ptr;
		
	protected:
		rpc_task(client_private_t& client, context_ptr& ctx)
			: client_(client)
			, ctx_(ctx)
			, dismiss_(false)
		{}

		~rpc_task()
		{
			do_call_managed();
		}

		rpc_task(rpc_task&& other)
			: client_(other.client_)
			, ctx_(std::move(other.ctx_))
			, dismiss_(other.dismiss_)
		{
			other.dismiss_ = true;
		}

		rpc_task(rpc_task const& other) = default;

		void do_call_managed()
		{
			if (!dismiss_)
			{
				client_.call(ctx_);
			}
		}

		void do_call_and_wait()
		{
			if (!dismiss_)
			{
				dismiss_ = true;
				ctx_->create_barrier();
				client_.call(ctx_);
				ctx_->wait();
			}
		}

	public:
		void cancel()
		{
			this->client_.cancel(ctx_);
		}

	protected:
		client_private_t&		client_;
		context_ptr			ctx_;
		bool					dismiss_;
	};

	template <typename CodecPolicy, typename Ret>
	class typed_rpc_task : public rpc_task<CodecPolicy>
	{
	public:
		using codec_policy = CodecPolicy;
		using base_type = rpc_task<CodecPolicy>;
		using result_type = Ret;
		using client_private_t = typename base_type::client_private_t;
		using context_ptr = typename base_type::context_ptr;

	public:
		typed_rpc_task(client_private_t& client, context_ptr& ctx)
			: base_type(client, ctx)
		{
		}

		typed_rpc_task(typed_rpc_task const&) = default;

		template <typename F>
		typed_rpc_task&& on_ok(F&& f) &&
		{
			if (nullptr == result_)
			{
				result_ = std::make_shared<result_type>();
			}

			this->ctx_->on_ok = [func = std::forward<F>(f), r = result_](char const* data, size_t size)
			{
				codec_policy codec{};
				*r = codec.template unpack<result_type>(data, size);
				func(*r);
			};
	
			return std::move(*this);
		}

		template <typename F>
		typed_rpc_task&& on_error(F&& f) &&
		{
			this->ctx_->on_error = std::forward<F>(f);
			return std::move(*this);
		}

		typed_rpc_task&& timeout(duration_t const& t) &&
		{
			this->ctx_->timeout = t;
			return std::move(*this);
		}

		void wait(duration_t const& duration = duration_t::max()) &
		{
			if (!this->dismiss_)
			{
				if (nullptr == result_)
				{
					result_ = std::make_shared<result_type>();
				}

				this->ctx_->on_ok = [r = result_](char const* data, size_t size)
				{
					codec_policy codec{};
					*r = codec.template unpack<result_type>(data, size);
				};
				this->ctx_->on_error = nullptr;
				this->ctx_->timeout = duration;
			}
			this->do_call_and_wait();
		}

		result_type const& get(duration_t const& duration = duration_t::max()) &
		{
			wait(duration);
			return *result_;
		}

	private:
		std::shared_ptr<result_type>	result_;
	};

	template <typename CodecPolicy>
	class typed_rpc_task<CodecPolicy, void> : public rpc_task<CodecPolicy>
	{
	public:
		using codec_policy = CodecPolicy;
		using base_type = rpc_task<CodecPolicy>;
		using result_type = void;
		using client_private_t = typename base_type::client_private_t;
		using context_ptr = typename base_type::context_ptr;

	public:
		typed_rpc_task(client_private_t& client, context_ptr& ctx)
			: base_type(client, ctx)
		{
		}

		typed_rpc_task(typed_rpc_task const&) = default;

		template <typename F>
		typed_rpc_task&& on_ok(F&& f) &&
		{
			this->ctx_->on_ok = [func = std::forward<F>(f)](char const* data, size_t size) { func(); };
			return std::move(*this);
		}

		template <typename F>
		typed_rpc_task&& on_error(F&& f) &&
		{
			this->ctx_->on_error = std::forward<F>(f);
			return std::move(*this);
		}

		typed_rpc_task&& timeout(duration_t const& t) &&
		{
			this->ctx_->timeout = t;
			return std::move(*this);
		}

		void wait(duration_t const& duration = duration_t::max()) &
		{
			if (!this->dismiss_)
			{
				this->ctx_->timeout = duration;
				this->ctx_->on_ok = nullptr;
				this->ctx_->on_error = nullptr;
			}
			this->do_call_and_wait();
		}
	};
} }