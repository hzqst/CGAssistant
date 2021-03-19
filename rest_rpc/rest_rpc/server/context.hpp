#pragma once

namespace timax { namespace rpc 
{
	struct context_t
	{
		using post_func_t = std::function<void()>;
		using message_t = std::vector<char>;

		context_t() = default;

		template <typename Message>
		context_t(rep_header const& h, Message&& msg, post_func_t postf)
			: context_t(h, std::move(message_t{ msg.begin(), msg.end() }), std::move(postf))
		{
		}

		template <typename Message>
		context_t(Message&& msg, post_func_t postf)
			: context_t(std::move(message_t{ msg.begin(), msg.end() }), std::move(postf))
		{
		}

		context_t(rep_header const& h, message_t msg, post_func_t postf)
			: head(h)
			, message(std::move(msg))
			, post_func(std::move(postf))
		{
			head.len = static_cast<uint32_t>(message.size());
		}

		context_t(message_t msg, post_func_t postf)
			: message(std::move(msg))
			, post_func(std::move(postf))
		{
			head.len = static_cast<uint32_t>(message.size());
		}

		void apply_post_func() const
		{
			if (post_func)
				post_func();
		}

		auto get_message() const
			-> std::vector<boost::asio::const_buffer>
		{
			if (message.empty())
				return{ boost::asio::buffer(&head, sizeof(head)) };

			return{ boost::asio::buffer(&head, sizeof(head)), boost::asio::buffer(message) };
		}

		template <typename Message>
		static auto make_error_message(req_header const& h, Message&& msg, post_func_t postf = nullptr)
		{
			auto ctx = make_message_with_head(h, std::forward<Message>(msg), std::move(postf));
			ctx->head.code = static_cast<int16_t>(result_code::FAIL);
			return ctx;
		}

		template <typename Message>
		static auto make_message_with_head(req_header const& h, Message&& msg, post_func_t postf = nullptr)
		{
			return std::make_shared<context_t>(h, std::forward<Message>(msg), std::move(postf));
		}

		template <typename Message>
		static auto make_message_without_head(Message&& msg, post_func_t postf = nullptr)
		{
			return std::make_shared<context_t>(std::forward<Message>(msg), std::move(postf));
		}

		rep_header		head{ 0, 0, 0 };
		message_t		message;
		post_func_t		post_func;
	};
} }