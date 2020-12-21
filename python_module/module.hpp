/*

*/
#pragma once

#include <iostream>

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/embed.h"

#include "utility.hpp"

#include "q3def.h"
#include "api.h"
#include "import.h"

#include "interpreter.hpp"

namespace py = pybind11;

namespace python_module
{
	static std::unique_ptr<utility::debug_console> console;

	class local_paths
	{
	public:
		const file::path root;
		const file::path baseq3;
		const file::path python_module;
		const file::path log;
		const file::path scripts;

		local_paths(const file::path& q3a_root)
			: root(q3a_root)
			, baseq3(root / "baseq3")
			, python_module(root / PYTHON_MODULE_NAME)
			, scripts(python_module / "scripts")
			, log(python_module / "log")
		{};
	};
	static std::unique_ptr<python_module::local_paths> paths;

	// TODO check that path:: sanitizes the filename, to avoid traversal bugs
	static std::map<std::string, utility::log::instance> logs;
	static auto log_get(const char* filename)
	{
		auto it = logs.find(filename);
		if(it == logs.end())
		{
			it = logs.try_emplace(
				file::path(filename).stem().string(),
				std::move(utility::log::instance(paths->log, filename))
			).first;
		};
		return it;
	}
	template<typename... ARGs>
	void log_writeln(const char* filename, const char* fmt, ARGs&&... args)
	{
		auto it = log_get(filename);
		it->second.writeln(fmt, std::forward<ARGs&&>(args)...);
	};
	static void log_writeln(const char* filename, const char* str)
	{
		auto it = log_get(filename);
		it->second.writeln("{}", str);
	};

	static std::unique_ptr<python_module::python_instance> interpreter;

	namespace bindings
	{
		class rgba4f
		{
		private:
			std::array<float, 4> array;
			float to_float(const uint8_t u8)
			{
				return u8 * (1.0f / 255.f);
			};
		public:
			union u8x4
			{
				struct
				{
					uint8_t a, b, g, r;
				};
				uint32_t abgr;

				u8x4(const uint32_t u32)
					: abgr(u32) {};
			};
			rgba4f(float r, float g, float b, float a) :
				array({ r,g,b,a }) {};

			rgba4f(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
				array({ to_float(r)
					  , to_float(g)
					  , to_float(b)
					  , to_float(a) }) {

			};

			rgba4f(u8x4 u32) :
				array({ to_float(u32.r)
					  , to_float(u32.g)
					  , to_float(u32.b)
					  , to_float(u32.a) }) {

			};

			float* get() { return array.data(); };
			float& r() { return array[0]; };
			float& g() { return array[1]; };
			float& b() { return array[2]; };
			float& a() { return array[3]; };
		};

		using abgru32 = uint32_t;

		/*
			binding SCR_ stuff is easier this way,
			and less verbose to use in scripts
		*/
		class hud
		{
		public:
#pragma warning(push)
#pragma warning(disable: 4244)// float/int argument conversion is of no consequence here
			static void image(float x, float y, float w, float h, const char* name) {
				SCR_DrawNamedPic(x, y, w, h, name);
			};
			static void character(float x, float y, int ch) {
				SCR_AdjustFrom640(&x, &y, nullptr, nullptr);
				SCR_DrawSmallChar(x, y, ch);
			};
			static void string(float x, float y, int type, const char* str, abgru32 color = 0xFFFFFFFF) {
				auto rgba = rgba4f(color);
				switch(type)
				{
					case 0: {
						SCR_AdjustFrom640(&x, &y, nullptr, nullptr);
						SCR_DrawSmallStringExt(x, y, str, rgba.get(), qfalse, qfalse);
						break;
					}
					case 1: {
						SCR_AdjustFrom640(&x, &y, nullptr, nullptr);
						SCR_DrawSmallStringExt(x, y, str, rgba.get(), qtrue, qtrue);
						break;
					}
					case 2: {
						SCR_DrawBigString(x, y, str, rgba.a(), qfalse);
						break;
					}
					case 3: {
						SCR_DrawBigStringColor(x, y, str, rgba.get(), qtrue);
						break;
					}
				}
			};
			static void quad(float x, float y, float w, float h, float b, abgru32 color) {
				auto rgba = rgba4f(color);
				if(b == 0.f)
				{
					SCR_FillRect(x, y, w, h, rgba.get());
				} else
				{
					auto uh = [ ] (float c)
					{
						return c > 0.f ? c : 0.f;
					};
					auto lh = [ ] (float c)
					{
						return c < 0.f ? c : 0.f;
					};
					SCR_FillRect(x - uh(b), y - uh(b), w + uh(b) + uh(b), abs(b), rgba.get());//T
					SCR_FillRect(x - uh(b), y + h + lh(b), w + uh(b) + uh(b), abs(b), rgba.get());//B
					SCR_FillRect(x - uh(b), y - lh(b), abs(b), h + lh(b) + lh(b), rgba.get());//L
					SCR_FillRect(x + w + lh(b), y - lh(b), abs(b), h + lh(b) + lh(b), rgba.get());//R
				};
			};
#pragma warning(pop)
		};

		void register_callback(const char* name, py::function fn)
		{
			interpreter->register_callback(name, fn);
		};

		void try_invoke_callback(const char* name)
		{
			interpreter->try_invoke_callback(name);
		};

		PYBIND11_EMBEDDED_MODULE(utility, m)
		{
			m.doc() = "defragtv utilities\n";

			py::class_<rgba4f>(m, "color")
				.def(py::init<float, float, float, float>())
				.def(py::init<uint8_t, uint8_t, uint8_t, uint8_t>())
				.def(py::init<uint32_t>())
				.def("get", &rgba4f::get)
				.def("r", &rgba4f::r)
				.def("g", &rgba4f::g)
				.def("b", &rgba4f::b)
				.def("a", &rgba4f::a)
				;

			py::class_<abgru32>(m, "ucolor");

			py::class_<hud>(m, "hud")
				.def_static("image", &hud::image)
				.def_static("character", &hud::character)
				.def_static("string", &hud::string)
				.def_static("quad", &hud::quad)
				;

			m.def("register_callback", &register_callback, "try to register a function for direct calls from the client");
		};

		PYBIND11_EMBEDDED_MODULE(log, m)
		{
			m.doc() = "write to /module_dir/log/<filename>.log\n";
			m.def("writeln", [ ] (const char* filename, const char* str)
			{
				log_writeln(filename, str);
			});
		};

		PYBIND11_EMBEDDED_MODULE(q3a, m)
		{
			m.doc() = "quake 3 arena : python bindings for defragtv\n";

			py::enum_<qboolean>(m, "qboolean")
				.value("qfalse", qboolean::qfalse)
				.value("qtrue", qboolean::qtrue);

#define PY_BIND_ENUM(e, v) .value(#v, e::v)
			py::enum_<cvar_flag_bits>(m, "cvar_flag_bits")
				PY_BIND_ENUM(cvar_flag_bits, ARCHIVE)
				PY_BIND_ENUM(cvar_flag_bits, USERINFO)
				PY_BIND_ENUM(cvar_flag_bits, SERVERINFO)
				PY_BIND_ENUM(cvar_flag_bits, SYSTEMINFO)
				PY_BIND_ENUM(cvar_flag_bits, INIT)
				PY_BIND_ENUM(cvar_flag_bits, LATCH)
				PY_BIND_ENUM(cvar_flag_bits, ROM)
				PY_BIND_ENUM(cvar_flag_bits, USER_CREATED)
				PY_BIND_ENUM(cvar_flag_bits, TEMP)
				PY_BIND_ENUM(cvar_flag_bits, CHEAT)
				PY_BIND_ENUM(cvar_flag_bits, NORESTART)
				PY_BIND_ENUM(cvar_flag_bits, SERVER_CREATED)
				PY_BIND_ENUM(cvar_flag_bits, VM_CREATED)
				PY_BIND_ENUM(cvar_flag_bits, PROTECTED)
				PY_BIND_ENUM(cvar_flag_bits, MODIFIED)
				PY_BIND_ENUM(cvar_flag_bits, NONEXISTENT)
				;
#undef PY_BIND_ENUM

			// wrapped to avoid C variadics, use python string formatting instead
			m.def("Com_Printf", [ ] (const std::string& str) -> void { Com_Printf("%s", str.c_str()); });
#pragma warning(push)
#pragma warning(disable: 4180)//
#define PY_BIND_Q3A(name) m.def(#name, name);
			PY_BIND_Q3A(CL_AddReliableCommand);
			PY_BIND_Q3A(Cbuf_ExecuteText);
			PY_BIND_Q3A(Cmd_AddCommand);
			PY_BIND_Q3A(Cmd_Argc);
			PY_BIND_Q3A(Cmd_Args);
			PY_BIND_Q3A(Cmd_Argv);
			PY_BIND_Q3A(Cvar_Set);
			PY_BIND_Q3A(Cvar_VariableString);
			PY_BIND_Q3A(Cvar_VariableValue);
			PY_BIND_Q3A(Info_ValueForKey);
			PY_BIND_Q3A(CL_GetPlayerNameForClient);//yeah it crashes, obv TODO ???
			PY_BIND_Q3A(CL_GetServerMapName);
			PY_BIND_Q3A(CL_DoesMapExist);
#undef PY_BIND_Q3A
#pragma warning(pop)
		};
	};
};
