check:
	black --line-length 70 *.py
	isort *.py
	flake8 --ignore=E203 *.py
	pylint --disable=duplicate-code,broad-except,redefined-outer-name,invalid-name,unused-argument,global-statement *.py
