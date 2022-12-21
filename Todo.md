<style type="text/css">
	c { color:#00FF00; }
	c::before { content: "✓"; }
	nc::before { content: "☐"; }
</style>

# To-do

1. <c></c> Check for linking errors.  
2. <c></c> Allow the user to enable debugging.  
4. <c></c> Fix redundency in std::build::build_type naming.  
5. <c></c> Revert from concepts to traditional sfinae to lower the required standard version needed when building stdbuild.  
8. <c></c> Automatically call exit(1) when create_executable fails and allow that to be disabled.
9. <c></c> Rename std::build::build_path.  
10. <c></c> Add help command to stdbuild
11. <c></c> Create and cache an include hierarchy.
12. <c></c> Add helper functions for checking for things like which compiler is being used.
13. <c></c> Remove constexpr support.
3. <nc></nc> Fully implement upward propagation of flags, include/lib directories, and packages.  
6. <nc></nc> Begin work on cross platform and cross compiler support.  
7. <nc></nc> Compilation parallelization.  
14. <nc></nc> Add caching to libraries instead of just the main project.
15. <nc></nc> Cleanup normal/verbose build output.