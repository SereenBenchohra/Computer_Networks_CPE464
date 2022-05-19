#include "buffer.h"

void valid_arguments(int argc)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: trace [file]\n");
        exit(EXIT_FAILURE);
    }
    
}

void print_IP_bytes(u_char *pk_data, int byte_length )
{
    for (int i = 0; i < byte_length; i++)
    {
        if (i == (byte_length - 1)) // the last byte 
            printf("%d\n", pk_data[i]);
        else
            printf("%d.", pk_data[i]);

    }
}


void print_MAC_bytes(u_char *pk_data, int byte_length )
{
    for (int i = 0; i < byte_length; i++)
    {
        if (i == (byte_length - 1)) // the last byte 
            printf("%x\n", pk_data[i]);
        else
            printf("%x:", pk_data[i]);

    }
}

void print_TCP_flags(u_int8_t flag)
{
    int ACK_mask = 0x10; //high bit
    int SYN_mask = 0x02;
    int RST_mask = 0x04;
    int FIN_mask = 0x01;

    printf("\t\tACK Flag: ");
    if((flag & ACK_mask) == ACK_mask )
        printf("Yes\n");
    else if((flag & ACK_mask) != ACK_mask )
            printf("No\n");
    printf("\t\tSYN Flag: ");
    if ((flag & SYN_mask) == SYN_mask )
        printf("Yes\n");
    else if ((flag & SYN_mask) != SYN_mask )
        printf("No\n");
    printf("\t\tRST Flag: ");
    if ((flag & RST_mask) == RST_mask)
        printf("Yes\n");
    else if ((flag & RST_mask) != RST_mask)
        printf("No\n");
    printf("\t\tFIN Flag: ");
    if ((flag & FIN_mask) == FIN_mask)
        printf("Yes\n");
    else if ((flag & FIN_mask) != FIN_mask)
        printf("No\n");

}

void TCP_checksum(const u_char *pkt_data)
{
    uint8_t header_len, reserved = 0; // reserved 8 bit of zeros
    uint16_t PDU_len, TCP_len, TCP_net_len;
    u_char *Pseudo_head = NULL;

    memcpy(&header_len, pkt_data + IP_HEADER_LEN, 1);
    header_len = (header_len & 0x0F) * 4;
    memcpy(&PDU_len, pkt_data + TOTAL_PDU_LEN, sizeof(uint16_t) );
    TCP_len = ntohs(PDU_len) - header_len;
    //save TCP length
    TCP_net_len = htons(TCP_len); // save TCP len into network order

    uint16_t total_len = TCP_len + 12; // used for malloc and function itself
    Pseudo_head = malloc(total_len ); // allocates buffer for
    uint16_t checksum;
    memcpy(&checksum, pkt_data + 50, sizeof(uint16_t));
    
    // create the pseudo-header
    memcpy(Pseudo_head, pkt_data + SENDER_IP, 4); // copy sender IP in ther header
    memcpy(Pseudo_head + 4, pkt_data + DEST_IP, 4); // copy dest into the header
    memcpy(Pseudo_head + 8, &reserved, sizeof(u_int8_t)); // copies the reserved 0 bits in the header
    memcpy(Pseudo_head + 9, pkt_data + PROTOCOL, sizeof(u_char) ); // copies prot into header
    memcpy(Pseudo_head + 10 , &TCP_net_len, sizeof(uint16_t)); 
    memcpy(Pseudo_head + 12 , pkt_data + header_len + IP_HEADER_LEN, TCP_len);

    unsigned short res = in_cksum((unsigned short*)Pseudo_head, total_len);
    if (res == 0)
        printf("Correct (0x%x)\n", htons(checksum));
    else
        printf("Incorrect (0x%x)\n", htons(checksum) );

    free(Pseudo_head);

}


void print_TCP_header(const u_char *pkt_data)
{
    printf("\n\tTCP Header\n");
    u_int16_t pk_copy, data;
    
    memcpy(&pk_copy, pkt_data + SOURCE_PORT, sizeof(uint16_t)); // get Source port
    
    data = htons(pk_copy);

    if(data == 80)
        printf("\t\tSource Port:  HTTP\n");
    else
        printf("\t\tSource Port: : %u\n", data);

    memcpy(&pk_copy, pkt_data + DEST_PORT, sizeof(uint16_t)); // move over to get Dest port
    
    data = htons(pk_copy);

    if(data == 80)
        printf("\t\tDest Port:  HTTP\n");
    else
        printf("\t\tDest Port: : %u\n", data);

    uint32_t seq, res, ack_num;
    memcpy(&seq, pkt_data + SEQUENCE , sizeof(uint32_t)); // get 4 byte sequence 
    res = htonl(seq); // format it in proper network order(not needed I think )
    printf("\t\tSequence Number: %u\n",res);

    memcpy(&ack_num, pkt_data + ACK_NUM, sizeof(uint32_t));
    res = htonl(ack_num); // copy ACK number (prob issue here)

    int ACK_mask = 0x10; //high bit

    uint8_t flag;
    // skip over first byte, not needed in our calcuations
    memcpy(&flag, pkt_data + FLAGS , 1); // get last flag byte
    if((flag & ACK_mask) == ACK_mask )
        printf("\t\tACK Number: %u\n",res);
    else
        printf("\t\tACK Number: <not valid>\n");
    print_TCP_flags(flag);


    uint16_t wind;
    printf("\t\tWindow Size: ");
    memcpy(&wind, pkt_data + WINDOW, 2); // get Window number

    printf("%u\n", htons(wind));

    printf("\t\tChecksum: ");
    TCP_checksum(pkt_data);

}

void print_protocol_type(u_char protocol)
{
    if(protocol == (u_char)0x6)
        printf("TCP\n");
    else if(protocol == (u_char)0x01)
        printf("ICMP\n");
    else if(protocol == (u_char)0x11)
        printf("UDP\n");
    else
        printf("Unknown\n");

}

void print_ICMP_header(const u_char *pkt_data, int header_len)
{
    printf("\n\tICMP Header\n");
    printf("\t\tType: ");
    u_char pk_copy[1];
    memcpy(pk_copy, pkt_data + ICMP_TYPE, 1); // copy ICMP type
    
    if (header_len > 20)
       printf("109\n");
    else if (pk_copy[0] == (u_char)0x8)
        printf("Request\n");
    else
        printf("Reply\n");

}

void print_UDP_header(const u_char *pkt_data)
{
    printf("\n\tUDP Header\n");
    u_int16_t data;

    memcpy(&data, pkt_data + SOURCE_PORT, sizeof(uint16_t));
    printf("\t\tSource Port: : %u\n", htons(data));

    u_int16_t data1;
    memcpy(&data1, pkt_data + DEST_PORT, sizeof(u_int16_t));
    printf("\t\tDest Port: : %u\n", htons(data1));    
}


void print_protocol_header(u_char protocol, const u_char *pkt_data, int header_len)
{
    if(protocol == (u_char)0x6)
        print_TCP_header(pkt_data);
    else if(protocol == (u_char)0x01)
        print_ICMP_header(pkt_data, header_len);
    else if(protocol == (u_char)0x11)
        print_UDP_header(pkt_data);
}

void read_IP_header(const u_char *pkt_data)
{
    u_char pkt_copy[6];
    
    printf("\n\tIP Header\n");     // print IP header

    printf("\t\tHeader Len: ");
    memcpy(pkt_copy, pkt_data + IP_HEADER_LEN, 1); // copy over Header Length 
    // convert the hex value to the actual number of bytes 
    int head_len = (pkt_copy[0] & 0x0F) * 4;
    unsigned short res = in_cksum((unsigned short *)(pkt_data + IP_HEADER_LEN), head_len);

    printf("%d (bytes)\n", head_len);

    memcpy(pkt_copy, pkt_data + TOS, 1); // copy over TOS
    printf("\t\tTOS: 0x%x\n", pkt_copy[0]);
    
    memcpy(pkt_copy, pkt_data + TTL, 1); // copy over TTL
    printf("\t\tTTL: %d\n", pkt_copy[0]);

    uint16_t pdu_len;
    memcpy(&pdu_len, pkt_data + TOTAL_PDU_LEN , sizeof(uint16_t)); // get PDU len
    printf("\t\tIP PDU Len: %d (bytes)\n", htons(pdu_len));

    u_char protocol[1];
    memcpy(protocol, pkt_data + PROTOCOL, 1); // get Protocol
  
    printf("\t\tProtocol: "); // works 
    print_protocol_type(protocol[0]);

   
    uint16_t chksum; 
    memcpy(&chksum, pkt_data + CHECKSUM, 2);  // get the checksum value
    printf("\t\tChecksum: ");

    if (res == 0) // print correctness based on chksum function
        printf("Correct (0x%x)\n", chksum);
    else
        printf("Incorrect (0x%x)\n", chksum);
       
    memcpy(pkt_copy, pkt_data + SENDER_IP, 4); // get the Sender IP
    printf("\t\tSender IP: ");
    print_IP_bytes(pkt_copy, 4);

    memcpy(pkt_copy, pkt_data + DEST_IP, 4); // get the Dest IP
    printf("\t\tDest IP: ");
    print_IP_bytes(pkt_copy, 4);

    // print header type based on info 
    // determine which type of protocol is it (TCP), ICMP, UDP 
   print_protocol_header(protocol[0], pkt_data, head_len);

}

// print out ethernet header 
void read_ethernet_header(const u_char *pkt_data, u_char *type1)
{
    printf("\n");
    printf("\tEthernet Header\n");
    printf("\t\tDest MAC: ");
    u_char info_copy[6]; // info buffer to copy data into 

    memcpy(info_copy, pkt_data, 6);
    print_MAC_bytes(info_copy, 6);

   // move over to copy the Source address 
    printf("\t\tSource MAC: ");
    memcpy(info_copy, pkt_data + SOURCE_ADDRESS, 6);
    print_MAC_bytes(info_copy, 6);
    
   // move over to type
    memcpy(type1, pkt_data+ ETH_HEAD_TYPE , 2);
    printf("\t\tType: ");

    if (type1[1] == (u_char)0x0) // checks if its ARP or IP
        printf("IP");
    else
        printf("ARP"); 
    
    printf("\n");

} 

// Note : packet data increments from Ethernet Function resettted , since it is a single pointer 
void read_ARP_header(const u_char *pkt_data)
{
    // increment by 8 get opcode bytes 
    u_char opcode[2];
    u_char pkt_copy[6];
    memcpy(opcode, pkt_data + ARP_OPCODE , 2); 
    printf("\n\tARP header\n");
    printf("\t\tOpcode: ");

    if(opcode[1] == (u_char)0x1)
        printf("Request\n");    
    else
        printf("Reply\n");

    // pkt_data+=2; // increment over to Sender MAC address 
    printf("\t\tSender MAC: ");
    memcpy(pkt_copy, pkt_data + ARP_SENDER_MAC, 6); // 
    print_MAC_bytes(pkt_copy, 6);

    // increment over to print IP address (4 bytes in decimal )
    printf("\t\tSender IP: ");
    memcpy(pkt_copy, pkt_data + ARP_SENDER_IP, 4); // copy over Sender IP
    print_IP_bytes(pkt_copy, 4);

    // increment over to retrieve Target Mac Address
    printf("\t\tTarget MAC: ");
    memcpy(pkt_copy, pkt_data + ARP_TARGET_MAC, 6); // copy over Sender IP
    print_MAC_bytes(pkt_copy, 6);

     // increment over to print IP address (4 bytes in decimal )
    printf("\t\tTarget IP: ");
    memcpy(pkt_copy, pkt_data + ARP_TARGET_IP, 4); // copy over Sender IP
    print_IP_bytes(pkt_copy, 4);
    printf("\n");
}

int main(int argc, char *argv[])
{
    // check if arguments are valid 
    valid_arguments(argc);
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *p;
    p = pcap_open_offline(argv[1], errbuf);
    if (p == NULL)
    {
        printf("%s\n", errbuf);
        exit(EXIT_FAILURE);
    }
    
    struct pcap_pkthdr *header;
    const u_char *pkt_data;

    int n, packet_num = 1;
    
    while ((n =  pcap_next_ex(p, &header, &pkt_data)) >= 0)
    {
        if (n == 0)
            continue;
        
        printf("\nPacket number: %d  Frame Len: %d\n", packet_num, header->len);         // print out packet # (use a counter that is is set to 0 and header len 

        u_char type[2]; // save Ethernet type 
        read_ethernet_header(pkt_data, type);         // read Ethernet header 

        if (type[1] == (u_char)0x0) // checks if its ARP or IP for header reading
            read_IP_header(pkt_data);
        else
            read_ARP_header(pkt_data); 
        
        packet_num++;
        

   }
   
    if (n == -1)
    {
        printf("Error reading the packets\n");
        exit(EXIT_FAILURE);
    }    
    pcap_close(p);

    return 0;
}