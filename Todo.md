<style type="text/css">
	c {
		color:#00FF00;
	}

	c::before {
		content: "✓";
		/* ✓🗹 */
	}

	nc::before {
		content: "☐";
		/* ☐ */
	}
</style>

# To-do

<c></c> Check for linking errors.  
<c></c> Allow the user to enable debugging.  
<nc></nc> Fully implement upward propagation of flags, include/lib directories, and packages.  
<nc></nc> Fix redundency in std::build::build_type naming.  
<nc></nc> Revert from concepts to traditional sfinae to lower the required standard version needed when building stdbuild.  
<nc></nc> Begin work on cross platform and cross compiler support.  
<nc></nc> Compilation parallelization.  