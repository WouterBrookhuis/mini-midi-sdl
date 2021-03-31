#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "mini-midi/include/mini_midi.h"

static uint8_t fileBuffer[1024 * 1024];

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  FILE *midiFile = fopen("resources/il-vento-d-oro.mid", "r");
  if (midiFile == NULL)
  {
    printf("Failed to open file\n");
    return -1;
  }

  size_t bytesRead = fread(&fileBuffer[0], sizeof(uint8_t), sizeof(fileBuffer), midiFile);
  fclose(midiFile);

  printf("Read %lu bytes\n", bytesRead);

  mm_header_t midiHeader;
  if (!mm_read_header(&fileBuffer[0], bytesRead, &midiHeader))
  {
    printf("Failed to read header\n");
    return -2;
  }

  printf("Format: %u\n", midiHeader.format);
  printf("Tracks: %u\n", midiHeader.trackCount);
  printf("Time: 0x%04X\n", midiHeader.timeDivision);

  printf("Hello, MIDI!\n");
  return 0;
}
