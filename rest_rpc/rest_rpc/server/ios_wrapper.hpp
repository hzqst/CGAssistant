#pragma once

namespace timax { namespace rpc 
{
	class ios_wrapper
	{
	public:
		using post_func_t = std::function<void()>;
		using context_ptr = std::shared_ptr<context_t>;
		using context_container_t = std::list<std::pair<connection_ptr, context_ptr>>;

	public:
		explicit ios_wrapper(io_service_t& ios);
		void write(connection_ptr& conn_ptr, context_ptr& context);
		io_service_t& get_io_service() noexcept;

	private:
		void write_progress_entry(connection_ptr& conn_ptr, context_ptr& context);
		void write_progress();
		void write_progress(context_container_t&& delay_messages);

	private:
		void handle_write_entry(connection_ptr conn_ptr, context_ptr context, boost::system::error_code const& error);
		void handle_write(context_container_t& delay_messages, boost::system::error_code const& error);

	private:
		io_service_t&					ios_;
		context_container_t			delay_messages_;
		bool							write_in_progress_;
		mutable std::mutex			mutex_;
	};
} }