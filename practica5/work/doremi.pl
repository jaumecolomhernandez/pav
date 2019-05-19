#!/usr/bin/perl -w

# Configuration
$octave_shift = -0*12;
@notes = (60, 62, 64, 65, 67, 69, 71, 72); # melody
$beats_per_note_on = 3;                    # time
$beats_per_note_off = 1;                   # time
#-----------------------------------------------


$ticks_per_beat = 120;

$ticks_per_note_off = int(0.5+$beats_per_note_on   * $ticks_per_beat);
$ticks_per_note_on = int(0.5 + $beats_per_note_off * $ticks_per_beat);

$INSTRUMENT = 1;
$KEY_PRESSED = 9;
$KEY_RELEASED = 8;
$VELOCITY = 100;


do {
    $note = shift(@notes) + $octave_shift;
    
    print "$ticks_per_note_on\t$KEY_PRESSED\t$INSTRUMENT\t$note\t$VELOCITY\n";
    print "$ticks_per_note_off\t$KEY_RELEASED\t$INSTRUMENT\t$note\t$VELOCITY\n";
} while (@notes);

print "0\t0\t$INSTRUMENT\t0\t0\n";

