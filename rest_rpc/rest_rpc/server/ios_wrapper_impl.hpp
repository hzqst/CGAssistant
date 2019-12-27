#pragma once

namespace timax { namespace rpc 
{
	ios_wrapper::ios_wrapper(io_service_t& ios)
		: ios_(ios)
		, write_in_progress_(false)
	{
	}

	void ios_wrapper::write(connection_ptr& conn_ptr, context_ptr& context)
	{
		assert(nullptr != context);

		lock_t lock{ mutex_ };
		if (!write_in_progress_)
		{
			write_in_progress_ = true;
			lock.unlock();
			write_progress_entry(conn_ptr, context);
		}
		else
		{
			delay_messages_.emplace_back(conn_ptr, context);
		}
	}

	io_service_t& ios_wrapper::get_io_service() noexcept
	{
		return ios_;
	}

	void ios_wrapper::write_progress_entry(connection_ptr& conn_ptr, context_ptr& context)
	{
		assert(nullptr != context);
		async_write(conn_ptr->socket(), context->get_message(), boost::bind(
			&ios_wrapper::handle_write_entry, this, conn_ptr, context, asio_error));
	}

	void ios_wrapper::write_progress()
	{
		lock_t lock{ mutex_ };
		if (delay_messages_.empty())
		{
			write_in_progress_ = false;
			return;
		}
		else
		{
			context_container_t delay_messages = std::move(delay_messages_);
			lock.unlock();
			write_progress(std::move(delay_messages));
		}
	}

	void ios_wrapper::write_progress(context_container_t&& delay_messages)
	{
		auto& conn_ptr = delay_messages.front().first;
		auto& ctx_ptr = delay_messages.front().second;

		async_write(conn_ptr->socket(), ctx_ptr->get_message(), std::bind(
			&ios_wrapper::handle_write, this, std::move(delay_messages), std::placeholders::_1));
	}

	void ios_wrapper::handle_write_entry(connection_ptr conn_ptr, context_ptr context, boost::system::error_code const& error)
	{
		assert(nullptr != context);
		if (!conn_ptr->socket().is_open())
			return;

		if (error)
		{
			conn_ptr->on_error(error);
		}
		else
		{
			// call the post function
			context->apply_post_func();

			// release the memory as soon as possible
			context.reset();

			// continue
			write_progress();
		}
	}

	void ios_wrapper::handle_write(context_container_t& delay_messages, boost::system::error_code const& error)
	{
		connection_ptr conn_ptr;
		context_ptr ctx_ptr;
		std::tie(conn_ptr, ctx_ptr) = std::move(delay_messages.front());
		delay_messages.pop_front();

		if (!conn_ptr->socket().is_open())
			return;

		if (error)
		{
			conn_ptr->on_error(error);
		}
		else
		{
			// call the post function
			ctx_ptr->apply_post_func();

			// release the memory as soon as possible
			ctx_ptr.reset();
			conn_ptr.reset();

			// continue
			if (!delay_messages.empty())
			{
				write_progress(std::move(delay_messages));
			}
			else
			{
				write_progress();
			}
		}
	}
} }