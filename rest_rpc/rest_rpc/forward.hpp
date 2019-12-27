#pragma once
// boost libraries
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>

// standard libraries
#include <cstdint>
#include <atomic>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <tuple>
#include <list>
#include <vector>
#include <array>
#include <utility>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <type_traits>
#include <future>

namespace timax { namespace rpc
{
	using tcp = boost::asio::ip::tcp;
	using io_service_t = boost::asio::io_service;
	using lock_t = std::unique_lock<std::mutex>;
	using deadline_timer_t = boost::asio::deadline_timer;
	using steady_timer_t = boost::asio::steady_timer;
	using duration_t = steady_timer_t::duration;

	class router_base;
	class connection;
	using connection_ptr = std::shared_ptr<connection>;

	template <typename Decode>
	class server;

	static const auto asio_error = boost::asio::placeholders::error;
} }

// common headers
#include <rest_rpc/codec/codec.hpp>
#include <rest_rpc/base/log.hpp>
#include <rest_rpc/base/function_traits.hpp>
#include <rest_rpc/base/consts.h>
#include <rest_rpc/base/common.h>
#include <rest_rpc/base/excetion.hpp>
#include <rest_rpc/base/utils.hpp>
#include <rest_rpc/base/io_service_pool.hpp>
#include <rest_rpc/base/hash.hpp>
