#!/usr/bin/perl -w
#
# Lab of Speech and Audio Processing (PAV)
# Antonio Bonafonte, UPC
# Barcelona, 2016
# ------------------------------------
# Compare temporal segmentation
# Given a reference file, .lab with a segmentation
# Ex:
# 0.0    2.5 S
# 2.5    4.0 V
# 4.0    6.0 S
# 
# and a segmentation to be tested, given in .vad file,
# Ex:
# 0.0    1.5 S
# 1.5    6.0 V
# 
# it computes, for each label in the test, the hit rate for each label.
#
# In this example:
# S hypothesis => 100% correct ([0,1.5])
# V hypothesis => 1.5 correct ([2.5, 4]) from 4.5 ([1.5, 6]): 33.33%

use strict;

# Read lab files (used to read .lab and .vad)
sub read_lab {
    my @data = ();
    my $filename = shift;
    my $t_old = 0;
    open(IN, $filename) or die "Error opening input file: $filename\n";
    while (<IN>) {
	chomp;
	my ($tbeg, $tend, $label);
	if (3 != (($tbeg, $tend, $label) = split)) {
	    print "Format error: $_\n";
	    die;
	}
	if ($tbeg != $t_old) {
	    print "Error: time discontinuity:\n",
	    "  t_old: $t_old\n",
	    "  new line: $_\n";
	}
	push @data, [$tbeg, $tend, $label];
	$t_old = $tend;
    }    
    return @data;
}

# Print two lines; used for debuggin purposes
#sub show_items {
#    my $i = shift;
#    my $j = shift;
#    print "a) $i->[2]\t[$i->[0], $i->[1]]\n";
#    print "b) $j->[2]\t[$j->[0], $j->[1]]\n";
#    print "\n";
#}

# Given two labels, return '1' if the intervals of both
# labels are not disjoint
sub match {
    my $i_vad = shift;
    my $i_ref = shift;
    return 1 if (($i_ref->[0] < $i_vad->[1]) &&
	($i_ref->[1] > $i_vad->[0]));
    
    return 0;
}

# Given two labels, return the common duration of
# both intervals.
sub match_duration {
    my $i_vad = shift;
    my $i_ref = shift;
    my $t_beg = ($i_vad->[0] > $i_ref->[0] ?  $i_vad->[0] : $i_ref->[0]);
    my $t_end = ($i_vad->[1] < $i_ref->[1] ?  $i_vad->[1] : $i_ref->[1]);
    return 0 if ($t_beg >= $t_end);
    return $t_end - $t_beg;
}

# Prints hit statistics
# Each label is stored in a hash table.
# The hash value of this label is a list with: hit_time and error_time
sub print_statistics {
    my $stat = shift;
    foreach my $k (sort keys %{$stat}) {
	my $label = $k;
	my $t_hit = $stat->{$k}->[0];
	my $t_err = $stat->{$k}->[1];
	my $t_tot = $t_hit + $t_err;
	
	printf "HIT %s.\t%.2fs/%.2fs.\t%.2f%%\n", 
	$label, $t_hit, $t_tot,
	100.0*$t_hit/$t_tot;
    }
    print "\n";
}

# Add the statistics table of one file to the global statistics.
sub acum_statistics{
    my $tot = shift;
    my $stat = shift;
    foreach my $k (sort keys %{$stat}) {
	my $t_hit = $stat->{$k}->[0];
	my $t_err = $stat->{$k}->[1];
	if (!defined($tot->{$k})) {
	      $tot->{$k} = [$t_hit, $t_err];
	  } else {
	      $tot->{$k}->[0] += $t_hit;
	      $tot->{$k}->[1] += $t_err;
	  }
    }
}

# Compare two label files (.lab, .vad) and compute hit statistics
# of each label present in the test (.vad).

sub compare_labs {
    my $fileref = shift;
    my $filevad = shift;

    my %stat = ();
    my @ref = read_lab($fileref) or die "Error reading .lab file: $fileref\n"; 
    my @vad = read_lab($filevad) or die "Error reading .vad file: $filevad\n"; 

    my $duration = ($ref[-1]->[1] > $vad[-1]->[1] ? $ref[-1]->[1] : $vad[-1]->[1]);
    if ($ref[-1]->[1] < $duration) {
	print "Warning: adding extra silence [", $ref[-1]->[1], ", ", $duration, "] at the end of ref: $fileref\n";
	push @ref, [$ref[-1]->[1], $duration, "S"];
    } elsif ($vad[-1]->[1] < $duration) {
	print "Warning: adding extra silence [",$vad[-1]->[1], ", ", $duration, "] at the end of ref: $filevad\n";
	push @vad, [$vad[-1]->[1], $duration, "S"];
    }

    my $iref = 0;

    foreach my $item (@vad) {
      # skip previous labels in the reference 
      while ($iref <= $#ref && !match($item, $ref[$iref])) {
	  $iref++;	
      }	
      last if $iref > $#ref;
      
      # Compute the match for all the ref. intervals that 
      # (partially) match the tested label

      while ($iref <= $#ref && match($item, $ref[$iref])) {
	  my $t = match_duration($item, $ref[$iref]);
	  my ($t_hit, $t_err);
	  if ($item->[2] eq $ref[$iref]->[2]) {
	      $t_hit = $t; $t_err = 0;
	  } else {
	      $t_hit = 0; $t_err = $t;
	  }
	  
#	print "MATCH $item->[2]\t$t_hit\t$t_err\n";
#	show_items($item, $ref[$iref]);	
	  
	  if (!defined($stat{$item->[2]})) {
	      $stat{$item->[2]} = [$t_hit, $t_err];
	  } else {
	      $stat{$item->[2]}->[0] += $t_hit;
	      $stat{$item->[2]}->[1] += $t_err;
	  }
	  
	  $iref++;
      } 
      --$iref;
  }
  return %stat;
}

# Main program ... loop through the .lab files and print results

if (@ARGV < 1) {
    print STDERR<<EOF;
 Usage: $0 file1.lab [file2.lab ....]
     For each .lab file (reference) a file with extension .vad has to present 
     (same name and directory)
EOF
    exit 1;
}

my %acum_statistics = ();
foreach my $filelab (@ARGV) {
    die "Reference file not found: $filelab\n" 
	unless -f $filelab;

    my $filevad;
    ($filevad = $filelab) =~ s/\.lab$/.vad/;

    die "VAD file not found: $filevad\n" 
	unless -f $filevad;

    print "**************** $filelab ****************\n";
    my %statistics = compare_labs($filelab, $filevad);
    print_statistics(\%statistics);
    acum_statistics(\%acum_statistics, \%statistics);
}

if (@ARGV > 1) {
    print "**************** Summary ****************\n";
    print_statistics(\%acum_statistics);
}

exit 0;
