 #!/bin/bash
 for testInput in *.pcap; do
    # Strip off the file extension, i.e., the ".in"
    #name=${testInput%.pcap}

    # Run the test
    ./trace < ArpTest.pcap > ArpTest.pcap.actual
    
    # diff the results
    diff -s ArpTest.pcap.actual ArpTest.pcap.out
 done
