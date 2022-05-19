 #!/bin/bash
 for testInput in *.pcap; do
    # Strip off the file extension, i.e., the ".in"
    #name=${testInput%.pcap}

    # Run the test
    ./trace < $testInput > $testInput.actual
    
    # diff the results
    diff -s $testInput.actual $testInput.out
 done
