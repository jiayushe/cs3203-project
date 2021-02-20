$cnt=[System.IO.Directory]::GetFiles("$pwd\Team28\Tests28\", "*.source.txt").Count
Write-Host "$cnt test suites found."
For ($i=1; $i -le $cnt; $i++) {
    Write-Host "Test suite $i running..."
    Invoke-Expression "& `".\Team28\Code28\build_win\x86-Release\src\autotester\autotester.exe`" .\Team28\Tests28\$i.source.txt .\Team28\Tests28\$i.queries.txt .\Team28\Tests28\$i.out.xml | out-null"
    Write-Host "Test suite $i completed."
}
