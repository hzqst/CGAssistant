#pragma once

namespace timax{ namespace rpc 
{
	class async_connection
	{
	public:
		async_connection(
			io_service_t& ios, 
			tcp::endpoint const& endpoint,
			size_t max_retry_count = 0)
			: socket_(ios)
			, endpoint_(endpoint)
			, max_retry_count_(max_retry_count)
			, retry_count_(0)
		{
		}

		void start(
			std::function<void()>&& on_success,
			std::function<void()>&& on_error)
		{
			on_success_ = std::move(on_success);
			on_error_ = std::move(on_error);
			start_connect();
		}

		tcp::socket& socket()
		{
			return socket_;
		}

		tcp::endpoint const& endpoint() const
		{
			return endpoint_;
		}

	private:
		void start_connect()
		{
			socket_.async_connect(endpoint_, boost::bind(&async_connection::handle_connection, this, asio_error));
		}

		void handle_connection(const boost::system::error_code& error)
		{
			if (!error)
			{
				if (on_success_)
					on_success_();
			}
			else if (++retry_count_ < max_retry_count_ || 0 == max_retry_count_)
			{
				start_connect();
			}
			else
			{
				if (on_error_)
					on_error_();
			}
		}

	private:
		tcp::socket				socket_;
		tcp::endpoint				endpoint_;
		size_t const				max_retry_count_;
		size_t					retry_count_;
		std::function<void()>		on_success_;
		std::function<void()>		on_error_;
	};
} }