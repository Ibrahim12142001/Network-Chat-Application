# Network Chat Application – Application (C, Sockets, UDP)
• Developed a multi-threaded network chat application in C, enabling real-time message exchange between terminals.
• Implemented socket programming and UDP protocols to facilitate communication across remote hosts.
• Utilized semaphores and condition variables to manage concurrency, ensuring synchronized access to shared resource

## How to Use 

1. Download the file and open a terminal
2. Specifiy the posrt number and remote machine name that you will use to chat
3. Specify this information on the command line and run the code with the following format: s-talk [my port number] [remote machine name] [remote port number]

## Example 
Say that Fred and Barney want to talk. Fred is on machine "csil-cpu1" and will use port
number 6060. Barney is on machine "csil-cpu3" and will use port number 6001.
To initiate s-talk, Fred must type:
s-talk 6060 csil-cpu3 6001
And Barney must type:
s-talk 6001 csil-cpu1 6060.

