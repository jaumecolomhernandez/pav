#

F=ToyStory_A_Friend_in_me


if [[ ! -f $F.mid ]]; then echo "Not found $F.mid"; exit 1; fi
if [[ ! -x ./midi2skini ]]; then echo "Not found exec ./midi2skini"; exit 1; fi
if [[ ! -x ./fmtMidi.pl ]]; then echo "Not found exec ./fmtMidi.pl"; exit 1; fi


# Check that midi2skini works
echo ./midi2skini  $F.mid
./midi2skini  $F.mid >  $F.fmt || (echo 'Error executing midi2kini'; exit 1)


# midi2skini is used by this simple perl script
echo ./fmtMidi.pl  $F.mid
./fmtMidi.pl $F.mid > $F.sco || (echo 'Error formatting score'; exit 1)

ls -lh $F.sco
