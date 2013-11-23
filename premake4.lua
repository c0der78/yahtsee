if _ACTION == "clean" then
    matches = os.matchfiles("**.orig")
    for i=1, #matches do
        os.remove(matches[i])
    end
    os.rmdir("bin")
    os.rmdir("obj")
end
newaction {
   trigger     = "indent",
   description = "Format source files",
   execute = function ()
      os.execute("astyle -N -A1 -R '*.cpp' '*.h'");
   end
}

solution "yahtzee"
    configurations { "Debug", "Release" }

    language "C++"

    buildoptions { "-std=c++11", "-stdlib=libc++", "-Wall", "-Werror"}

    linkoptions { "-stdlib=libc++" }

    configuration "Debug"
        buildoptions { "-D_DEBUG" }
        flags "Symbols"
        targetdir "bin/Debug"

    configuration "Release"
        targetdir "bin/Release"

    project "yahtzee"
    	kind "ConsoleApp"
    	files {
    		"main.cpp"
    	}
    	links {
    		"arg3dice", "caca"
    	}

    project "yahtest"
    	kind "ConsoleApp"
    	files {
    		"**.test.cpp"
    	}
    	links {
    		"arg3dice", "caca"
    	}
    	configuration "Debug"
	    	postbuildcommands {
	    		"bin/Debug/yahtest"
	    	}
    	configuration "Release"
	    	postbuildcommands {
	    		"bin/Release/yahtest"
	    	}