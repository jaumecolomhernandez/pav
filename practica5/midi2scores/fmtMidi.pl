#!/usr/bin/perl -w

if (@ARGV != 1) {
    print STDERR "$0 filein.midi\n";
    exit -1;
}

$cmd = "midi2skini";

$bpm=154;
$tpq=120;
@linesAll = `$cmd $ARGV[0]`;
@linesNotes = grep {/^Note/} @linesAll; 
@linesTracks = grep {/track:.*text/} @linesAll; 

for $_ (@linesTracks) {
    s/.*track:/track:/;
    print "# $_";
}

$last_ticks = 0;
for $_ (@linesNotes) {
    chomp;
    if (5 != (($event, $t, $ch, $note, $vol) = split)) {
	print STDERR "Warning: format error line $_\n";
	next;
    }
    if (! (($event =~ s/NoteOn/9/) || ($event =~ s/NoteOff/8/))) {
	print STDERR "Warning: format error in note ($event), line: $_\n";
	next;
    }
    $t =~ s/=//;
    $b = $t * $bpm/60;
    $ticks = int(0.5+ $b * $tpq);
    $delta = $ticks - $last_ticks;

#    print "$t\t";
    print "$delta\t$event\t$ch\t$note\t$vol\n";
    $last_ticks = $ticks;

}

#    if (istr >> lTimeTick >> cEvent >> cChannel >> param[0] >> param[1])
