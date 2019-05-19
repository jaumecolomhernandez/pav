#!/bin/sh


#Script to run the synth and play the computed audio

make -C ./prj all

./bin/release/synth ./work/instruments.txt ./scores/ToyStory_A_Friend_in_me.sco ./out2.wav

play ./out2.wav