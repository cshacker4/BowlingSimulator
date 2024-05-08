# Define the folder containing the rapidjson library
$rapidjsonFolderPath = "C:\Users\Bryce.Valverde\CS473\Joycon\include"

# Define the source file to compile
$sourceFile = ".\joycon.cpp"

# Define the output executable file
$outputFile = ".\joycon.exe"

# Compile the program with g++
g++ -I $rapidjsonFolderPath $sourceFile -o $outputFile

