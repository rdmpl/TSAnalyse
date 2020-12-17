.PHONY: all doc app clean
all:app doc
app:
	cmake -Happ -Bbuild/app
	cmake --build build/app
test:
	cmake -Htest -Bbuild/test
	cmake --build build/app
doc:
	cmake -Hdocumentation -Bbuild/doc
	cmake --build build/doc --target GenerateDocs
clean:
	rm -rf build/
