#!/usr/bin/env bash
(
  cd bufferstream || exit
  git checkout main
  git pull
)
(
  cd cryptopp || exit
  git checkout master
  git pull
)
(
  cd hat-trie || exit
  git checkout master
  git pull
)
(
  cd libwebp || exit
  git checkout main
  git pull
cd ..
)
(
  cd miniz || exit
  git checkout master
  git pull
)
(
  cd minizip-ng || exit
  git checkout develop
  git pull
)
(
  cd qoi || exit
  git checkout master
  git pull
)
