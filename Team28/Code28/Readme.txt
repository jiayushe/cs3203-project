# Recommended (Visual Studio + Powershell)

Build, unit, and integration tests:
1. Use Visual Studio
  1.1. Build in "Release" mode
  1.2. Upon successful build, the path to `autotester` binary will be `Team28\Code28\build_win\x86-Release\src\autotester`

Autotester (Powershell):
1. Ensure that code has been built successfully
2. Run Windows Powershell as Administrator
3. cd to the directory `Team28\Code28`
4. You may need to run `Set-ExecutionPolicy Unrestricted` to enable execution of remote scripts
5. Run .\autotest.ps1

If the abovementioned script failed to run/generate output, you may manually run Autotester with the following command in directory "Team28\Code28":
.\build_win\x86-Release\src\autotester\autotester.exe ..\Tests28\1_source.txt ..\Tests28\1_queries.txt ..\Tests28\1_out.xml
.\build_win\x86-Release\src\autotester\autotester.exe ..\Tests28\2_source.txt ..\Tests28\2_queries.txt ..\Tests28\2_out.xml
.\build_win\x86-Release\src\autotester\autotester.exe ..\Tests28\3_source.txt ..\Tests28\3_queries.txt ..\Tests28\3_out.xml

# Alternative (Bash)

Working directory = Team28/Code28

Build:
1. mkdir build
2. cd build
3. cmake -A Win32 ..
4. cmake --build . --target autotester --config Release
5. cmake --build . --target unit_testing --config Release
6. cmake --build . --target integration_testing --config Release

Unit and integration tests:
1. Ensure that code has been built successfully
2. build/src/unit_testing/Release/unit_testing.exe -d yes --order lex
3. build/src/integration_testing/Release/integration_testing.exe -d yes --order lex

Autotester:
1. Ensure that code has been built successfully
2. build/src/autotester/Release/autotester.exe ../Tests28/1_source.txt ../Tests28/1_queries.txt ../Tests28/1_out.xml
3. build/src/autotester/Release/autotester.exe ../Tests28/2_source.txt ../Tests28/2_queries.txt ../Tests28/2_out.xml
4. build/src/autotester/Release/autotester.exe ../Tests28/3_source.txt ../Tests28/3_queries.txt ../Tests28/3_out.xml
