# !/ bin / bash
# ponga aqui el nombre de su fichero . wav sin extension

F = /audios/output-16k-mono
G= /exports/exports.txt
./pav_analysis $F.wav $G.txt
cut -f 2 $G.txt > $G.pot
cut -f 3 $G.txt > $G.am
cut -f 4 $G.txt > $G.zcr
exit 0
