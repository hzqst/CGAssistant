#pragma once

namespace timax { namespace rpc 
{
	template <typename CodecPolicy>
	class server
	{
	public:
		using codec_policy = CodecPolicy;
		using connection_weak = std::weak_ptr<connection>;
		using router_t = router<codec_policy>;
		using invoker_t = typename router_t::invoker_t;
		using sub_container = std::multimap<std::string, connection_weak>;

	public:
		server(uint16_t port, size_t pool_size, duration_t time_out = duration_t::max())
			: ios_pool_(pool_size)
			, acceptor_(ios_pool_.get_io_service(), tcp::endpoint{ boost::asio::ip::address::from_string("127.0.0.1"), port})
			, time_out_(time_out)
		{
			init_callback_functions();

			register_handler(SUB_TOPIC, [this](std::string const& topic) { return topic; }, [this](auto conn, auto const& topic)
			{
				if (!topic.empty())
				{
					lock_t lock{ mutex_ };
					subscribers_.emplace(topic, conn);
				}
			});

			router_.register_raw_invoker(PUB, [this](connection_ptr conn, char const* data, size_t size)
			{
				std::string topic;
				size_t length = 0;
				std::tie(topic, data, length) =
					std::move(get_topic_and_data(data, size));
				pub(topic, data, length);
				auto ctx = context_t::make_message_with_head(conn->head_, context_t::message_t{});
				conn->response(ctx);
			});
		}

		~server()
		{
			stop();
		}

		void start()
		{
			ios_pool_.start();
			do_accept();
		}

		void stop()
		{
			ios_pool_.stop();
		}

		template <typename Handler, typename PostFunc>
		bool register_handler(std::string const& name, Handler&& handler, PostFunc&& post_func)
		{
			return router_.register_invoker(name, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
		}

		template <typename Handler, typename PostFunc>
		bool async_register_handler(std::string const& name, Handler&& handler, PostFunc&& post_func)
		{
			return router_.async_register_invoker(name, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
		}

		template <typename Handler>
		bool register_handler(std::string const& name, Handler&& handler)
		{
			return router_.register_invoker(name, std::forward<Handler>(handler));
		}

		template <typename Handler>
		bool async_register_handler(std::string const& name, Handler&& handler)
		{
			return router_.async_register_invoker(name, std::forward<Handler>(handler));
		}

		template <typename Result>
		void pub(std::string const& topic, Result const& result, std::function<void()>&& postf = nullptr)
		{
			auto buffer = pack_as_tuple_if_not(codec_policy{}, result);
			auto ctx = context_t::make_message_without_head(std::move(buffer), std::move(postf));
			public_to_subscriber(topic, ctx);
		}

		void pub(std::string const& topic, char const* data, size_t size, std::function<void()>&& posf = nullptr)
		{
			context_t::message_t buffer{ data, data + size };
			auto ctx = context_t::make_message_without_head(std::move(buffer), std::move(posf));
			public_to_subscriber(topic, ctx);
		}

		void remove_sub_conn(connection_ptr conn)
		{
			lock_t lock{ mutex_ };
			for (auto itr = subscribers_.begin(); itr != subscribers_.end(); )
			{
				if (itr->second.lock() == conn)
					itr = subscribers_.erase(itr);
				else
					++itr;
			}
		}

	private:
		void init_callback_functions()
		{
			router_.set_on_read([this](connection_ptr conn_ptr)
			{
				//auto& header = conn_ptr->get_read_header();
				auto read_buffer = conn_ptr->get_read_buffer();
				router_.apply_invoker(conn_ptr, read_buffer.data(), read_buffer.size());
			});

			router_.set_on_error([this](connection_ptr conn_ptr, boost::system::error_code const& /*error*/)
			{
				// TODO ...
				remove_sub_conn(conn_ptr);
			});
		}

		void do_accept()
		{
			auto new_connection = std::make_shared<connection>(
				ios_pool_.get_io_service(), router_, time_out_);

			acceptor_.async_accept(new_connection->socket(), 
				[this, new_connection](boost::system::error_code const& error)
			{
				if (!error)
				{
					new_connection->start();
				}
				else
				{
					// TODO log error
				}

				do_accept();
			});
		}

		void public_to_subscriber(std::string const& topic, std::shared_ptr<context_t>& ctx)
		{
			lock_t lock{ mutex_ };
			auto range = subscribers_.equal_range(topic);
			if (range.first == range.second)
				return;

			std::list<connection_ptr> alives;
			std::for_each(range.first, range.second, [&alives](auto& elem)
			{
				auto conn = elem.second.lock();
				if (conn)
				{
					alives.push_back(conn);
				}
			});

			lock.unlock();

			for (auto& alive_conn : alives)
			{
				alive_conn->response(ctx);
			}
		}

	private:
		router_t						router_;
		io_service_pool				ios_pool_;
		tcp::acceptor					acceptor_;
		duration_t					time_out_;
		mutable std::mutex			mutex_;
		sub_container					subscribers_;
	};
} }