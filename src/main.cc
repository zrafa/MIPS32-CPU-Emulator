#include <fstream>
#include <sstream>

#include <string.h>
#include "cpu.h"

uint32_t atoh(char **argv) {
  int res, pos = 0;
  unsigned int byte;
  uint32_t entry=0;
  printf( "Scanning %s", argv[1] );
  while ( (res=sscanf( &argv[1][pos], "%2x", &byte )) == 1 ) {
	entry = entry << 8;
	entry = entry | byte;
    	pos += 2;
  }
  return entry;
}

int main(int argc, char **argv) {
    using namespace std;
    
  	uint32_t entry;
	/* entry point must be argv[1] */
	if ((argc<2) || (strlen(argv[1]) != 8)) {
		printf("\nERROR: entry point must be argv[1] and 8 ascii of len (example: 8A00B001) \n\n");
		exit(1);
	}

	entry = atoh(argv);
	printf( "kernel_entry=%X \n", entry);

    ifstream rom("OS_image/rom");
    ifstream flash("OS_image/disk0");

    // copy files to memory to speed up access
    stringstream rom_buffer;
    stringstream flash_buffer;
    copy(istreambuf_iterator<char>(rom), istreambuf_iterator<char>(), 
         ostreambuf_iterator<char>(rom_buffer));
    copy(istreambuf_iterator<char>(flash), istreambuf_iterator<char>(), 
         ostreambuf_iterator<char>(flash_buffer));

    CPU cpu(rom_buffer, flash_buffer, entry);

    cpu.run();

}

