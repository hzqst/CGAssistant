#pragma once

#include "../../forward.hpp"

namespace timax { namespace rpc 
{
	class sub_channel : public std::enable_shared_from_this<sub_channel>
	{
	public:
		using function_t = std::function<void(char const*, size_t)>;
		using error_function_t = std::function<void(exception const&)>;

	public:
		template <typename Message>
		sub_channel(
			io_service_t& ios, 
			tcp::endpoint const& endpoint,
			std::string const& topic_string,
			Message const& topic, 
			function_t&& func,
			error_function_t&& error = nullptr)
			: hb_timer_(ios)
			, connection_(ios, endpoint)
			, topic_string_(topic_string)
			, topic_(topic.begin(), topic.end())
			, function_(std::move(func))
			, error_(std::move(error))
		{
		}

		void start()
		{
			running_flag_.store(true);
			auto self = this->shared_from_this();
			connection_.start(
				[self, this]()
			{
				request_sub();
			},
				[self, this]()
			{
				stop();
			});
		}

		void stop()
		{
			running_flag_.store(false);
		}

		tcp::endpoint const& get_endpoint() const noexcept
		{
			return connection_.endpoint();
		}

		std::string const& get_topic() const noexcept
		{
			return topic_string_;
		}

		static auto get_on_error() 
			-> std::function<void(std::shared_ptr<sub_channel>&)>&
		{
			static std::function<void(std::shared_ptr<sub_channel>&)> on_error;
			return on_error;
		}

		static auto get_deserialize_exception()
			-> std::function<exception(char const*, size_t)>&
		{
			static std::function<exception(char const*, size_t)> on_deserialize_exception;
			return on_deserialize_exception;
		}

	private:
		auto request_sub_message()
			-> std::vector<boost::asio::const_buffer>
		{
			send_head_.len = static_cast<uint32_t>(topic_.size());
			send_head_.hash = sub_topic.name();
			return 
			{
				boost::asio::buffer(&send_head_, sizeof(send_head_)),
				boost::asio::buffer(topic_)
			};
		}

		void request_sub()
		{
			if (running_flag_.load())
			{
				auto requet_message = request_sub_message();
				async_write(connection_.socket(), requet_message, boost::bind(&sub_channel::handle_request_sub, 
					this->shared_from_this(), asio_error));
			}
		}

		void begin_sub_procedure()
		{
			setup_heartbeat_timer();			// setup heart beat
			recv_sub_head();
		}

		void setup_heartbeat_timer()
		{
			using namespace std::chrono_literals;
			hb_timer_.expires_from_now(15s);
			hb_timer_.async_wait(boost::bind(&sub_channel::handle_heartbeat, this->shared_from_this(), asio_error));
		}

		void recv_sub_head()
		{
			async_read(connection_.socket(), boost::asio::buffer(&recv_head_, sizeof(recv_head_)), boost::bind(
				&sub_channel::handle_sub_head, this->shared_from_this(), asio_error));
		}

		void on_error(exception const& exception) 
		{
			if (error_)
			{
				error_(exception);
			}

			auto& on_error_func = get_on_error();
			if (on_error_func)
			{
				auto self = this->shared_from_this();
				on_error_func(self);
			}
		}

		void on_error()
		{
			auto& deserialize_exception = get_deserialize_exception();
			if (deserialize_exception)
			{
				auto e = deserialize_exception(response_.data(), response_.size());
				on_error(e);
			}
		}

	private:
		void handle_request_sub(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			std::memset(&send_head_, 0, sizeof(req_header));
			if (!error)
			{
				async_read(connection_.socket(), boost::asio::buffer(&recv_head_, sizeof(recv_head_)), boost::bind(
					&sub_channel::handle_response_sub_head, this->shared_from_this(), asio_error));
			}
			else
			{
				on_error(exception{ error_code::BADCONNECTION, std::move(error.message()) });
			}
		}

		void handle_response_sub_head(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			if (!error)
			{
				if (recv_head_.len > 0)
				{
					response_.resize(recv_head_.len);
					async_read(connection_.socket(), boost::asio::buffer(response_), boost::bind(
						&sub_channel::handle_response_sub_body, this->shared_from_this(), asio_error));
				}
			}
			else
			{
				on_error(exception{ error_code::BADCONNECTION, std::move(error.message()) });
			}
		}

		void handle_response_sub_body(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			if (!error)
			{
				if (result_code::OK == static_cast<result_code>(recv_head_.code))
				{
					begin_sub_procedure();
				}
				else
				{
					on_error();
				}
			}
			else
			{
				on_error(exception{ error_code::BADCONNECTION, std::move(error.message()) });
			}
		}

		void handle_sub_head(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			if (!error)
			{
				if (recv_head_.len > 0)
				{
					// in this case, we got sub message
					response_.resize(recv_head_.len);
					async_read(connection_.socket(), boost::asio::buffer(response_), boost::bind(
						&sub_channel::handle_sub_body, this->shared_from_this(), asio_error));
				}
				else
				{
					// in this case we got heart beat back
					recv_sub_head();
				}
			}
			else
			{
				on_error(exception{ error_code::BADCONNECTION, std::move(error.message()) });
			}
		}

		void handle_sub_body(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			if (!error)
			{
				if (function_)
				{
					try
					{
						function_(response_.data(), response_.size());
					}
					catch (exception const& error)
					{
						on_error(error);
						return;
					}
				}

				recv_sub_head();
			}
			else
			{
				on_error(exception{ error_code::BADCONNECTION, std::move(error.message()) });
			}
		}

		void handle_heartbeat(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			if (!error)
			{
				async_write(connection_.socket(), boost::asio::buffer(&send_head_, sizeof(send_head_)),
					boost::bind(&sub_channel::handle_send_hb, this->shared_from_this(), asio_error));

				setup_heartbeat_timer();
			}
		}

		void handle_send_hb(boost::system::error_code const& error)
		{
			if (!(connection_.socket().is_open() && running_flag_.load()))
				return;

			if (error)
			{
				on_error(exception{ error_code::BADCONNECTION, std::move(error.message()) });
			}
		}

	private:
		steady_timer_t						hb_timer_;
		async_connection						connection_;
		req_header							send_head_;
		rep_header							recv_head_;
		std::string const						topic_string_;
		std::vector<char> const				topic_;
		std::vector<char>						response_;
		function_t							function_;
		error_function_t						error_;
		std::atomic<bool>						running_flag_;
	};


	template <typename CodecPolicy>
	class sub_manager
	{
	public:
		using codec_policy = CodecPolicy;
		using sub_channel_t = sub_channel;
		using sub_channel_ptr = std::shared_ptr<sub_channel_t>;
		using topics_map_t = std::map<std::string, sub_channel_ptr>;
		using endpoint_map_t = std::map<tcp::endpoint, topics_map_t>;
		using function_t = sub_channel_t::function_t;

	public:
		sub_manager(io_service_t& ios)
			: ios_(ios)
		{
			auto& on_error = sub_channel_t::get_on_error();
			on_error = [this](sub_channel_ptr& channel) { remove(channel); };

			auto& on_deserialize_exception = sub_channel_t::get_deserialize_exception();
			on_deserialize_exception = [](char const* data, size_t size) 
			{ 
				codec_policy cp{};
				return cp.template unpack<exception>(data, size);
			};
		}

		template <typename Protocol, typename Func>
		void sub(tcp::endpoint const& endpoint, Protocol const& protocol, Func&& func)
		{
			auto channel = make_sub_channel(endpoint, protocol, std::forward<Func>(func));
			sub_impl(endpoint, protocol.topic(), channel);
		}

		template <typename Protocol, typename Func, typename EFunc>
		void sub(tcp::endpoint const& endpoint, Protocol const& protocol, Func&& func, EFunc&& error)
		{
			auto channel = make_sub_channel(endpoint, protocol, std::forward<Func>(func), std::forward<EFunc>(error));
			sub_impl(endpoint, protocol.topic(), channel);
		}

		void remove(sub_channel_ptr& channel)
		{
			lock_t lock{ mutex_ };
			auto endpoint_itr = topics_.find(channel->get_endpoint());
			if (topics_.end() != endpoint_itr)
			{
				auto& topics_in_endpoint = endpoint_itr->second;
				auto topic_itr = topics_in_endpoint.find(channel->get_topic());
				if (topics_in_endpoint.end() != topic_itr)
				{
					topics_in_endpoint.erase(topic_itr);
				}

				if (topics_in_endpoint.empty())
					topics_.erase(endpoint_itr);
			}
		}

		void sub_impl(tcp::endpoint const& endpoint, std::string const& topic, sub_channel_ptr& channel)
		{
			lock_t lock{ mutex_ };
			auto endpoint_itr = topics_.find(endpoint);
			if (topics_.end() == endpoint_itr)
			{
				topics_map_t topic_map;
				topic_map.emplace(topic, channel);
				topics_.emplace(endpoint, std::move(topic_map));
				channel->start();
			}
			else
			{
				auto topic_itr = endpoint_itr->second.find(topic);
				if (endpoint_itr->second.end() != topic_itr)
				{
					throw exception{ error_code::UNKNOWN, "Sub topic already existed!" };
				}
				else
				{
					endpoint_itr->second.emplace(topic, channel);
					channel->start();
				}
			}
		}

		template <typename Protocol, typename Func>
		sub_channel_ptr make_sub_channel(tcp::endpoint const& endpoint, Protocol const& protocol, Func&& func)
		{
			codec_policy cp{};
			auto topic = protocol.pack_topic(cp);
			auto proc_func = make_proc_func(protocol, std::forward<Func>(func));
			return std::make_shared<sub_channel_t>(ios_, endpoint, protocol.topic(), topic, std::move(proc_func));
		}

		template <typename Protocol, typename Func, typename EFunc>
		sub_channel_ptr make_sub_channel(tcp::endpoint const& endpoint, Protocol const& protocol, Func&& func, EFunc&& efunc)
		{
			codec_policy cp{};
			auto topic = protocol.pack_topic(cp);
			auto proc_func = make_proc_func(protocol, std::forward<Func>(func));
			return std::make_shared<sub_channel_t>(ios_, endpoint, protocol.topic(), topic, std::move(proc_func), std::forward<EFunc>(efunc));
		}

		template <typename Handler, typename ForwardTuple, size_t ... Is>
		static void apply_handler_impl(Handler&& handler, ForwardTuple&& tuple, std::index_sequence<Is...>)
		{
			handler(std::forward<std::tuple_element_t<Is, std::remove_reference_t<ForwardTuple>>>(std::get<Is>(tuple))...);
		}

		template <typename Handler, typename ForwardTuple>
		static void apply_handler(Handler&& handler, ForwardTuple&& tuple)
		{
			using indices_type = std::make_index_sequence<std::tuple_size<std::remove_reference_t<ForwardTuple>>::value>;
			apply_handler_impl(std::forward<Handler>(handler), std::forward<ForwardTuple>(tuple), indices_type{});
		}

		template <typename Protocol, typename Func>
		static function_t make_proc_func(Protocol const& protocol, Func&& func)
		{
			return [f = std::forward<Func>(func), &protocol](char const* data, size_t size)
			{
				codec_policy cp{};
				auto result = protocol.unpack(cp, data, size);
				apply_handler(f, result);
			};
		}

	private:
		io_service_t&				ios_;
		endpoint_map_t			topics_;
		std::mutex				mutex_;
	};
} }