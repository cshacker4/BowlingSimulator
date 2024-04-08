# Remove the build directory recursively
Remove-Item -Recurse -Path "build"

# Run cmake and ninja commands in the build directory
mkdir build
cd build
cmake -G "Ninja" ..
ninja

# Move the executable file from the build directory to its parent directory
Move-Item -Path ".\myProgram.exe" -Destination "..\myProgram.exe" -Force

#run program
cd ..
.\myProgram.exe
