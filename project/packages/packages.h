#include <stdbuild.h>

//#include "pkg1/build.h"
//#include "pkg2/build.h"
#include "MyLibrary/build.h"

const stdbuild::package_list packages = {
	/*PKG1("packages/pkg1/"),
	PKG2("packages/pkg2/")*/
	MyLibrary("packages/MyLibrary/")
};