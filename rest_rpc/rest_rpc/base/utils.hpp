#pragma once

namespace timax{ namespace rpc 
{
	inline bool retry(const std::function<bool()>& func, size_t max_attempts, size_t retry_interval = 0)
	{
		for (size_t i = 0; i < max_attempts; i++)
		{
			if (func())
				return true;

			if (retry_interval > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(retry_interval));
		}

		return false;
	}

	inline std::vector<tcp::endpoint> get_tcp_endpoints(std::string const& address_port_string_list)
	{
		std::vector<std::string> address_port_list;
		boost::split(address_port_list, address_port_string_list, boost::is_any_of(" ,|"));
		std::vector<tcp::endpoint> tcp_endpoints;
		for (auto const& address_port : address_port_list)
		{
			auto pos = address_port.rfind(':');
			if (std::string::npos != pos)
			{
				tcp_endpoints.emplace_back(
					boost::asio::ip::address::from_string(address_port.substr(0, pos)),
					boost::lexical_cast<uint16_t>(address_port.substr(pos + 1))
				);
			}
		}

		return tcp_endpoints;
	}

	inline static tcp::endpoint get_tcp_endpoint(std::string const& address, uint16_t port)
	{
		return{ boost::asio::ip::address::from_string(address), port };
	}

	inline auto get_topic_and_data(char const* data, size_t size) -> std::tuple<std::string, char const*, size_t>
	{
		std::string topic = data;
		if (topic.empty())
			throw exception{ error_code::FAIL, "Miss topic!" };

		auto topic_length = topic.size() + 1;
		if(size <= topic_length)
			throw exception{ error_code::FAIL, "Pub protocol error!" };

		data += topic_length;
		size -= topic_length;
		return std::make_tuple(std::move(topic), data, size);
	}
} }
