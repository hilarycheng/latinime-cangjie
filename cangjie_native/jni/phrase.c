#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include "sqlite3.h"

#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
// #include "phrase_internal.h"

int phrase_count = 0;
int phrase_index = 0;
int phrase_max   = 0;
int phrase_saved = 0;
// int phrase_map[32768];
char phrase_path[1024];
// int phrase_freq[sizeof(phrase) / sizeof(struct PHRASE_INDEX)];

jchar phrase_last_key = 0;
jchar phrase_word[256][10];
int   phrase_length[256];
int   phrase_frequency[256];
int   phrase_rowid[256];
int counter = 0;
char sql[1024];
char utf[1024];
static sqlite3 *db;

void init_phrase()
{
  /* memset((char *) phrase_map, 0, sizeof(phrase_map)); */
  phrase_count = 0;
  phrase_index = 0;
  phrase_max   = 0;
  phrase_saved = 0;
  
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  if (counter >= 256) return 0;

  unsigned char *ptr = (unsigned char *) &phrase_word[counter][0];
  int count, index = 0, state = 0, bits = 0;

  /* utf[0] = 0; */
  /* for (count = 0; count < strlen(argv[0]); count++) { */
  /*   sprintf(utf + strlen(utf), "%02X ", argv[0][count] & 0x00FF); */
  /* } */

  index = 0;
  for (count = 0; count < strlen(argv[0]);) {
    ptr[index + 0] = ptr[index + 1] = 0;
    if (((argv[0][count] & 0x00FF) & 0x80) == 0) {
      ptr[index + 1] = argv[0][count] & 0x7f;
      ptr[index + 0] = 0;
      count += 1;
    } else if (((argv[0][count] & 0x00FF) & 0xe0) == 0xc0) {
      ptr[index + 1] = (argv[0][count] >> 2) & 0x07;
      ptr[index + 0] = ((argv[0][count] << 6) & 0xc0) | (argv[0][count + 1] & 0x3f);
      count += 2;
    } else if (((argv[0][count] & 0x00FF) & 0xe0) == 0xe0) {
      ptr[index + 1] = ((argv[0][count + 0] << 4) & 0xf0) | ((argv[0][count + 1] >> 2) & 0x0f);
      ptr[index + 0] = ((argv[0][count + 1] << 6) & 0xc0) | ((argv[0][count + 2]     ) & 0x3f);
      count += 3;
    }
    index += 2;
  }
  phrase_length[counter] = (index >> 1);
  ptr[index + 0] = 0;
  ptr[index + 1] = 0;
  phrase_frequency[counter] = atoi(argv[1]);
  phrase_rowid[counter]     = atoi(argv[2]);
  if (phrase_max < phrase_length[counter]) phrase_max = phrase_length[counter];
  
  /* sql[0] = 0; */
  /* for (count = 0; count < (index >> 1); count++) { */
  /*   sprintf(sql + strlen(sql), "%04X ", phrase_word[counter][count]); */
  /* } */
  /* LOGE("Phase Length : %d %d %d , Counter : %d, Word : %s, Utf : %s", index, */
  /*      phrase_length[counter], phrase_max, */
  /*      counter, sql, utf); */
  counter++;

  /* LOGE("Value : %s", argv[1]); */
  
  return 0;
}

int search_phrase(jchar index)
{
  /* int min = 0, max = sizeof(phraseindex) / sizeof(struct PHRASE_INDEX), mid = 0; */
  int count = 0;
  int ch = (int) index;
  int loop = 1;
  int found = -1;
  int total = 0;
  /* LOGE("Phrase : %d", sizeof(phraseindex) / sizeof(struct PHRASE_INDEX)); */

  counter = 0;
  phrase_max   = 0;
  snprintf(sql, 1024, "select phrase, frequency, rowid from phrase where key = %d order by frequency desc limit 256", index);
  int rc = sqlite3_exec(db, sql, callback, 0, 0);
  if (rc != 0) return 0;

  phrase_index = 0;
  phrase_count = counter;
  phrase_last_key = index;
  LOGE("Total Phrase : %d", phrase_count);

  return phrase_count;
#if 0
  while (max > min) {
    if (phraseindex[min].c == index) {
      found = min;
      break;
    }
    if (phraseindex[max].c == index) {
      found = max;
      break;
    }
    if (min == max)
      break;
    mid = (min + max) / 2;
    if (phraseindex[mid].c == index) {
      found = mid;
      break;
    }
    if (phraseindex[mid].c > index) {
      max = mid - 1;
    } else {
      min = mid + 1;
    }
    total++;
  }
  if (found >= 0) {
    int count = 0;
    /* LOGE("Phrase : %d %d %d %d %d %d", total, found, phraseindex[found].index, phraseindex[found].size, ch, phraseindex[found].c); */
    phrase_index = phraseindex[found].index;
    phrase_count = phraseindex[found].size;
    phrase_max   = phraseindex[found].maxphrase;
    /* if (pcount >= 10) pcount = 10; */
    /* for (count = pindex; count < pindex + pcount; count++) { */
    /*   LOGE("Phrase : %s", phrase[count]); */
    /* } */
    for (count = 0; count < phrase_count; count++) {
      phrase_map[count] = phrase_index + count;
      /* LOGE("Phrase Map0 : %d %d %d", count, phrase_map[count], phrase_freq[phrase_map[count]]); */
    }

    int n = phrase_count;
    int swap = 1;
    int temp = 0;
    do {
      swap = 0;
      int i = 0;
      for (i = 0; i < n - 1; i++) {
    	if (phrase_freq[phrase_map[i]] < phrase_freq[phrase_map[i + 1]]) {
    	  temp = phrase_map[i];
    	  phrase_map[i] = phrase_map[i + 1];
    	  phrase_map[i + 1] = temp;
    	  swap = 1;
    	}
      }
    } while (swap);

    /* for (count = 0; count < phrase_count; count++) { */
    /*   LOGE("Phrase Map1 : %d %d %d", count, phrase_map[count], phrase_freq[phrase_map[count]]); */
    /* } */
    /* LOGE("Search Phrase 3"); */

  } else {
    phrase_index = 0;
    phrase_count = 0;
    phrase_max   = 0;
    memset(phrase_map, 0, sizeof(phrase_map));
    /* LOGE("No Phrase : %d", sizeof(phraseindex) / sizeof(struct PHRASE_INDEX)); */
  }

  return phrase_count;
#endif
}

int get_phrase_count()
{
  return phrase_count;
}

int get_phrase_index()
{
  return phrase_index;
}

int get_phrase_max()
{
  /* LOGE("Phrase Max : %d", phrase_max); */
  return phrase_max;
}

jchar* get_phrase(int index)
{
  /* LOGE("Get Phrase : %d %d %d", index, phrase_index, index - phrase_index); */
  /* return &phrase[phrase_map[index - phrase_index]][1]; */
  /* LOGE("Get Phrase : %d %04X", index, phrase_word[index][0] & 0x00FFFF); */
  return phrase_word[index];
}

int get_phrase_length(int index)
{
  /* return (int) phrase[index][0]; */
  /* LOGE("Phrase Len :  %d %d", index, phrase_length[index]); */
  return phrase_length[index];
}

void update_phrase_frequency(int index)
{
  /* LOGE("Update Phrase Frequency : %d", index); */
  /* phrase_saved = 1; */
  /* phrase_freq[phrase_map[index - phrase_index]]++; */
  int i = 0, count;
  utf[0] = 0;
  for (count = 0; count < phrase_length[index]; count++) {
    unsigned short value = phrase_word[index][count];
    if ((value & 0xFF80) == 0) {
      utf[i] = value & 0x7F;
      i += 1;
    } else if ((value & 0xF800) == 0) {
      utf[i + 1] = (value & 0x3F) | 0x80;
      utf[i + 0] = ((value >> 6) & 0x1F) | 0xc0;
      i += 2;
    } else {
      utf[i + 2] = (value         & 0x3F) | 0x80;
      utf[i + 1] = ((value >>  6) & 0x3F) | 0x80;
      utf[i + 0] = ((value >> 12) & 0x0F) | 0xe0;
      i += 3;
    }
  }
  utf[i] = 0;

  sql[0] = 0;
  // snprintf(sql, 1024, "update phrase set frequency = frequency + 1 where key = %d and phrase like '%s'", phrase_last_key, utf);
  snprintf(sql, 1024, "update phrase set frequency = frequency + 1 where rowid = %d", phrase_rowid[index]);
  LOGE("Update Frequency : %s", sql);
  
  int rc = sqlite3_exec(db, sql, 0, 0, 0);
  if (rc != 0) {
    LOGE("Sql Error");
  }
}

void load_phrase(char *path)
{
  strncpy(phrase_path,          path, sizeof(phrase_path));
  strncat(phrase_path, "/phrase.db", sizeof(phrase_path));

  sqlite3_open(phrase_path, &db);
#if 0
  int clear = 1;
  char key[8];
  char buf[8];

  strncpy(phrase_path,          path, sizeof(phrase_path));
  strncat(phrase_path, "/phrase.dat", sizeof(phrase_path));

  memset(key, 0, 8);
  strcpy(key, "PHRAS0");

  /* LOGE("Load Phrase Path : %s", phrase_path); */
  FILE *file = fopen(phrase_path, "r");
  if (file != 0) {
    int read = fread(buf, 1, sizeof(buf), file);
    /* LOGE("Load Phrase0 : %d", read); */
    /* LOGE("Load Phrase0-1 : %02X %02X %02X %02X %02X %02X %02X %02X ", */
    /* 	 buf[0], buf[1], buf[2], buf[3],  */
    /* 	 buf[4], buf[5], buf[6], buf[7] */
    /* 	 ); */
    if (memcmp(buf, key, 8) == 0) {
      int read = fread(phrase_freq, 1, sizeof(phrase_freq), file);
      /* LOGE("Load Phrase1 : %d %d", read, sizeof(phrase_freq)); */
      fclose(file);
      if (read == sizeof(phrase_freq)) clear = 0;
    }
  }

  if (clear != 0) {
    memset(phrase_freq, 0, sizeof(phrase_freq));
  }
#endif
}

void save_phrase()
{
#if 0
  char key[8];

  if (phrase_saved == 0) return;
  phrase_saved = 0;

  memset(key, 0, 8);
  strcpy(key, "PHRAS0");
  FILE *file = fopen(phrase_path, "w");
  if (file != NULL) {
    fwrite(key, 1, sizeof(key), file);
    fwrite(phrase_freq, 1, sizeof(phrase_freq), file);
    /* LOGE("Save Phrase : %d", sizeof(phrase_freq)); */
    fclose(file);
  }
#endif
  sqlite3_close(&db);
  sqlite3_open(phrase_path, &db);
}

void clear_phrase()
{
  /* LOGE("Clear Phrase"); */
  // memset(phrase_freq, 0, sizeof(phrase_freq));
}

jint get_phrase_frequency(int index)
{
  /* LOGE("Phrase Frequency : %d %d %d", index, phrase_map[index], phrase_freq[phrase_map[index]]); */
  /* return phrase_freq[phrase_map[index - phrase_index]]; */
  return phrase_frequency[index];
}

void learn_phrase(jchar key, jchar value)
{
  sql[0] = 0;
}
