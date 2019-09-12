#include "ShellService.h"
#include <boost/process.hpp>
#include "Application.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

ShellService::ShellService(boost::shared_ptr<Application> application): active(false),
                                                                        application(std::move(application))
{
}

void ShellService::spawnShell()
{
	active = true;
	try
	{
		if (process_output_stream == nullptr || !process_output_stream->pipe().is_open())
			process_output_stream = boost::make_unique<boost::process::opstream>();

		if (process_input_stream == nullptr || !process_input_stream->pipe().is_open())
			process_input_stream = boost::make_unique<boost::process::ipstream>();

		c = boost::process::child(
#ifdef _WIN32
			boost::process::search_path("cmd"),
#elif  linux
			boost::process::search_path("bash"),
#endif
			// Pipe process_output_stream to Process StdIn
			boost::process::std_in < *process_output_stream,
			// Pipe Process StdOut and StdErr to process_input_stream
			(boost::process::std_out & boost::process::std_err) > *process_input_stream
		);


		// Pipe Shell to Network
		shell_reader = boost::thread([&,this]
		{
			char buffer[1024];
			try
			{
				while (active)
				{
					if (!c.running())
					{
						int exitCode = c.exit_code();
						std::cout << "Process exited with " << exitCode << std::endl;
						break;
					}

					const int read_so_far = process_input_stream->pipe().read(buffer, 1024);
					buffer[read_so_far] = 0;
					application->onShellData(std::string(buffer, read_so_far));
				}
			}
			catch (std::exception& e)
			{
				std::cout << "catch: " << e.what() << std::endl;
			}

			active = false;
			application->onShellStopped();
			stopShell();
		});
	}
	catch (boost::system::system_error& e)
	{
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
	}
}

void ShellService::writeToShell(const std::string& data)
{
	if (boost::ends_with(data, "\n"))
	{
		process_output_stream->write(data.data(), data.size());
	}
	else
	{
		process_output_stream->write((data + "\n").data(), data.size() + 1);
	}

	process_output_stream->flush();
}

void ShellService::stopShell()
{
	active = false;

	process_output_stream->pipe().close();
	process_input_stream->pipe().close();
	c.terminate();
}

bool ShellService::isShellActive()
{
	return active;
}
