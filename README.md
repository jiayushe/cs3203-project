# CS3203 Team 28

[![CircleCI](https://circleci.com/gh/nus-cs3203/20s2-cp-spa-team-28/tree/master.svg?style=shield&circle-token=0342347ca6fa1b3cda5dc5a6b871b511634e9da1)](https://app.circleci.com/pipelines/github/nus-cs3203/20s2-cp-spa-team-28?invite=true)

## Development

### Code formatter
```bash
make format
```

### Autotester

_Ensure autotester binary is available (build the project)_

- Powershell
  1. Run Windows Powershell as Administrator
  2. You may need to run `Set-ExecutionPolicy Unrestricted` to enable execution of remote scripts
  3. `cd Team28\Code28\scripts`
  4. `.\autotest.ps1`

- Python
  1. Ensure python 3.6+ is installed (tested on python 3.6.8)
  2. `cd Team28/Code28/scripts`
  3. (Optional) `python -m venv venv`
  4. `pip install -r requirements.txt`
  5. `python autotest.py PATH_TO_AUTOTESTER_BINARY`

