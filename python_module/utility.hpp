#pragma once

#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <time.h>

// TODO check for c++20 fmt implementation
#define FMT_HEADER_ONLY
#include <fmt\format.h>
#include <fmt\printf.h>
#include <fmt\core.h>
#include <fmt\os.h>
#include <fmt\chrono.h>

namespace string
{
	// uses libformat, when compilers add C++20 fmt support, swap for that
	template<typename... ARGs>
	std::string format(const char* fmt, ARGs&&... args)
	{
		return fmt::format(fmt, std::forward<ARGs>(args)...);
	};
};

namespace datetime
{
	static auto local_time()
	{
		using namespace std::chrono;
		std::time_t time = system_clock::to_time_t(system_clock::now());
#pragma warning(push)
#pragma warning(disable: 4996)
		auto lt = std::localtime(&time);
#pragma warning(pop)
		return lt;
	};

	// TODO only used by a one thread, so a static stringstream can avoid realloc
	static std::string timestamp()
	{
		std::stringstream ss; ss << std::put_time(local_time(), "%H:%M:%S");
		return ss.str();
	};

	static std::string timestamp_fs()
	{
		std::stringstream ss; ss << std::put_time(local_time(), "%H.%M.%S");
		return ss.str();
	};

	static std::string datestamp()
	{
		std::stringstream ss; ss << std::put_time(local_time(), "%Y/%m/%d");
		return ss.str();
	};

	static std::string datestamp_fs()
	{
		std::stringstream ss; ss << std::put_time(local_time(), "%Y.%m.%d");
		return ss.str();
	};
};

namespace file {
	using entry = std::filesystem::directory_entry;
	using path = std::filesystem::path;
	using error = std::filesystem::filesystem_error;

	constexpr const std::size_t max_size = (std::numeric_limits<std::size_t>::max)();

	namespace read
	{
		// note : check for existence before calling, it is assumed
		static std::string as_string(const file::entry& file) {
			std::fstream stream; stream.open(file.path(), std::ios::in);

			if(!stream.is_open() || stream.bad())
			{
				throw(std::filesystem::filesystem_error
				(
					string::format("file '{}' : read error\n", file.path().string())
					, std::make_error_code(std::errc::file_exists))
				);
			};
			if(file.file_size() == 0)
			{
				return "";
			};
			//if(file.file_size() > max_size)
			//{
			//
			//};

			std::string raw; raw.reserve(static_cast<std::size_t>(file.file_size()));

			raw = std::string(
				(std::istreambuf_iterator<char>(stream)),
				(std::istreambuf_iterator<char>())
			);

			if(stream.is_open()) {
				stream.close();
			};

			return raw;
		};
	};
};

namespace utility
{
	class debug_console
	{
	private:
		FILE* in;
		FILE* out;
		FILE* err;
	public:
		debug_console()
		{
			auto alloc = AllocConsole();
			auto bind = alloc ? AttachConsole(ATTACH_PARENT_PROCESS) : false;

			SetConsoleTitle("defragtv");

			freopen_s(&in, "CONIN$", "r", stdin);
			freopen_s(&out, "CONOUT$", "w", stdout);
			freopen_s(&err, "CONOUT$", "w", stderr);
		};
		~debug_console()
		{
			if(in)  fclose(in);
			if(out) fclose(out);
			if(err) fclose(err);
		};

		static std::unique_ptr<debug_console> create()
		{
			return std::make_unique<debug_console>();
		};

		void resize(short x, short y)
		{
			SMALL_RECT CONSOLE_SIZE = { 0, 0, x, y };
			SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &CONSOLE_SIZE);
		};
		void move(int x, int y)
		{
			HWND CONSOLE_WINDOW = GetConsoleWindow();
			SetWindowPos(CONSOLE_WINDOW, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		};
		void show(bool visible)
		{
			ShowWindow(GetConsoleWindow(), visible);
		};
	};

	namespace log
	{
		// TODO requires lots of string allocs for formatting
		class instance
		{
		private:
			const std::string name;
			std::fstream stream;

			std::string file_name()
			{
				//return string::format("{}{}", name, ".log");
				return string::format("{}_{}_{}{}"
									  , name
									  , datetime::datestamp_fs()
									  , datetime::timestamp_fs()
									  , ".log"
				);
			};

			std::string timestamp()
			{
				return datetime::timestamp();//? why is there a newline
			};

			void write_tagged(const std::string& str, bool newline = false)
			{
				auto lts = timestamp();
				std::printf("[%s %s] %s %s", name.c_str(), lts.c_str(), str.c_str(), newline ? "\n" : "");

				if(stream.is_open())
				{
					stream << string::format("{} : {}", lts, str.c_str());
					if(newline) stream << '\n';
				} else
				{
					std::printf("%s : log not accessible\n", name.c_str());
				};
			};
		public:
			instance(const file::path& path, const std::string& name)
				: name(name)
			{
				auto file = file::entry(path / file_name());

				stream.open(file, std::ios::out | std::ios::app);// why doesn't this return bool at least?

				stream.is_open()
					? std::printf("%s : log created\n", name.c_str())
					: std::printf("%s : failed to open file\n\t%s\n", name.c_str(), file.path().string().c_str());
			};
			instance(instance&& move) = default;

			~instance()
			{

				if(stream.is_open())
				{
					stream.flush();
					stream.close();
				};
			};

			static std::unique_ptr<instance> open(const file::path& path, const std::string& name)
			{
				return std::make_unique<instance>(path, name);
			};

			bool is_writable()
			{
				return stream.is_open() && !stream.bad();
			};

			void write(const std::string_view str)
			{
				write_tagged(std::string(str));
			};

			void write(const char* str)
			{
				write_tagged(std::string(str));
			};

			template<typename... ARGs>
			void write(const char* fmt, ARGs&&... args)
			{
				write_tagged(::string::format(fmt, std::forward<ARGs>(args)...));
			};

			void writeln(const std::string_view str)
			{
				write_tagged(std::string(str), true);
			};

			void writeln(const char* str)
			{
				write_tagged(std::string(str), true);
			};

			template<typename... ARGs>
			void writeln(const char* fmt, ARGs&&... args)
			{
				write_tagged(::string::format(fmt, std::forward<ARGs>(args)...), true);
			};

			void flush()
			{
				stream.flush();
			};
		};
	};
};

