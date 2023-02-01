/*
 * Secure Hash Algorithm.
@author Nicolas Steger
@since 5/5/21
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

//circle left shift
#define CIRCLE_LEFT_SHIFT(n, x) (((x) << n) | ((x) >> (32-n)))
//file size maximum
#define MAX 1038576

//read file into buffer
unsigned int readFile(unsigned char buffer[]) {
    int i;
    char tempChar = 0;
    while((tempChar = getchar()) != EOF) {
        buffer[i] = tempChar;
        i++;
        if(i > MAX) {
            cout <<"File is too large. \n";
        }
    }
    buffer[i+1] = 0x80;
    return i;
}

//returns the size of the
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes) {
    unsigned int blockCount = (((8 * sizeOfFileInBytes) + 1) / 512) + 1;

    if((((8 * sizeOfFileInBytes) + 1) % 512) > (512 - 64)) {
        blockCount = blockCount + 1;
    }
    return blockCount;
}

//append a buffer to the end of char to array
unsigned int append(int index) {
    unsigned int result = 1;
    if (index == 1) {
        result = (0x80 << 16);
    }
    else if (index == 2){
        result = (0x80 << 8);
    }
    else if (index == 3) {
        result = 0x80;
    }
    else if (index == 0) {
        result = (0x80 << 24);
    }
    return result;
}
//char packer
unsigned int charPacker(int index, unsigned char c) {
    unsigned char tempChar = c;
    unsigned int tempInt;
    if (index == 1) {
        tempInt = (tempChar << 24);
        return tempInt;
    }
    else if (index == 2){
        tempInt = (tempChar << 16);
        return tempInt;
    }
    else if (index == 3) {
        tempInt = (tempChar << 8);
        return tempInt;
    }
}
//converts char to int array
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes) {
    int counter = 1;
    int loop = 0;
    int j = 0;
    unsigned char tempChar;
    unsigned int tempInt = 0;
    //loops through array
    while (loop < sizeOfFileInBytes) {
        counter = counter % 4;
        //every fourth char creates new message in array
        if (counter == 0) {
            tempChar = buffer[loop];
            tempInt = tempInt + tempChar;
            message[j] = tempInt;
            counter++;
            loop++;
            tempInt = 0;
            j++;
            //for first 3 loops of char packs into single string
        } else {
            tempChar = buffer[loop];
            tempInt = tempInt + charPacker(counter, tempChar);
            message[j] = tempInt;
            counter++;
            loop++;
        }
    }
        //ads the extra 1 at the end
        if (loop == sizeOfFileInBytes) {
            int index = sizeOfFileInBytes % 4;
            tempInt = tempInt + append(index);
            message[j] = tempInt;
        }


}
//add bit count to last block
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount) {
   unsigned int sizeOfFileInBits = sizeOfFileInBytes * 8;
   unsigned int indexOfEndOfLastBlock = (blockCount * 16) - 1;
   message[indexOfEndOfLastBlock] += sizeOfFileInBits;
}
//f helper function for message digest
unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D) {
    unsigned int tempNum;
    if (t == 1) {
        tempNum = ((B & C) | ((~B) & D));
        return tempNum;
    }
    if (t == 2) {
        tempNum = (B ^ C ^ D);
        return tempNum;
    }
    if (t == 3) {
        tempNum = ((B & C) | (B & D) | (C & D));
        return tempNum;
    }
    return tempNum;
} //end f
//constants used for message digest
unsigned int K(unsigned int t) {
    unsigned int K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
    return K[t];
}
//message digest.
void computeMessageDigest(unsigned int message[], unsigned int blockCount) {
    unsigned int H[5];
    unsigned int t;
    unsigned int temp;
    unsigned int tempNum;
    unsigned int tempK;
    unsigned int A, B, C, D, E;
    unsigned int W[80];
    unsigned int totalPass = 0;
    int count = 0;
    int i = 0;
    int j = 0;
    int index = 0;

    H[0] = 0x67452301;
    H[1] = 0xEFCDAB89;
    H[2] = 0x98BADCFE;
    H[3] = 0x10325476;
    H[4] = 0xC3D2E1F0;

    while (totalPass < blockCount) {
        //part a divide 16 words
        for (i = 0; i < 16; i++) {
            W[i] = message[j];
            j++;
        }
        //part b
        for (t = 16; t <= 79; t++) {
            W[t] = (W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
            W[t] = CIRCLE_LEFT_SHIFT(1, W[t]);
        }
        //part c
        A = H[0];
        B = H[1];
        C = H[2];
        D = H[3];
        E = H[4];
        if (totalPass == 0 ){
            cout << "The initial hex values of {H_i} are" <<endl;
            cout << uppercase << hex << A << " " << B << " " << C << " " << D << " " << E << endl;
        }
        //part d
        for (int t = 0; t <= 79; t++) {
            if (0 <= t && t <=19) {
                tempNum = f(1, B, C, D);
                index = 0;

            }
            else if (20 <= t && t <=39) {
                tempNum = f(2, B, C, D);
                index = 1;

            }
            else if (40 <= t && t <=59) {
                tempNum = f(3, B, C, D);
                index = 2;
            }
            else if (60 <= t && t <=79) {
                tempNum = f(2, B, C, D);
                index = 3;

            }
            tempK = K(index);
            temp = (CIRCLE_LEFT_SHIFT(5, A) + tempNum + E + W[t] + tempK);
            E = D;
            D = C;
            C = CIRCLE_LEFT_SHIFT(30, B);
            B = A;
            A = temp;
        }
            //part e
            H[0] += A;
            H[1] += B;
            H[2] += C;
            H[3] += D;
            H[4] += E;
        totalPass++;
        } //end block loop
    count = 0;
    cout << "Message Digest: " << endl;
    while (count < 5) {
        cout << uppercase << hex << H[count] << " ";
        count++;
    }
    cout << endl;
    } //end function



int main () {
    int messageSize = 0;
    int count = 0;
    unsigned int blockCount;
    unsigned char buffer[MAX];

    count = readFile(buffer);
    blockCount = calculateBlocks(count);
    messageSize = blockCount * 16;

    unsigned int message[messageSize];
    fill(message, message + messageSize, 0);

    convertCharArrayToIntArray(buffer, message, count);
    addBitCountToLastBlock(message, count, blockCount);
    computeMessageDigest(message, blockCount);

    //cout << "Count = " << count << endl;
    //cout << "Block count = " << blockCount << endl;
    //cout << "buffer = ";
    //for (int i = 0; i < count; i++) {
    //        cout << buffer[i];
    //}
    cout << endl;
    return 0;
}

