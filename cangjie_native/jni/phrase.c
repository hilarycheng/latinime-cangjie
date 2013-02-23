#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#include "phrase_internal.h"

int phrase_count = 0;
int phrase_index = 0;
int phrase_max   = 0;

int search_phrase(jchar index)
{
  int min = 0, max = sizeof(phraseindex) / sizeof(struct PHRASE_INDEX), mid = 0;
  int count = 0;
  int ch = (int) index;
  int loop = 1;
  int found = -1;
  int total = 0;
  /* LOGE("Phrase : %d", sizeof(phraseindex) / sizeof(struct PHRASE_INDEX)); */

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
    /* LOGE("Phrase : %d %d %d %d %d %d", total, found, phraseindex[found].index, phraseindex[found].size, ch, phraseindex[found].c); */
    phrase_index = phraseindex[found].index;
    phrase_count = phraseindex[found].size;
    phrase_max   = phraseindex[found].maxphrase;
    /* if (pcount >= 10) pcount = 10; */
    /* for (count = pindex; count < pindex + pcount; count++) { */
    /*   LOGE("Phrase : %s", phrase[count]); */
    /* } */
  } else {
    phrase_index = 0;
    phrase_count = 0;
    phrase_max   = 0;
    /* LOGE("No Phrase : %d", sizeof(phraseindex) / sizeof(struct PHRASE_INDEX)); */
  }

  return phrase_count;
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
  return phrase_max;
}

jchar* get_phrase(int index)
{
  return &phrase[index][1];
}

int get_phrase_length(int index)
{
  return (int) phrase[index][0];
}
