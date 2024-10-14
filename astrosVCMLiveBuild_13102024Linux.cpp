/*
 
---------------------------------------------------------
     /\    / ____|__   __|  __ \ / __ \ / ____|
    /  \  | (___    | |  | |__) | |  | | (___  
   / /\ \  \___ \   | |  |  _  /| |  | |\___ \ 
  / ____ \ ____) |  | |  | | \ \| |__| |____) |
 /_/    \_\_____/   |_|  |_|  \_\\____/|_____/ 
Raspberry Pi 4 Model B                                             
Jason Deatrick
October 13, 2024
Version: A.1.0VCM
---------------------------------------------------------

*/
// C++ Libraries
#include <iostream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <string.h>

// Linux Libraries
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

// Variables
int runStatus = 0;
int alwaysLoop = 1;
int dialogueSolderLoop = 0;
int cableSolder = 0;
int braccioBootComplete = 0;
int serial_port = open("/dev/ttyS0", O_RDWR);

int main() {
    // Serial Setup START
    if (serial_port < 0) {
		printf("Error %i from open: %s\n", errno, strerror(errno));
	}
    struct termios tty;
	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);
    if (tcgetattr(serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	} else {
            std::cout << "Serial Communication Established" << std::endl;
        }
    // Serial Setup END
    
    do {
        runStatus = 0;
        dialogueSolderLoop = 0;

        // dialogueInitial Keyword database START
        std::string dialogueInitial;
        std::string keywordHome = "home"; // runStatus = 1
        std::string keywordTake = "take"; // runStatus = 2
        std::string keywordCancel = "thank"; // runStatus = 3
        std::string keywordStatus = "status"; // runStatus = 4
        std::string keywordSolder = "solder"; // runStatus = 5
        // dialogueInitial Keyword database END

        // dialogueSolder Keyword database START
        std::string dialogueSolder;
        std::string keywordSolderEject = "eject"; // cableSolder = 1
        std::string keywordSolderCore = "core"; // cableSolder = 2
        std::string keywordSolderEOA = "eoa"; // cableSolder = 3
        std::string keywordSolderSTAR = "star"; // cableSolder = 4
        std::string keywordSolderL4 = "l4"; // cableSolder = 4
        std::string keywordSolderCancel = "cancel"; // cableSolder = 5
        // dialogueSolder Keyword database END

        std::cout << "What would you like me to do?" << std::endl;
        std::cout << "Prompt: ";
        std::getline(std::cin, dialogueInitial);
        std::transform(dialogueInitial.begin(), dialogueInitial.end(), dialogueInitial.begin(), ::tolower);

        // Check if any keyword exists in dialogueInitial
        if (dialogueInitial.find(keywordHome) != std::string::npos) {
            std::cout << "Running Home Program..." << std::endl;
            runStatus = 1;
            unsigned char homeSerial[] = { '0', '\r' }; // Write 0x30 to Serial
            write(serial_port, homeSerial, sizeof(homeSerial));
        } 
        if (dialogueInitial.find(keywordTake) != std::string::npos) {
            std::cout << "Running Take Program..." << std::endl;
            runStatus = 2;
            unsigned char takeSerial[] = { '1', '\r' }; // Write 0x31 to Serial
            write(serial_port, takeSerial, sizeof(takeSerial));
        }
        if (dialogueInitial.find(keywordCancel) != std::string::npos) {
            std::cout << "Taking Object back to Home position..." << std::endl;
            runStatus = 3;
            unsigned char cancelSerial[] = { '2', '\r' }; // Write 0x32 to Serial
            write(serial_port, cancelSerial, sizeof(cancelSerial));
        }
        if (dialogueInitial.find(keywordStatus) != std::string::npos) {
            std::cout << "Requesting Status Update..." << std::endl;
            runStatus = 4;
            unsigned char statusSerial[] = { '3', '\r' }; // Write 0x33 to Serial
            write(serial_port, statusSerial, sizeof(statusSerial));
        }
        if (dialogueInitial.find(keywordSolder) != std::string::npos) {
            dialogueSolderLoop = 1;
            unsigned char solderSerial[] = { '4', '\r' }; // Write 0x34 to Serial
            write(serial_port, solderSerial, sizeof(solderSerial));
            do {
                cableSolder = 0;
                
                std::cout << "What cable would you like to solder?" << std::endl;
                std::cout << "Ejector Confirmation" << std::endl;
                std::cout << "Core Confirmation" << std::endl;
                std::cout << "EOAT Product Confirmation" << std::endl;
                std::cout << "STAR L4 Confirmation" << std::endl;
                runStatus = 5;
                std::cout << "Prompt: ";
                std::getline(std::cin, dialogueSolder);
                std::transform(dialogueSolder.begin(), dialogueSolder.end(), dialogueSolder.begin(), ::tolower);

                    // Check if any keyword exists in dialogueSolder
                    if (dialogueSolder.find(keywordSolderEject) != std::string::npos) {
                        std::cout << "Soldering Ejector Cable..." << std::endl;
                        cableSolder = 1;
                        dialogueSolderLoop = 0;
                    }
                    if (dialogueSolder.find(keywordSolderCore) != std::string::npos) {
                        std::cout << "Soldering Core Cable..." << std::endl;
                        cableSolder = 2;
                        dialogueSolderLoop = 0;
                    }
                    if (dialogueSolder.find(keywordSolderEOA) != std::string::npos) {
                        std::cout << "Soldering EOAT Cable..." << std::endl;
                        cableSolder = 3;
                        dialogueSolderLoop = 0;
                    }
                    if (dialogueSolder.find(keywordSolderSTAR) != std::string::npos) {
                        std::cout << "Soldering STAR L4 Cable..." << std::endl;
                        cableSolder = 4;
                        dialogueSolderLoop = 0;
                    }
                    if (dialogueSolder.find(keywordSolderL4) != std::string::npos) {
                        std::cout << "Soldering STAR L4 Cable..." << std::endl;
                        cableSolder = 4;
                        dialogueSolderLoop = 0;
                    }
                    if (dialogueSolder.find(keywordSolderCancel) != std::string::npos) {
                        std::cout << "Cancelled Solder Program" << std::endl;
                        cableSolder = 5;
                        dialogueSolderLoop = 0;
                    }
                    if (dialogueSolderLoop == 1) {
                        std::cout << "Solder Prompt Error!" << std::endl;
                    }
            } while(dialogueSolderLoop == 1);
        } 
        if (runStatus == 0) {
            std::cout << "Prompt Error!" << std::endl;
        }
    } while(alwaysLoop == 1);
    return 0;
}
