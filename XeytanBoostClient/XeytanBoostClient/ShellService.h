#pragma once
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/make_unique.hpp>

class Application;

class ShellService
{
public:
	ShellService(boost::shared_ptr<Application> application);
	void spawnShell();
	void writeToShell(const std::string& data);
	void stopShell();
	bool isShellActive();

private:
	bool active;
	boost::shared_ptr<Application> application;

	std::unique_ptr<boost::process::ipstream> process_input_stream;
	std::unique_ptr<boost::process::opstream> process_output_stream;

	boost::process::child c;
	boost::thread shell_reader;
};
