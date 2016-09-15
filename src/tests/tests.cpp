
#define CATCH_CONFIG_RUNNER
#include "catch/single_include/catch.hpp"


int main(int argc, char *const argv[])
{
	// utilize ccache:
	// since this file is always the same and it's compilation is slow
	// so put the tests in other translation units to speed up compilation

	srand(time(NULL));
	return Catch::Session().run(argc, argv);
}
