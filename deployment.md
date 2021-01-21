## Network resources used by the updater
- News REST endpoint which returns links to Wordpress articles on unvanquished.net. The featured image in each news article must be a type which is supported by the updater (see issue #51). Currently PNG and JPEG are known to work.
- versions.json file on unvanquished.net, used to determine whether update is needed
- Github releases. These are targeted by download links un unvanquished.net and by the updater's self-update process.
- Torrent URL used to download the latest game version

## Release process
1. Create a new Git tag.
2. Make a Github release with the same name as the tag. Build and upload the files for each platform:
    ### Mac
    1. `rm -rf build/`
    2. Follow the __Build Mac version natively__ instructions in the README.
    3. Zip
        ```
        mv updater2.app 'Unvanquished Updater.app'
        zip -r UnvUpdaterOSX.zip 'Unvanquished Updater.app'
        ```
    4. Upload `UnvUpdaterOSX.zip`.

    ### Linux
    1. Build with Docker and extract files:
        ```
        docker build -t updater2 . --build-arg=release=1
        docker run -v `pwd`/build-docker:/build-docker -u `id -u $USER` updater2
    2. Upload `UnvUpdaterLinux.zip` from `build-docker/`.

    ### Windows
    1. Build with Docker and extract files:
        ```
        docker build -t updater2-win -f Dockerfile.win . --build-arg=release=1
        docker create --name updater2-win updater2-win
        docker cp updater2-win:/release-win ./build-docker
        docker rm updater2-win
        ```
    2. Upload `UnvUpdaterWin.zip` and `UnvanquishedUpdater.exe` from `build-docker/release-win/`.

4. Bump the updater version on unvanquished.net to the new tag, so that it is reflected in versions.json and the download links.
