#! /usr/bin/env bash

guetzli --nomemlimit --quality 84 header.png header.jpg &
guetzli --nomemlimit --quality 84 background.png background.jpg &
guetzli --nomemlimit --quality 93 splash.png splash.jpg &

wait
