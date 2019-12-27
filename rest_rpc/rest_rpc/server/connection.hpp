#pragma once

namespace timax { namespace rpc 
{
	class ios_wrapper;
	using blob_t = iguana::blob_t;

	class connection : public std::enable_shared_from_this<connection>
	{
	public:
		template <typename> friend class server;
		template <typename> friend class router;
		template <typename, typename> friend struct invoker_traits;
		friend class ios_wrapper;

		using connection_ptr = std::shared_ptr<connection>;
		using context_ptr = std::shared_ptr<context_t>;

	public:
		connection(io_service_t& ios, router_base& router, duration_t time_out);
		void close();
		io_service_t& get_io_service();

	protected:
		tcp::socket& socket();
		void start();
		void response(context_ptr& ctx);
		void on_error(boost::system::error_code const& error);
		blob_t get_read_buffer() const;
		req_header const& get_read_header() const;

	private:
		void set_no_delay();
		void expires_timer();
		void cancel_timer();
		void read_head();
		void read_body();

	private:
		void handle_read_head(boost::system::error_code const& error);
		void handle_read_body(boost::system::error_code const& error);
		void handle_time_out(boost::system::error_code const& error);

	private:
		ios_wrapper						ios_wrapper_;
		router_base&						router_;
		tcp::socket						socket_;
		req_header						head_;
		std::vector<char>					read_buffer_;
		steady_timer_t					timer_;
		duration_t						time_out_;
	};
} }