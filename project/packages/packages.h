#include "../../stdbuild"

#include "pkg1/build.h"
#include "pkg2/build.h"

const stdbuild::package_list packages = {
	PKG1("packages/pkg1/"),
	PKG2("packages/pkg2/")
};