@all:
	mkdir -p dist
	GOOS=windows GOARCH=amd64 CGO_ENABLED=0 \
	go build -a -ldflags '-extldflags "-static" -s -w' -o dist/neko_windows_amd64.exe
	#GOOS=linux GOARCH=amd64 CGO_ENABLED=1 \
	#go build -o dist/neko_linux_amd64
	GOOS=darwin GOARCH=amd64 CGO_ENABLED=0 \
	go build -a -ldflags '-s -w' -o dist/neko_darwin_amd64
	GOOS=darwin GOARCH=arm64 CGO_ENABLED=0 \
	go build -a -ldflags '-s -w' -o dist/neko_darwin_arm64
	lipo -create -output dist/neko_darwin_universal \
		dist/neko_darwin_amd64 dist/neko_darwin_arm64
	rm -f dist/neko_darwin_amd64 dist/neko_darwin_arm64

@clean:
	rm -rf dist
