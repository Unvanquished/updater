#!/bin/bash
# Intended to be used on OS X as it does not ship with md5sum
badAssets=();

while IFS=" *" read assetMD5 assetName; do
	MD5Output=($( md5 -r $assetName 2>&1 ))
	if [[ ${MD5Output[0]} != $assetMD5 && ${MD5Output[@]} != *'No such file or directory'  ]]; then
		badAssets+=($assetName);
	fi;
done < md5sums0.19;

for asset in "${badAssets[@]}"; do
	echo "$asset";
done;

