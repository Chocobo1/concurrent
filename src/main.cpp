
#include "main.h"

#include "helpers.h"
#include "master.h"


static bool isPipe(const char *arg);
static void printUsage(const char *cmdName);


int main(const int argc, const char *argv[])
{
	const unsigned workers = Helpers::defaultWorkersNumber();

	if(argc == 2)
	{
		if(isPipe(argv[1]))
			Master m(nullptr, workers);  // stdin (pipe)
		else
			Master m(argv[1], workers);  // read file
	}
	else
		printUsage(argv[0]);

	return EXIT_SUCCESS;
}


void printUsage(const char *cmdName)
{
	printf("\n");
	printf("A naive tool that take commands from text file (or pipe) and execute it concurrently.\n");
	printf("Version: %s\n", PROGRAM_VERSION);

	printf("\n");
	printf("Usage: %s [options]\n", cmdName);

	printf("\n");
	printf("Options:\n");
	printf("    - \t\t\t take commands from stdin (pipe)\n");
	printf("    <file_path> \t take commands from specified file \n");
	printf("\n");
}


bool isPipe(const char *arg)
{
	return (std::string("-").compare(arg) == 0);
}
