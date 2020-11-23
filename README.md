tinyweb
===
A tiny web server that serves files and compiles c source codes on the fly. The c source codes will run on HTTP request.

# BEFORE YOU PROCEED
You should be aware that this project is ***NOT SAFE*** for testing people's project.
Any C source code will be run on the tinyweb program, meaning that it will run any code you feed in,
and can be a ***SERIOUS SECURITY THREAT*** if you don't know what you are doing.  
I can't guarantee that tinyweb will run smoothly, or even correctly, because it's still on very early testing.
Only try this if you know what you are trying to do.

## Features
- [X] Scripts run super fast, since it directly runs on compiled c source code.
- [ ] Quick html page load, since it caches every html content in memory.

## Building
You will need the latest TCC release (should be on system path, preferably `tcc-0.9.27-winXX`), and the full winapi include for the TCC. The full winapi archive can be found in the tinycc releases repository (preferably `winapi-full-for-0.9.27.zip`).

### Setting Up TCC
> These steps will perform changes to your TCC files. Make sure you backed up your TCC compiler.

1. Back up the TCC include folder.
2. Delete the TCC include folder and replace it with the include folder inside the full-winapi archive.  
   The full-winapi archive contains the `winsock` header files.
3. Replace the content of the TCC include folder with the content of the original TCC include folder.  
   Original TCC include folder contains purged `GLIBC` stuff.

### Setting up tinyweb
> Make sure that tcc is in the system `PATH` by typing `echo %PATH%` inside cmd and search for tcc installation folder.

Copy `libtcc.def` and `libtcc.h` inside `libtcc` folder of TCC, and `libtcc.dll` inside TCC folder to the tinyweb folder, then run `build`.

## Contributing
Any changes to this file, any typo fixes, code fixes, code improvements, and new documentations are very welcome.
