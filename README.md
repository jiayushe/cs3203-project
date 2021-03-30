# CS3203 Team 28

[![CircleCI](https://circleci.com/gh/nus-cs3203/20s2-cp-spa-team-28/tree/master.svg?style=shield&circle-token=0342347ca6fa1b3cda5dc5a6b871b511634e9da1)](https://app.circleci.com/pipelines/github/nus-cs3203/20s2-cp-spa-team-28?invite=true)

## Development

### Formatting the code
```bash
make format
```

### Building the project

There are a few ways you can build the project:
1. Use your favourite IDE, e.g. Visual Studio or CLion.
2. Use the Bash scripts provided in `Team28/Code28/scripts` (see [Installing Python and its packages](#installing-python-and-its-packages)). You can run either:
   - `Team28/Code28/scripts/build.sh`, which builds the project, or
   - `Team28/Code28/scripts/build_and_autotest.sh`, which builds the project and runs the system tests defined in `Team28/Tests28` using autotester.

### Running autotester

There are a few ways you can run autotester:
1. Powershell
   - Build the project (see [Building the project](#building-the-project)).
   - Run Windows Powershell as Administrator.
   - You may need to run `Set-ExecutionPolicy Unrestricted` to enable execution of remote scripts.
   - Run `cd Team28\Code28\scripts`.
   - Run `.\autotest.ps1`.
2. Python
   - Install Python 3.6+ and some packages (see [Installing Python and its packages](#installing-python-and-its-packages)).
   - Build the project (see [Building the project](#building-the-project)).
   - Run `python autotest.py PATH_TO_AUTOTESTER_BINARY` (replace `PATH_TO_AUTOTESTER_BINARY` with the path to your autotester binary).
3. Bash
   - Install Python 3.6+ and some packages (see [Installing Python and its packages](#installing-python-and-its-packages)).
   - Run `Team28/Code28/scripts/build_and_autotest.sh`.

### Installing Python and its packages

Some of our scripts in `Team28/Code28/scripts`, namely `build.sh`, `build_and_autotest.sh`, and `autotest.py`, depend on Python 3.6+ and some packages which can be installed from pip.

To run these scripts:
- Ensure that Python 3.6+ is installed.
- Run `pip install -r Team28/Code28/scripts/requirements.txt`.
