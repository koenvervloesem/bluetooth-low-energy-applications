check:
	black --line-length 70 *.py
	isort *.py
	flake8 *.py
	pylint --disable=duplicate-code,broad-except,redefined-outer-name,invalid-name,unused-argument *.py
