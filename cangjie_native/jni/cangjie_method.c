#include <stdio.h>
#include <string.h>
#include "cangjie_method.h"
#include "cangjie.h"
#ifndef X86
#include <android/log.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGE(...)
#endif

void cangjie_init(char *path)
{
  int clear = 0;
  int count = 0;

  cangjie_func.mSaved = 0;
  strncpy(cangjie_func.mPath,           path, sizeof(cangjie_func.mPath));
  strncat(cangjie_func.mPath, "/cangjie.dat", sizeof(cangjie_func.mPath));

  for (count = 0; count < sizeof(cangjie_index) / sizeof(jint); count++) {
    cangjie_index[count] = -1;
  }

  FILE *file = fopen(cangjie_func.mPath, "r");
  if (file != 0) {
    int read = fread(cangjie_frequency, 1, sizeof(cangjie_frequency), file);
    fclose(file);
    if (read != sizeof(cangjie_frequency))
      clear = 1;
  } else {
    clear = 1;
  }
   
  if (clear != 0) {
    for (count = 0; count < sizeof(cangjie_frequency) / sizeof(jint); count++) {
      cangjie_frequency[count] = 0;
    }
  }
}

int cangjie_maxKey(void)
{
  return 5;
}

jboolean cangjie_searching(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4, int updateindex)
{
  jchar src[6];
  int total = sizeof(cangjie) / (sizeof(jchar) * CANGJIE_COLUMN);
  int loop  = 0;
  int found = 0;
  int ismatch = 0;
  int count0 = 0, count1 = 0, state = 0;
  int firstlen = 0, secondlen = 0, firstmatch = 0;

  src[0] = key0;
  src[1] = key1;
  src[2] = key2;
  src[3] = key3;
  src[4] = key4;
  src[5] = 0;

  found = 0;
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == '*') found++;
  }

  if (found > 1)
    return 0;

  if (updateindex != 0) {
    for (count0 = 0; count0 < sizeof(cangjie_index) / sizeof(jint); count0++) {
      cangjie_index[count0] = 0;
    }
  }

  // Clear End Star Match
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == '*' && src[count0 + 1] == 0) {
      src[count0] = 0;
      break;
    }
  }

  state = 0; firstlen = 0; secondlen = 0; 
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == 0)
      break;
    if (src[count0] == '*')
      state = 1;
    if (state == 0)
      firstlen++;
    else
      secondlen++;
  }

  if (secondlen > 0)
    secondlen--;

  found = 0; state = 0;
  for (count0 = 0; count0 < total; count0++) {
    ismatch = 0;
    if (memcmp(cangjie[count0], src, firstlen * sizeof(jchar)) == 0) {
      state = 1;
      firstmatch++;
      if (secondlen == 0 && (cangjie_func.mEnableHK != 0 || cangjie[count0][6] == 0)) {
	ismatch = 1;
      } else {
	if (firstlen + secondlen <= cangjie[count0][7]) {
	  if (memcmp(&cangjie[count0][cangjie[count0][7] - secondlen], &src[firstlen + 1], secondlen * sizeof(jchar)) == 0 &&
	      (cangjie_func.mEnableHK != 0 || cangjie[count0][6] == 0)) {
	    ismatch = 1;
	  }
	} else {
	  ismatch = 0;
	}
      }
      if (ismatch) {
	if (updateindex != 0)
	  cangjie_index[loop] = count0;
	loop++;
	/* LOGE("Matched %d = %02x %02x %02x %02x %02x = %02x %02x %02x %02x %02x", */
	/*      firstlen, */
	/*      src[0], */
	/*      src[1], */
	/*      src[2], */
	/*      src[3], */
	/*      src[4], */
	/*      cangjie[count0][0], */
	/*      cangjie[count0][1], */
	/*      cangjie[count0][2], */
	/*      cangjie[count0][3], */
	/*      cangjie[count0][4]); */
      }
    } else if (state == 1) {
      break;
    }
  }

  if (updateindex != 0)
    cangjie_func.mTotalMatch = loop;

  if (firstmatch > 0 && secondlen > 0 && updateindex == 0)
    return 1;
  
  return (loop > 0) ? 1 : 0;
}

void cangjie_searchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  cangjie_searching(key0, key1, key2, key3, key4, 1);
}

int cmp(jchar *s, jchar *d, int n) {
  int c = 0, l = 0;
  for (c = 0; c < n; c++) {
    if (s[c] != d[c]) l++;
  }
  return l;
}

jboolean cangjie_tryMatchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  return cangjie_searching(key0, key1, key2, key3, key4, 0);
}

int cangjie_totalMatch(void)
{
  return cangjie_func.mTotalMatch;
}

int cangjie_updateFrequency(jchar ch)
{
  int index = 0;

  return cangjie_frequency[index];
}

void cangjie_clearFrequency(void)
{
  int count = 0;
  
  for (count = 0; count < sizeof(cangjie_frequency) / sizeof(jint); count++) {
    cangjie_frequency[count] = 0;
  }

  remove(cangjie_func.mPath);
}

jchar cangjie_getMatchChar(int index)
{
  int total = sizeof(cangjie) / (sizeof(jchar) * CANGJIE_COLUMN);

  if (index >= total) return 0;
  if (cangjie_index[index] < 0) return 0;

  return cangjie[cangjie_index[index]][5];
}

jint cangjie_getFrequency(int index)
{
  return 0;
}

void cangjie_reset(void)
{
  cangjie_func.mTotalMatch = 0;
}

void cangjie_saveMatch(void)
{
  if (cangjie_func.mSaved == 0) return;
  cangjie_func.mSaved = 0;
  FILE *file = fopen(cangjie_func.mPath, "w");
  if (file != NULL) {
    fwrite(cangjie_frequency, 1, sizeof(cangjie_frequency), file);
    fclose(file);
  }
}

void cangjie_enableHongKongChar(jboolean hk)
{
  cangjie_func.mEnableHK = (hk != 0);
}

struct _input_method cangjie_func =
{
  .init            = cangjie_init,
  .maxKey          = cangjie_maxKey,
  .searchWord      = cangjie_searchWord,
  .tryMatchWord    = cangjie_tryMatchWord,
  .enableHongKongChar = cangjie_enableHongKongChar,
  .totalMatch      = cangjie_totalMatch,
  .updateFrequency = cangjie_updateFrequency,
  .clearFrequency  = cangjie_clearFrequency,
  .getMatchChar    = cangjie_getMatchChar,
  .getFrequency    = cangjie_getFrequency,
  .reset           = cangjie_reset,
  .saveMatch       = cangjie_saveMatch
};
