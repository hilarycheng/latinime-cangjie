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

char stroke_char[STROKE_MAXKEY + 1];
int stroke_index[STROKE_TOTAL];
int stroke_index_temp[STROKE_TOTAL];
int _mTotalMatch;

void stroke_init(char *path)
{
  stroke_func.mTotalMatch = 0;
  _mTotalMatch = 0;
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
  }
}

jboolean stroke_tryMatchWordArray(jchar *key, int len)
{
  int index = 0, count;
  for (count = 0; count < len; count++) stroke_char[count] = key[count];

  int start = stroke_map[key[0] - '1'].index;
  int end   = stroke_map[key[0] - '1'].count + start;
  
  for (count = start; count < end; count++) {
    if (memcmp(stroke[count].stroke, stroke_char, len) == 0) {
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
  /*
  int total = sizeof(stroke) / (sizeof(jchar) * 3);
  int count = 0;
  int max = 0;
  int index = 0;

  for (count = 0; count < total; count++) {
    if (stroke_frequency[count] > max) max = stroke_frequency[count];
  }
 
  for (count = 0; count < total; count++) {
    if (stroke[count][2] == ch) {
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
  */
  return 0;
}

void stroke_clearFrequency(void)
{
  /*
  int count = 0;
  
  for (count = 0; count < sizeof(stroke_frequency) / sizeof(jint); count++) {
    stroke_frequency[count] = 0;
  }

  remove(stroke_func.mPath);
  */
}

jchar stroke_getMatchChar(int index)
{
  /*
  int total = sizeof(stroke) / (sizeof(jchar) * 3);

  if (index >= total) return 0;
  if (stroke_index[index] < 0) return 0;

  return stroke[stroke_index[index]][2];
  */

  return stroke[stroke_index[index]].ch;
}

jint stroke_getFrequency(int index)
{
  /*
  return stroke_frequency[stroke_index[index]];
  */

  return 0;
}

void stroke_reset(void)
{
  stroke_func.mTotalMatch = 0;
  _mTotalMatch = 0;
}

void stroke_saveMatch(void)
{
  /*
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
  */
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
