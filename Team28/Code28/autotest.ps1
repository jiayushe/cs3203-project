$cnt=[System.IO.Directory]::GetFiles("$pwd\..\Tests28\", "*_source.txt").Count
Write-Host "$cnt test suites found."
For ($i=1; $i -le $cnt; $i++) {
    Write-Host "Test suite $i running..."
    Invoke-Expression "& `".\build_win\x86-Release\src\autotester\autotester.exe`" ..\Tests28\${i}_source.txt ..\Tests28\${i}_queries.txt ..\Tests28\${i}_out.xml | out-null"
    Write-Host "Test suite $i completed."
}
