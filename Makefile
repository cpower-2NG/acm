PYTHON ?= conda run -n acm python

.PHONY: index clean-cph

index:
	$(PYTHON) scripts/update_problem_index.py

clean-cph:
	find problems -type f \( -name '*.prob' -o -name '.*.prob' \) -delete
	find problems -type d -name '.cph' -exec rm -rf {} +
