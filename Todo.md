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

1. <c></c> Check for linking errors.  
2. <c></c> Allow the user to enable debugging.  
3. <nc></nc> Fully implement upward propagation of flags, include/lib directories, and packages.  
4. <nc></nc> Fix redundency in std::build::build_type naming.  
5. <nc></nc> Revert from concepts to traditional sfinae to lower the required standard version needed when building stdbuild.  
6. <nc></nc> Begin work on cross platform and cross compiler support.  
7. <nc></nc> Compilation parallelization.  