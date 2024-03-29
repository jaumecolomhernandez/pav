#!/bin/bash

# Scripting is very useful to repeat tasks, as testing different configuration, multiple files, etc.
# This bash script is provided as one example
# Please, adapt at your convinience, add cmds, etc.
# Antonio Bonafonte, Nov. 2015

w="/home/pav113/pav/practica4/temp"
db="/home/pav113/pav/practica4/speecon"

#EXPORT TO PATH THE COMPILED FILES FOLDER

# ------------------------
# Usage
# ------------------------

if [[ $# < 1 ]]; then
   echo "$0 cmd1 [...]"
   echo "Where commands can be:"
   echo  "    lists: create, for each spk, training and devel. list of files"
   echo  "      mcp: feature extraction (mel cepstrum parameters)"
   echo  " trainmcp: train gmm for the mcp features"
   echo  "  testmcp: test GMM using only mcp features"
   echo  " classerr: count errors of testmcp"
   echo  "finaltest: reserved for final test"
   echo  "listverif: create, list for verification -- files, candidates, ...."
   echo "trainworld: estimate world model"
   echo "    verify: test gmm in verification task"
   echo " verifyerr: count errors of verify"
   echo "       roc: print error as a function of thr, so that roc can be plot"
   exit 1
fi


# ------------------------
# Check directories
# ------------------------

if [[ -z "$w" ]]; then echo "Edit this script and set variable 'w'"; exit 1; fi
mkdir -p $w  #Create directory if it does not exists
if [[ $? -ne 0 ]]; then echo "Error creating directory $w"; exit 1; fi

if [[ ! -d "$db" ]]; then
   echo "Edit this script and set variable 'db' to speecon db"
   exit 1
fi
	

# ------------------------
# Check if gmm_train is in path
# ------------------------
which gmm_train > /dev/null
if [[ $? != 0 ]] ; then
   echo "Set PATH to include PAV executable programs ... "
   echo "Maybe ... source pavrc ? or modify bashrc ..."
   #echo "export PATH=$PATH:/WHATEVER/" 
   #Exportem al script on cridem el run_spkid (classification_script.sh)
   exit 1
fi 
# Now, we assume that all the path for programs are already in the path 

CMDS="lists extract trainmcp testmcp classerr finaltest listverif train_world verify verifyerr roc"


# ------------------------
# Auxiliar functions
# ------------------------

# cmd lists: foreach spk, create train and test list.
create_lists() {
    \rm -fR $w/lists
    mkdir -p $w/lists
    for dir in $db/BLOCK*/SES* ; do
	name=${dir/*\/}
	echo Create list for speaker $dir $name ----
	(find -L $db/BLOCK*/$name -name "*.wav" | perl -pe 's/^.*BLOCK/BLOCK/; s/\.wav$//' | sort | unsort > $name.list) || exit 1
	# split in test list (5 files) and train list (other files)
	(head -5 $name.list | sort > $w/lists/$name.test) || exit 1
	(tail -n +6 $name.list | sort > $w/lists/$name.train) || exit 1
	\rm -f $name.list
    done
    cat $w/lists/*.train | sort > $w/lists/all.train
    cat $w/lists/*.test | sort > $w/lists/all.test
}


# command mcp: create feature from wave files
# Make you best choice or try several options

compute_features() {
    # PARAMETRES NOMBRE FEATURES I NOMBRE DE TRANSFORMADORS (NOMÉS EN EL CAS DEL WAV2LPCC)
    model=lpc
    case $model in 
        lpc)
          for line in $(cat $w/lists/all.train) $(cat $w/lists/all.test); do
            mkdir -p `dirname $w/mcp/$line.mcp`
            echo "EXTRACCIO LPCC $db/$line.wav" "$w/mcp/$line.mcp"
            wav2lpcc 4 8 "$db/$line.wav" "$w/mcp/$line.mcp" || exit 1
          done
          ;;
        mfcc)
          for line in $(cat $w/lists/all.train) $(cat $w/lists/all.test); do
            mkdir -p `dirname $w/mcp/$line.mcp`
            echo "Extracció MFCC $db/$line.wav" "$w/mcp/$line.mcp"
            wav2mfcc 12 "$db/$line.wav" "$w/mcp/$line.mcp" || exit 1
          done
          ;;
    esac
}


# command listverify
# Create list for users, candidates, impostors, etc.

create_lists_verif() {
    dirlv=$w/lists_verif
    mkdir -p $dirlv 

    # find all the speakers names
    find  -L $db -type d -name 'SES*' -printf '%P\n'|\
           perl -pe 's|BLOCK../||' | sort > $dirlv/all.txt

    unsort $dirlv/all.txt > all_spk 

    # split speakers into: users (50), impostors (50) and the others
    (head -50 all_spk | sort > $dirlv/users.txt) || exit 1
    (tail -n +51 all_spk  | head -50 | sort > $dirlv/impostors.txt) || exit 1
    (tail -n +101 all_spk | sort > $dirlv/others.txt) || exit 1
    \rm -f all_spk

    # create trainning/test data from legitime users
    \rm -f $dirlv/users.train; touch $dirlv/users.train
    \rm -f $dirlv/users.test; touch $dirlv/users.test
    for spk in `cat $dirlv/users.txt`; do
	cat $w/lists/$spk.train >> $dirlv/users.train
	cat $w/lists/$spk.test >> $dirlv/users.test
    done

    # create trainning data from 'other' speakers
    \rm -f $dirlv/others.train; touch $dirlv/others.train
    for spk in `cat $dirlv/others.txt`; do
	cat $w/lists/$spk.train >> $dirlv/others.train
    done

    # Join users and others training, just in case you want to use it both
    cat $dirlv/users.train $dirlv/others.train > $dirlv/users_and_others.train

    # create test data from 'impostors' speakers
    \rm -f $dirlv/impostors.test; touch $dirlv/impostors.test
    for spk in `cat $dirlv/impostors.txt`; do
	cat $w/lists/$spk.test >> $dirlv/impostors.test
    done

    # Create 4 claims for each file:
    cat $dirlv/impostors.test $dirlv/impostors.test \
        $dirlv/impostors.test $dirlv/impostors.test |\
        sort > $dirlv/impostors4.test

    # Create candidate claims:


    # From legitime users:
    cat $dirlv/users.test | perl -ne 'chomp; print "$1\n" if m|(SES\d+)|' > $dirlv/users.test.candidates

    # From impostors: (claim a random legitime user)
    perl -ne '{
       BEGIN {
          open("USERS", "$ARGV[0]") or die "Error opening user file: $ARGV[0]\n"; 
          @users = <USERS>;
          $nusers = int(@users);
          die "Error: nUsers == 0 in file $ARGV[0]\n" if $nusers == 0;
          shift;
          srand(102001);
       }
       $v = int(rand($nusers));
       print $users[$v];
    }' $dirlv/users.txt < $dirlv/impostors4.test > $dirlv/impostors4.test.candidates


    # Join all the test
    cat $dirlv/users.test $dirlv/impostors4.test  > $dirlv/all.test 
    cat $dirlv/users.test.candidates $dirlv/impostors4.test.candidates  > $dirlv/all.test.candidates 

    echo "Train lists:"
    wc -l $dirlv/*.train | grep -v total; echo

    echo "Test lists"
    wc -l $dirlv/*.test | grep -v total; echo
    wc -l $dirlv/*.test.candidates | grep -v total

}

# ---------------------------------
# Main program: 
# For each cmd in command line ...
# ---------------------------------


for cmd in $*; do
   echo `date`: $cmd '---';

   if [[ $cmd == lists ]]; then
      #Crea les llistes d'usuaris de train i de testing
      create_lists
   elif [[ $cmd == extract ]]; then
      #Extrau les features. La configuració está dins de la funció
      #PARAMETRES: MIRAR LA FUNCIÓ
      compute_features
   elif [[ $cmd == trainmcp ]]; then
      #Calcula les gmm òptimes per a cada parlant
      #PARAMETRES: TOTES LES FLAGS DEL GMM_TRAIN
      for dir in $db/BLOCK*/SES* ; do
	  name=${dir/*\/}
	  echo $name ----
	  # gmm_train  -v 1 -T 0.01 -N5 -m 1 -d $w/mcp -e mcp -g $w/gmm/mcp/$name.gmm $w/lists/$name.train || exit 1  #VERSIO SENSE OPCIONS
      gmm_train -d $w/mcp -e mcp -g $w/gmm/mcp/$name.gmm $w/lists/$name.train || exit 1
          echo
      done
   elif [[ $cmd == testmcp ]]; then
      #Classifica cada audio amb les gmm i tria la que té més probabilitat
       find $w/gmm/mcp -name '*.gmm' -printf '%P\n' | perl -pe 's/.gmm$//' | sort  > $w/lists/gmm.list
       (gmm_classify -d $w/mcp -e mcp -D $w/gmm/mcp -E gmm $w/lists/gmm.list  $w/lists/all.test | tee $w/spk_classification.log) || exit 1

   elif [[ $cmd == classerr ]]; then
      #Mira si la que s'ha triat a testmcp és el correcte i dona el resultat
       if [[ ! -s $w/spk_classification.log ]] ; then
          echo "ERROR: $w/spk_classification.log not created"
          exit 1
       fi
       # Count errors
       perl -ne 'BEGIN {$ok=0; $err=0}
                 next unless /^.*SA(...).*SES(...).*$/; 
                 if ($1 == $2) {$ok++}
                 else {$err++}
                 END {printf "nerr=%d\tntot=%d\terror_rate=%.2f%%\n", ($err, $ok+$err, 100*$err/($ok+$err))}' $w/spk_classification.log
   elif [[ $cmd == finaltest ]]; then
        #What is finaltest?
       echo "To be implemented ..."
   elif [[ $cmd == listverif ]]; then
    #Crea les llistes de verificació
      create_lists_verif
   elif [[ $cmd == train_world ]]; then
       #Crea la gmm del món, utilitza la llista ./lists/all.train
       #PARAMETRES: TOTES LES FLAGS DE GMM_TRAIN I TAMBÉ LES DADES QUE FEM SERVIR (user.train, other.train, users_and_others.train)
       gmm_train -d $w/mcp -e mcp -m 12 -N 20 -g $w/gmm/mcp/world.gmm $w/lists_verif/users_and_others.train || exit 1
   elif [[ $cmd == verify ]]; then
       #Realitza la verificació dels resultats 
       find $w/gmm/mcp -name '*.gmm' -printf '%P\n' | perl -pe 's/.gmm$//' | sort  > $w/lists_verif/gmm.list
       (gmm_verify -d $w/mcp -e mcp -D $w/gmm/mcp -E gmm $w/lists_verif/gmm.list  $w/lists_verif/all.test $w/lists_verif/all.test.candidates | tee $w/spk_verify.log) || exit 1
    elif [[ $cmd == verify_with_world ]]; then
       #Realitza la verificació dels resultats utilitzant el world
       find $w/gmm/mcp -name '*.gmm' -printf '%P\n' | perl -pe 's/.gmm$//' | sort  > $w/lists_verif/gmm.list
       (gmm_verify -d $w/mcp -e mcp -D $w/gmm/mcp -E gmm -w world $w/lists_verif/gmm.list  $w/lists_verif/all.test $w/lists_verif/all.test.candidates | tee $w/spk_verify.log) || exit 1
   elif [[ $cmd == verif_err ]]; then
      #Prova diversos tresholds per a trobar el òptim en funció de un cost d'error de 99 vegades
       if [[ ! -s $w/spk_verify.log ]] ; then
          echo "ERROR: $w/spk_verify.log not created"
          exit 1
       fi
       spk_verif_score $w/spk_verify.log | tee $w/spk_verify.res
   elif [[ $cmd == roc ]]; then
       # Change threshold and compute table prob false alarm vs. prob. detection 
       spk_verif_score $w/spk_verify.log | tee $w/spk_verify.res
       perl -ne '
         next if ! /^THR\t/; 
         chomp;
         @F=split/\t/;
         ($prob_miss = $F[3]) =~ s/\(.*//;  
         $prob_detect = 1 - $prob_miss;
         ($prob_fa = $F[4]) =~ s/\(.*//; 
         print "$prob_fa\t$prob_detect\n"'  $w/spk_verify.res > $w/spk_verify.roc

   else
       echo "undefined command $cmd" && exit 1
   fi
done

exit 0
