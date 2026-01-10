all:
	xcodegen generate
	xcodebuild -configuration Release -alltargets SYMROOT="$(CURDIR)/build"

clean:
	rm -fr build *.xcodeproj

install: all
	rsync -a --delete build/Release/unsigned.app ../Applications

codesign-display:
	codesign -dvv build/Release/unsigned.app
