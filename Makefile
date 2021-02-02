FILE_LIST := $(shell find . | grep -E ".*(\.cpp|\.h)$$")

.PHONY: format help

format: ## Lint all files	
	clang-format -i -style=file ${FILE_LIST}

help: ## Display help screen
	grep -h -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
