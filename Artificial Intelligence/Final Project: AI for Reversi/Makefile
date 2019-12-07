all:
	mkdir dist
	python3 setup.py build_ext --inplace
	mv *.so dist/
	rm *.c

clean:
	rm dist/*
