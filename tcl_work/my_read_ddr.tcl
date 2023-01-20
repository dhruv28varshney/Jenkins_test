#set systemTime [clock seconds]
#puts "The time is: [clock format $systemTime -format %H:%M:%S]"

connect
ta 5
#number of partitions- i/p to the script
set num_part [lindex $argv 0]
#length of 1 partition = 8 GB in bytes / num_part
set part_len [expr {8589934592 / $num_part}]
#The value to stop logging the data
set end_char 0xDEADFACE
#read count = partition length in bytes / 4 (size of 1 read = 32 bit = 4 bytes)
set read_count [expr {$part_len / 4}]



 for {set j 0} { $j < $num_part} { incr j} {
 set base_Address 5497558138880 
 set logfile [open "Part${j}_log.txt" "w"]
 set temp [expr $j * part_len]
 incr base_Address temp
 
  for {set i 0} {$i < $read_count} {incr i } {
   set ret [mrd $base_Address 1]
   set read_data [regsub {(.+):   (........)(.+)}  $ret {\2}]
   if { $read_data == $end_char } {
     break 
   } else { 
           puts $logfile [mrd $base_Address 1]
           incr base_Address 4
   }
  }
  close $logfile
 }
