#! /bin/bash

doxygen
cp -r ./docs/html ./.public
git checkout gh-pages
git reset --hard origin/gh-pages
cp -r ./.public/* .
rm -rf ./.public
git add -A
git commit -a -m 'gh-pages update'
git push origin gh-pages --force && git checkout master
