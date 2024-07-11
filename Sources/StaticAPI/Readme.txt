These API source files should be used when linking specific Serious Engine projects together.
Projects that need to be properly statically linked have an inline source file (.inl) of the same name as the project folder under "Sources/".

These files are supposed to be included *only once* in the entire project, preferably in some main source file in order to define all the necessary functionality and also link the library itself.
It can be included in a file such as "StdH.cpp", "Main.cpp", "<project name>.cpp" or similar.
