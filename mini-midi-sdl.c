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

  size_t totalBytes = fread(&fileBuffer[0], sizeof(uint8_t), sizeof(fileBuffer), midiFile);
  fclose(midiFile);

  printf("Read %lu bytes\n", totalBytes);

  mm_header_t midiHeader;
  size_t offset = 0;
  size_t bytesRead = mm_read_header(&fileBuffer[0], totalBytes, &midiHeader);
  if (0 == bytesRead)
  {
    printf("Failed to read header\n");
    return -2;
  }

  offset += bytesRead;

  printf("Format: %u\n", midiHeader.format);
  printf("Tracks: %u\n", midiHeader.trackCount);
  printf("Time: 0x%04X\n", midiHeader.timeDivision);

  for (uint16_t track = 0; track < midiHeader.trackCount; track++)
  {
    uint32_t trackSize;
    bytesRead = mm_read_track_chunk(&fileBuffer[offset], totalBytes - offset, &trackSize);
    if (0 == bytesRead)
    {
      printf("Error reading track chunk %u\n", track);
      return -3;
    }

    printf("Track %u at offset %lu is %u bytes long\n", track, offset, trackSize);

    offset += bytesRead;

    while (trackSize > 0)
    {
      mm_event_t event;
      bytesRead = mm_read_track_event(&fileBuffer[offset], totalBytes - offset, &event);
      if (0 == bytesRead)
      {
        printf("Error reading event in track %u (file offset %lu)\n", track, offset);
        for (size_t i = 0; i < 16; i++)
          printf("%02X ", fileBuffer[offset + i]);
        printf("\n");
        return -3;
      }

      printf("Track event %X on channel %u\n", event.type, event.note.channel);

      if (bytesRead > trackSize)
      {
        printf("Read event outside of track %u (file offset %lu)\n", track, offset);
        return -4;
      }

      offset += bytesRead;
      trackSize -= bytesRead;
    }
  }

  printf("Hello, MIDI!\n");
  return 0;
}
