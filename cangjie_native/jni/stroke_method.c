#include <stdio.h>
#include <string.h>
#include "stroke_method.h"
#include "stroke.h"
#ifndef X86
#include <android/log.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGE(...)
#endif

char dbg[2048];
char stroke_char[STROKE_MAXKEY + 1];
char stroke_char_second[STROKE_MAXKEY + 1];
int stroke_index[STROKE_TOTAL];
int stroke_index_temp[STROKE_TOTAL];
int stroke_frequency[STROKE_TOTAL];
int _mTotalMatch;

void stroke_init(char *path)
{
  int clear = 0;
  int count = 0;
  char key[8];

  stroke_func.mSaved = 0;
  strncpy(stroke_func.mPath,          path, sizeof(stroke_func.mPath));
  strncat(stroke_func.mPath, "/stroke.dat", sizeof(stroke_func.mPath));

  for (count = 0; count < STROKE_TOTAL; count++) {
    stroke_index[count] = -1;
  }
  memset(key, 0, 8);
  strcpy(key, "STROKE0");

  clear = 1;
  FILE *file = fopen(stroke_func.mPath, "r");
  if (file != 0) {
    int read = fread(stroke_func.mBuffer, 1, sizeof(stroke_func.mBuffer), file);
    if (memcmp(stroke_func.mBuffer, key, 8) == 0) {
      int read = fread(stroke_frequency, 1, sizeof(stroke_frequency), file);
      fclose(file);
      if (read == sizeof(stroke_frequency)) clear = 0;
    }
  }
   
  if (clear != 0) {
    for (count = 0; count < sizeof(stroke_frequency) / sizeof(jint); count++) {
      stroke_frequency[count] = 0;
    }
  }
}

int stroke_maxKey(void)
{
  return STROKE_MAXKEY;
}

void stroke_searchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
}

void stroke_searchWordMore(jchar *key0, jchar *key1, jchar *key2, jchar *key3, jchar *key4)
{
}

void stroke_searchWordArray(jchar *key, int len)
{
  if (_mTotalMatch > 0) {
    stroke_func.mTotalMatch = _mTotalMatch;
    memcpy(stroke_index, stroke_index_temp, _mTotalMatch * sizeof(int));

    int swap = 1, i;
    while (swap) {
      swap = 0;
      for (i = 0; i < _mTotalMatch - 1; i++) {
	if (stroke_frequency[stroke_index[i]] < stroke_frequency[stroke_index[i + 1]]) {
	  int temp = stroke_index[i];
	  stroke_index[i] = stroke_index[i + 1];
	  stroke_index[i + 1] = temp;
	  swap = 1;
	}
      }
    }
  }
}

int smemcmp(char *s, char *d, int len)
{
  int count = 0;
  for (count = 0; count < len; count++) {
    if ((d[count] & 0x0F) == 0) {
      if ((s[count] & 0xF0) != d[count]) {
	return 1;
      }
    } else {
      if (s[count] != d[count]) {
	return 1;
      }
    }
  }

  return 0;
}

jboolean stroke_tryMatchWordArray(jchar *key, int len)
{
  int index = 0, count;
  int _len = (len & ~0x01) + ((len & 0x01) << 1);
  int firstlen = 0;
  char pair = 0;
  int found = 0;

  for (count = 0; count < len; count++) {
    if (key[count] == '*') found++;
  }

  if (found > 1)
    return 0;
  
  memset(stroke_char, 0, sizeof(stroke_char));
  index = 0;
  for (count = 0; count < _len; count++) {
    pair = pair << 4;
    if (key[count] == '*') {
      stroke_char[index++] = pair;
      break;
    }
    if (count < len) {
      if (key[count] < '0' || key[count] > '5')
	continue;
      pair = pair | ((key[count] - '0') & 0x000F);
      if ((count & 0x01) == 0)
	continue;
    }
    stroke_char[index++] = pair;
    pair = 0;
  }

  /* if (stroke_char[index] == 0 && index > 0) index--; */
  /* firstlen = index + 1; */
  dbg[0] = 0;
  for (count = 0; count < 32; count++) {
    sprintf(dbg + strlen(dbg), "%02X ", stroke_char[count]);
  }
  LOGE("Stroke : %d %d %s", firstlen, index, dbg);

  int start = stroke_map[key[0] - '1'].index;
  int end   = stroke_map[key[0] - '1'].count + start;

  index = 0;
  for (count = start; count < end; count++) {
    if (len > stroke[count].num)
      continue;
    if (smemcmp(stroke[count].stroke, stroke_char, _len >> 1) == 0) {
      stroke_index_temp[index++] = count;
    }
  }

  _mTotalMatch = index;

  return _mTotalMatch > 0;
}

int stroke_totalMatch(void)
{
  return stroke_func.mTotalMatch;
}

int stroke_updateFrequency(jchar ch)
{
  int total = STROKE_TOTAL;
  int count = 0;
  int max = 0;
  int index = 0;

  for (count = 0; count < total; count++) {
    if (stroke_frequency[count] > max) max = stroke_frequency[count];
  }
 
  for (count = 0; count < total; count++) {
    if (stroke[count].ch == ch) {
      if (stroke_frequency[count] < max || max == 0) {
	stroke_frequency[count]++;
	stroke_func.mSaved = 1;
	return stroke_frequency[count];
      }
    }
  }

  for (count = 0; count < total; count++) {
    if (stroke_frequency[count] == max && count != index) {
      stroke_frequency[index]++;
      stroke_func.mSaved = 1;
      return stroke_frequency[index];
    }
  }
  
  return stroke_frequency[index];
}

void stroke_clearFrequency(void)
{
  int count = 0;
  
  for (count = 0; count < STROKE_TOTAL; count++) {
    stroke_frequency[count] = 0;
  }

  remove(stroke_func.mPath);
}

jchar stroke_getMatchChar(int index)
{
  return stroke[stroke_index[index]].ch;
}

jint stroke_getFrequency(int index)
{
  return stroke_frequency[stroke_index[index]];
}

void stroke_reset(void)
{
  stroke_func.mTotalMatch = 0;
  _mTotalMatch = 0;
}

void stroke_saveMatch(void)
{
  char key[8];

  if (stroke_func.mSaved == 0) return;
  stroke_func.mSaved = 0;

  memset(key, 0, 8);
  strcpy(key, "STROKE0");
  FILE *file = fopen(stroke_func.mPath, "w");
  if (file != NULL) {
    fwrite(key, 1, sizeof(key), file);
    fwrite(stroke_frequency, 1, sizeof(stroke_frequency), file);
    fclose(file);
  }
}

jboolean stroke_tryMatchWord(jchar c0, jchar c1, jchar c2, jchar c3, jchar c4)
{
  return 1;
}

jboolean stroke_tryMatchWordMore(jchar* c0, jchar* c1, jchar* c2, jchar* c3, jchar* c4)
{
  return 1;
}

void stroke_enableHongKongChar(jboolean hk)
{
  stroke_func.mEnableHK = (hk != 0);
}

struct _input_method stroke_func =
{
  .init            = stroke_init,
  .maxKey          = stroke_maxKey,
  .searchWord      = stroke_searchWord,
  .searchWordMore  = stroke_searchWordMore,
  .searchWordArray = stroke_searchWordArray,
  .tryMatchWord    = stroke_tryMatchWord,
  .tryMatchWordMore = stroke_tryMatchWordMore,
  .tryMatchWordArray = stroke_tryMatchWordArray,
  .enableHongKongChar = stroke_enableHongKongChar,
  .totalMatch      = stroke_totalMatch,
  .updateFrequency = stroke_updateFrequency,
  .clearFrequency  = stroke_clearFrequency,
  .getMatchChar    = stroke_getMatchChar,
  .getFrequency    = stroke_getFrequency,
  .reset           = stroke_reset,
  .saveMatch       = stroke_saveMatch
};
