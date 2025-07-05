#!/usr/bin/env bash
(
  cd bc7enc_rdo || exit
  git checkout master
  git pull
)
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
(
  cd qoi || exit
  git checkout master
  git pull
)
