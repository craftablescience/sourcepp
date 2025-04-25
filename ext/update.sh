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
  cd miniz || exit
  git checkout master
  git pull
)
(
  cd minizip-ng || exit
  git checkout develop
  git pull
)
