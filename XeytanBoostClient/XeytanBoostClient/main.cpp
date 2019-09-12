#include "Application.h"
#include "services/FileSystemService.h"
#include "appdefs.h"
boost::shared_ptr<Application> appInstance;


void test()
{
}

int main(int argc, char** argv)
{
	test();
	appInstance = Application::getInstance();
	appInstance->run();
	return 0;
}
