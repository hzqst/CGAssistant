#pragma once

namespace timax { namespace rpc 
{
	connection::connection(io_service_t& ios, router_base& router, duration_t time_out)
		: ios_wrapper_(ios)
		, router_(router)
		, socket_(ios)
		, read_buffer_(PAGE_SIZE)
		, timer_(ios)
		, time_out_(time_out)
	{
	}

	void connection::response(context_ptr& ctx)
	{
		auto self = this->shared_from_this();
		ios_wrapper_.write(self, ctx);
	}

	void connection::close()
	{
		boost::system::error_code ignored_ec;
		socket_.close(ignored_ec);
	}

	io_service_t& connection::get_io_service()
	{
		return ios_wrapper_.get_io_service();
	}

	tcp::socket& connection::socket()
	{
		return socket_;
	}

	void connection::start()
	{
		set_no_delay();
		read_head();
	}

	void connection::on_error(boost::system::error_code const& error)
	{
		close();
		router_.on_error(this->shared_from_this(), error);
	}

	blob_t connection::get_read_buffer() const
	{
		return{ read_buffer_.data(), head_.len };
	}

	req_header const& connection::get_read_header() const
	{
		return head_;
	}

	void connection::set_no_delay()
	{
		boost::asio::ip::tcp::no_delay option(true);
		boost::system::error_code ec;
		socket_.set_option(option, ec);
	}

	void connection::expires_timer()
	{
		if (time_out_.count() == 0)
			return;

		timer_.expires_from_now(time_out_);
		timer_.async_wait(boost::bind(&connection::handle_time_out, this->shared_from_this(), asio_error));
	}

	void connection::cancel_timer()
	{
		if (time_out_.count() == 0)
			return;

		timer_.cancel();
	}

	void connection::read_head()
	{
		async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
			boost::bind(&connection::handle_read_head, this->shared_from_this(), asio_error));
	}

	void connection::read_body()
	{
		if (head_.len > MAX_BUF_LEN)
		{
			socket_.close();
			return;
		}

		if (head_.len > PAGE_SIZE)
		{
			read_buffer_.resize(head_.len);
		}

		expires_timer();
		async_read(socket_, boost::asio::buffer(read_buffer_.data(), head_.len),
			boost::bind(&connection::handle_read_body, this->shared_from_this(), asio_error));
	}

	void connection::handle_read_head(boost::system::error_code const& error)
	{
		if (!socket_.is_open())
			return;

		if (!error)
		{
			if (head_.len == 0)
			{
				read_head();
			}
			else
			{
				read_body();
			}
		}
		else
		{
			on_error(error);
		}
	}

	void connection::handle_read_body(boost::system::error_code const& error)
	{
		cancel_timer();
		if (!socket_.is_open())
			return;

		if (!error)
		{
			router_.on_read(this->shared_from_this());
			read_head();
		}
		else
		{
			on_error(error);
		}
	}

	void connection::handle_time_out(boost::system::error_code const& error)
	{
		if (!socket_.is_open())
			return;

		if (!error)
		{
			boost::system::error_code e = boost::asio::error::timed_out;
			on_error(e);
		}
	}
} }