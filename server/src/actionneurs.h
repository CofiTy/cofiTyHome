//------------------ACTIONNEUR--------------
struct trame{ 
        char* SYNC;//SYNC = "A55A";
        char* HEADER;// (TX MESSAGE): 3
        char* LENGHT;//LENGHT: B
        char* ORG;//ORG : 05
        char* DATA;//DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
//DATA : DB3: 0110 0000, le reste a zero => DATA: 60000000 B0
        char* ID;//ID= FF9F1E05
        char* STATUS;//STATUS : doc
        char* CHECKSUM;//CHECKSUM:least significant byte from addition of all bytes except for sync and checksum
};

void OpenCourrant(char id[8]);
void CloseCourrant(char id[8]);