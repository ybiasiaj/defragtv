/*

*/
#define PY_MODULE 1
#define PYTHON_CLIENT_USE_EVAL 0
#include "api.h"
#include "module.hpp"

extern "C" {
	void python_module_initialize()
	{
		using namespace python_module;
		const auto cd = std::filesystem::current_path();
		if(!paths)
		{
			paths = std::make_unique<python_module::local_paths>(cd);
		}
		if(!console)
		{
			console = utility::debug_console::create();
		};
		if(!interpreter)
		{
			interpreter = python_module::python_instance::create();
		};

		log_writeln("module", "{} {}"
					, datetime::timestamp()
					, datetime::datestamp()
		);
		log_writeln("module", "initialized");
	};
	void python_module_shutdown()
	{
		using namespace python_module;

		log_writeln("module", "shutdown");

		//resetting console could be a problem, python expects stdout != null
		//
		console.reset();
		interpreter.reset();

		if(logs.size() > 0)
		{
			logs.clear();
		};
	};

	void python_module_invoke(const char* function)
	{
		python_module::interpreter->try_invoke_callback(function);
	};

	void python_module_invoke_cmd()
	{
		python_module::interpreter->try_invoke_callback();
	};

	void python_module_eval(const char* string)
	{
#if PYTHON_CLIENT_USE_EVAL != 0
		python_module::interpreter->try_eval(string);
#endif
	};

	void python_module_eval_cmd()
	{
#if PYTHON_CLIENT_USE_EVAL != 0
		python_module::interpreter->try_eval();
#endif
	};

	void python_module_load()
	{
		using namespace python_module;
		py::module sys = py::module::import("sys");
		// paths->python_module . PYTHON_MODULE_NAME
		interpreter->try_exec("from ioq3_py.ioq3_py import *");

		for(auto& [k, v] : interpreter->imports)
		{
			log_writeln("module", "import : {}", k);
		};
	};

	void python_module_reload()
	{
		using namespace python_module;
		log_writeln("module", "reloading");
		python_module_shutdown();
		python_module_initialize();
		python_module_load();
	};
};
