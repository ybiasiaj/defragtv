/*

*/
#pragma once

#include <iostream>
#include <map>
#include <string>
#include <string_view>

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"

#include "utility.hpp"

namespace py = pybind11;

// TODO use path when looking for module import
// TODO test that import can find common python lib installs?
namespace python_module
{
	template<typename... ARGs>
	void log_writeln(const char* filename, const char* fmt, ARGs&&... args);

	// pybind11::scoped_interpreter is broken, hence
	class python_instance
	{
	private:
		struct unique_initializer {};
	public:
		python_instance(unique_initializer)
		{
			py::initialize_interpreter();
		};
		~python_instance()
		{
			py::finalize_interpreter();
		};

		static std::unique_ptr<python_instance> create()
		{
			return std::make_unique<python_instance>(unique_initializer{});
		};

		std::map<std::string, py::module> imports;// TODO unused?
		std::map<std::string, py::function> callbacks;

		void try_import(const std::string& module)
		{
			std::pair<std::string, py::module> result;
			try
			{
				result = { module, py::module::import(module.c_str()) };
			} catch(py::import_error const& err)
			{
				log_writeln("module", "import error {}", err.what());
				return;
			};
			imports.emplace(std::move(result));
		};

		void register_callback(const char* name, py::function fn)
		{
			if(callbacks.count(name) != 0)
			{
				log_writeln("module", "interpreter error : callback {} already registered, cannot override", name);
				return;
			};
			if(!fn)
			{
				log_writeln("module", "interpreter error : callback {} is null", name);
				return;
			};
			callbacks.emplace(name, fn);
			log_writeln("module", "interpreter : callback {} registered", name);
		};

		void try_invoke_callback(const char* name)
		{
			auto it = callbacks.find(name);
			if(it == callbacks.end())
			{
				log_writeln("module", "invoke error : no registered function {}", name);
			} else
			{
				//PY_SCRIPT_LOG("invoke : {}", name);
				try
				{
					it->second();

				} catch(std::runtime_error err)
				{
					log_writeln("module", "	-> {}", err.what());
				}
			};
		};

		void try_invoke_callback()
		{
			Com_Printf("^6py_invoke %i\n", Cmd_Argc());
			if(Cmd_Argc() > 1)
			{
				try_invoke_callback(Cmd_Argv(1));
			};

		};

		void try_eval(const char* string)
		{
			if(std::string_view(string).length() == 0) return;
			try
			{
				py::eval<py::eval_single_statement>(string, py::globals());
			} catch(std::runtime_error err)
			{
				log_writeln("module", "eval error : {}", err.what());
			};
		};

		void try_eval()
		{
			Com_Printf("^6py_eval %i\n", Cmd_Argc());

			log_writeln("module", "py_eval");
			for(int i = 0; i < Cmd_Argc(); i++)
			{
				log_writeln("module", "{}	{}", i, Cmd_Argv(i));
			};
			if(Cmd_Argc() > 1)
			{
				try
				{
					py::eval<py::eval_single_statement>(Cmd_Args());
				} catch(std::runtime_error err)
				{
					log_writeln("module", "py_eval error : {}", err.what());
				};
			};
		};

		void try_exec(const char* string)
		{
			if(std::string_view(string).length() == 0) return;
			try
			{
				py::exec(string);
			} catch(std::runtime_error err)
			{
				log_writeln("module", "exec error : {}", err.what());
			};
		};
	};
};