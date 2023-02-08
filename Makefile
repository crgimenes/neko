@all:
	mkdir -p dist
	GOOS=windows GOARCH=amd64 CGO_ENABLED=0 \
	go build -a -ldflags '-extldflags "-static" -s -w' -o dist/neko_windows_amd64.exe
	#GOOS=linux GOARCH=amd64 \
	#go build -o dist/neko_linux_amd64
	GOOS=darwin GOARCH=arm64 \
    go build -a -ldflags '-s -w' -o dist/neko_darwin_arm64

@clean:
	rm -rf dist
