# System Programming 
 *Προγραμματισμός Συστήματος  
 Εαρινό Εξάμηνο 2019*  
 
 Νιάρχος Γεώργιος 

# [Project 1](https://github.com/GeorgeNiarkhos/System-Programming/blob/master/project1/hw1-spring-2019.pdf)
Ενα πρόγραμμα το οποίο δέχεται, επεξεργάζεται, καταγράφει και απαντάει ερωτήματα για Bitcoin συναλλαγές. Χρησιμοποιεί ένα σύνολο
δομών (hash tables, linked lists, trees) που επιτρέπουν την εισαγωγή σε επερωτήσεις με μεγάλο όγκο εγραφών τύπου BitcoinTransaction. Τα δεδομένα της άσκησης προέρχονται από αρχεία, αλλά όλες οι εγγραφές αποθηκεύονται στην κύρια μνήμη.

# [Project 2](https://github.com/GeorgeNiarkhos/System-Programming/blob/master/project2/hw2-spring-2019.pdf)
Μία απλουστευμένη έκδοση του dropbox όπου διαφορετικές διεργασίες συγχρονίζουν ένα σύνολο από αρχεία. Οι διεργασίες λειτουργούν όλες με τον ίδιο τρόπο και ο στόχος τους είναι να επικοινωνήσουν και να συγχρονιστούν μεταξύ τους έτσι ώστε να έχουν όλες από ένα κοινό αντίγραφο από τα αρχεία. Η δημιουργία των διεργασιών γίνεται μέσω των system calls fork/execm και η επικοινωνία τους μέσω pipes (low-level I/O). Για την δημιουργία dummy input files και για την ανάλυση των logFiles έχουν υλοποιηθεί και τα αντίστοιχα bash scripts.

# [Project 3](https://github.com/GeorgeNiarkhos/System-Programming/blob/master/project3/hw3-spring-2019.pdf)
Μία απλουστευμένη έκδοση του dropbox όπου διαφορετικοί clients συγχρονίζουν ένα σύνολο από αρχεία. Ο κάθε client συνδέεται σε ένα dropbox server, μαθαίνει ποιοι άλλοι clients έχουν σύνδεθεί καθώς επίσης και πληροφορίες για το πώς να επικοινώνεί μαζί τους. Κάθε client συνδέεται με κάθε έναν από τους άλλους clients με στόχο να συγχρονίσουν τα αρχεία τους, έτσι ώστε τελικά όλοι να έχουν ενημερωμένα αρχεία. Διαδικασία της μεταφόρας των αρχείων μεταξύ clients γίνεται με τη χρήση threads (pthread) και η επικοινωνία είναι δικτυακή, ακολουθώντας το πρότυπο Server - Clients. 

# Οδηγίες εκτέλεσης
Υπάρχει Makefile για την μεταγλώτιση και εκτέλεση του προγράμματος.

-Project 1
```sh
$ make
$ make run
```
-Project 2
```sh
$ make
$ bash create_infiles.sh 1_input 10 20 2
$ bash create_infiles.sh 2_input 10 20 2
$ make run1
$ make run2
```
-Project 3
```sh
$ make
$ make runs
$ make runc
$ ssh ...
$ make runc
```
Υπάρχει επίσης label για καθαρισμό αντικείμενων αρχείων, και του εκτελέσιμου
```sh
$ make clean
```
Για περισσότερες λειτουργίες αναζητηστε στα Makefile αρχεία των projects
[project1](https://github.com/GeorgeNiarkhos/System-Programming/blob/master/project1/Makefile) [project2](https://github.com/GeorgeNiarkhos/System-Programming/blob/master/project2/Makefile) [project3](https://github.com/GeorgeNiarkhos/System-Programming/blob/master/project3/Makefile)
