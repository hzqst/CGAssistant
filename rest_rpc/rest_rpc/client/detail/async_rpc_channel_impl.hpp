#pragma once

namespace timax { namespace rpc 
{
	template <typename CodecPolicy>
	rpc_channel<CodecPolicy>::rpc_channel(rpc_manager_t& mgr, io_service_t& ios, tcp::endpoint const& endpoint)
		: rpc_mgr_(mgr)
		, hb_timer_(ios)
		, connection_(ios, endpoint)
		, status_(status_t::ready)
		, is_write_in_progress_(false)
	{
	}

	template <typename CodecPolicy>
	rpc_channel<CodecPolicy>::~rpc_channel()
	{
		stop_rpc_service(error_code::CANCEL);
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::start()
	{
		auto self = this->shared_from_this();
		connection_.start(
			[this, self]		// when successfully connected
		{ 
			start_rpc_service(); 
		},
			[this, self]		// when failed to connect
		{ 
			stop_rpc_calls(error_code::BADCONNECTION);
		});
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::call(context_ptr& ctx)
	{
		auto status = status_.load();
		if (status_t::stopped == status)
		{
			ctx->error(error_code::BADCONNECTION, "rpc session already stoppet");
			return;
		}

		bool empty = false;
		{
			lock_t lock{ mutex_ };
			empty = calls_.call_list_empty();
			auto push_success = calls_.push_call(ctx);
			lock.unlock();
			if (!push_success)
			{
				ctx->error(error_code::UNKNOWN);
			}
			else
			{
				set_timeout(ctx);
			}
		}

		if (empty && status_t::running == status)
		{
			auto self = this->shared_from_this();
			rpc_mgr_.get_io_service().post([self, this]
			{
				if (to_calls_.empty())
				{
					call_impl();
				}
			});
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::cancel(context_ptr const& ctx)
	{
		auto self = this->shared_from_this();
		rpc_mgr_.get_io_service().post([self, this, ctx]
		{
			calls_.get_call_from_map(ctx->head.id);
			ctx->error(error_code::CANCEL, "Rpc cancled by client");
		});
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::start_rpc_service()
	{
		status_ = status_t::running;
		call_impl();
		recv_head();
		setup_heartbeat_timer();
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::stop_rpc_service(error_code error)
	{
		status_ = status_t::stopped;
		stop_rpc_calls(error);
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::call_impl()
	{
		lock_t lock{ mutex_ };
		if (!calls_.call_list_empty())
		{
			calls_.task_calls_from_list(to_calls_);
			lock.unlock();
			call_impl1();
		}
	}
	
	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::call_impl1()
	{
		auto& to_call = to_calls_.front();

		async_write(connection_.socket(), to_call->get_send_message(),
			boost::bind(&rpc_channel::handle_send, this->shared_from_this(), asio_error));
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::recv_head()
	{
		async_read(connection_.socket(), boost::asio::buffer(&head_, sizeof(head_)),
			boost::bind(&rpc_channel::handle_recv_head, this->shared_from_this(), asio_error));
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::recv_body()
	{
		auto call_id = head_.id;
		lock_t locker{ mutex_ };
		auto call_ctx = calls_.get_call_from_map(call_id);
		locker.unlock();

		if (0 == head_.len)
		{
			call_complete(call_ctx);
		}
		else if (head_.len > MAX_BUF_LEN)
		{
			stop_rpc_service(error_code::UNKNOWN);
			connection_.socket().close();
		}
		else
		{
			if (nullptr == call_ctx)
			{
				to_discard_message_.resize(head_.len);
				async_read(connection_.socket(), boost::asio::buffer(to_discard_message_),
					boost::bind(&rpc_channel::handle_recv_body_discard, this->shared_from_this(), asio_error));
			}
			else
			{
				async_read(connection_.socket(), call_ctx->get_recv_message(head_.len), boost::bind(&rpc_channel::handle_recv_body,
					this->shared_from_this(), call_ctx, asio_error));
			}
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::call_complete(context_ptr& ctx)
	{
		if (nullptr != ctx)
		{
			auto rcode = static_cast<result_code>(head_.code);
			if (result_code::OK == rcode)
			{
				ctx->ok();
			}
			else
			{
				ctx->error();
			}
		}

		recv_head();
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::setup_heartbeat_timer()
	{
		using namespace std::chrono_literals;

		hb_timer_.expires_from_now(15s);
		hb_timer_.async_wait(boost::bind(&rpc_channel::handle_heartbeat, this->shared_from_this(), asio_error));
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::stop_rpc_calls(error_code error)
	{
		call_map_t to_responses;
		{
			lock_t locker{ mutex_ };
			calls_.task_calls_from_map(to_responses);
		}
		for (auto& elem : to_responses)
		{
			auto ctx = elem.second;
			ctx->error(error);
		}

		rpc_mgr_.remove_session(connection_.endpoint());
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::set_timeout(context_ptr& ctx)
	{
		if (duration_t::max() != ctx->timeout)
		{
			ctx->timer.expires_from_now(ctx->timeout);
			ctx->timer.async_wait(boost::bind(&rpc_channel::handle_timeout,
				this->shared_from_this(), ctx, asio_error));
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::handle_send(boost::system::error_code const& error)
	{
		if (!connection_.socket().is_open())
			return;

		to_calls_.pop_front();

		if (error)
		{
			stop_rpc_service(error_code::BADCONNECTION);
		}
		else if(to_calls_.empty())
		{
			call_impl();
		}
		else
		{
			call_impl1();
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::handle_recv_head(boost::system::error_code const& error)
	{
		if (!connection_.socket().is_open())
			return;

		if (!error)
		{
			recv_body();
		}
		else
		{
			// TODO log
			stop_rpc_service(error_code::BADCONNECTION);
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::handle_recv_body(context_ptr ctx, boost::system::error_code const& error)
	{
		if (!connection_.socket().is_open())
			return;

		if (!error)
		{
			call_complete(ctx);
		}
		else
		{
			stop_rpc_service(error_code::BADCONNECTION);
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::handle_recv_body_discard(boost::system::error_code const& error)
	{
		if (!connection_.socket().is_open())
			return;

		if (!error)
		{
			recv_head();
		}
		else
		{
			stop_rpc_service(error_code::BADCONNECTION);
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::handle_heartbeat(boost::system::error_code const& error)
	{
		if (!connection_.socket().is_open())
			return;

		if (!error)
		{
			auto ctx = std::make_shared<context_t>(rpc_mgr_.get_io_service());
			call(ctx);
			setup_heartbeat_timer();
		}
	}

	template <typename CodecPolicy>
	void rpc_channel<CodecPolicy>::handle_timeout(context_ptr ctx, boost::system::error_code const& error)
	{
		if (!error && !ctx->is_over)
		{
			ctx->error(error_code::TIMEOUT, std::move(error.message()));
			lock_t lock{ mutex_ };
			calls_.remove_call_from_map(ctx->head.id);
		}
	}

	template <typename CodecPolicy>
	rpc_manager<CodecPolicy>::rpc_manager(io_service_t& ios)
		: ios_(ios)
	{}

	template <typename CodecPolicy>
	void rpc_manager<CodecPolicy>::call(context_ptr& ctx)
	{
		get_session(ctx->endpoint)->call(ctx);
	}

	template <typename CodecPolicy>
	void rpc_manager<CodecPolicy>::cancel(context_ptr const& ctx)
	{
		get_session(ctx->endpoint)->cancel(ctx);
	}

	template <typename CodecPolicy>
	io_service_t& rpc_manager<CodecPolicy>::get_io_service()
	{
		return ios_;
	}

	template <typename CodecPolicy>
	typename rpc_manager<CodecPolicy>::session_ptr rpc_manager<CodecPolicy>::get_session(tcp::endpoint const& endpoint)
	{
		lock_t locker{ mutex_ };
		auto itr = sessions_.find(endpoint);
		if (itr == sessions_.end())
		{
			auto session = std::make_shared<rpc_channel_t>(*this, ios_, endpoint);
			session->start();
			sessions_.emplace(endpoint, session);
			return session;
		}
		else
		{
			return itr->second;
		}
	}

	template <typename CodecPolicy>
	void rpc_manager<CodecPolicy>::remove_session(tcp::endpoint const& endpoint)
	{
		lock_t locker{ mutex_ };
		auto itr = sessions_.find(endpoint);
		if (itr != sessions_.end())
			sessions_.erase(itr);
	}
} }