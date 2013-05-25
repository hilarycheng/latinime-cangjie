#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include "input_method.h"
#include "phrase.h"

int mTotalMatch = 0;
int mSaved = 0;
int mCurrentIm = CANGJIE;
jboolean mEnableHK = 0;
char data_path[1024] = "0";
char quick_data[1024] = "0";
char cangjie_data[1024] = "0";
char cangjie_hk_data[1024] = "0";
jchar keyStorage[64];

void Java_com_diycircuits_cangjie_TableLoader_setPath(JNIEnv *env, jobject thiz, jbyteArray path)
{
  jbyte *buf = (*env)->GetByteArrayElements(env, path, NULL);
  jsize len = (*env)->GetArrayLength(env, path);

  strncpy(data_path, buf, len);
  quick_data[0] = 0;
  strncat(quick_data, data_path, sizeof(quick_data));

  (*env)->ReleaseByteArrayElements(env, path, buf, 0);
}

void Java_com_diycircuits_cangjie_TableLoader_initialize(JNIEnv* env, jobject thiz)
{
  input_method[QUICK]->init(quick_data);  
  input_method[CANGJIE]->init(quick_data);
  input_method[STROKE]->init(quick_data);
  init_phrase();
  load_phrase(quick_data);
}

void Java_com_diycircuits_cangjie_TableLoader_reset(JNIEnv* env, jobject thiz)
{
  input_method[QUICK]->reset();
  input_method[CANGJIE]->reset();
  input_method[STROKE]->reset();
}
 
jchar Java_com_diycircuits_cangjie_TableLoader_getChar(JNIEnv* env, jobject thiz)
{
  /* int count = 0; */

  /* for (count = 0; count < sizeof(quick_index) / sizeof(jint); count++) { */
  /*   quick_index[count] = -1; */
  /* } */

  /* LOGE("Char Quick : %d", sizeof(quick) / (sizeof(jchar) * 4)); */
  /* LOGE("Char Length : %d", sizeof(jchar)); */
  /* LOGE("Char Value : %d %02X %02X %02X\n", strlen(value), value[0] & 0x00FF, value[1] & 0x00FF, value[2] & 0x00FF); */
  
  return 0;
}

jchar Java_com_diycircuits_cangjie_TableLoader_passCharArray(JNIEnv* env, jobject thiz, jcharArray arr)
{
  jchar *buf = (*env)->GetCharArrayElements(env, arr, NULL);
  jsize len = (*env)->GetArrayLength(env, arr);

  /* LOGE("Array Length : %d", len); */

  (*env)->ReleaseCharArrayElements(env, arr, buf, 0);

  return 0;
}

jint Java_com_diycircuits_cangjie_TableLoader_getMaxKey(JNIEnv* env, jobject thiz)
{
  return input_method[mCurrentIm]->maxKey();
}

void Java_com_diycircuits_cangjie_TableLoader_setInputMethod(JNIEnv* env, jobject thiz, jint im)
{
  mCurrentIm = im;
}
 
void Java_com_diycircuits_cangjie_TableLoader_searchCangjie(JNIEnv* env, jobject thiz, jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  input_method[mCurrentIm]->searchWord(key0, key1, key2, key3, key4);
}

void Java_com_diycircuits_cangjie_TableLoader_searchCangjieMore(JNIEnv* env, jobject thiz, jcharArray key0, jcharArray key1, jcharArray key2, jcharArray key3, jcharArray key4)
{
  jchar key0array[6];
  jchar key1array[6];
  jchar key2array[6];
  jchar key3array[6];
  jchar key4array[6];

  (*env)->GetCharArrayRegion(env, key0, 0, 6, key0array);
  (*env)->GetCharArrayRegion(env, key1, 0, 6, key1array);
  (*env)->GetCharArrayRegion(env, key2, 0, 6, key2array);
  (*env)->GetCharArrayRegion(env, key3, 0, 6, key3array);
  (*env)->GetCharArrayRegion(env, key4, 0, 6, key4array);
  
  input_method[mCurrentIm]->searchWordMore(key0array, key1array, key2array, key3array, key4array);
}

void Java_com_diycircuits_cangjie_TableLoader_enableHongKongChar(JNIEnv* env, jobject thiz, jboolean hk)
{
  mEnableHK = hk;
  input_method[mCurrentIm]->enableHongKongChar(mEnableHK);
}

jboolean Java_com_diycircuits_cangjie_TableLoader_tryMatchCangjie(JNIEnv* env, jobject thiz, jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  return input_method[mCurrentIm]->tryMatchWord(key0, key1, key2, key3, key4);
}
 
void Java_com_diycircuits_cangjie_TableLoader_searchWord(JNIEnv* env, jobject thiz, jcharArray key, jint len)
{
  memset(keyStorage, 0, sizeof(keyStorage));
  (*env)->GetCharArrayRegion(env, key, 0, 64, keyStorage);

  if (mCurrentIm == QUICK || mCurrentIm == CANGJIE)
    input_method[mCurrentIm]->searchWord(keyStorage[0], keyStorage[1], keyStorage[2], keyStorage[3], keyStorage[4]);
  else if (mCurrentIm == STROKE)
    input_method[mCurrentIm]->searchWordArray(keyStorage, len);
}

jboolean Java_com_diycircuits_cangjie_TableLoader_tryMatchCangjieMore(JNIEnv* env, jobject thiz, jcharArray key0, jcharArray key1, jcharArray key2, jcharArray key3, jcharArray key4)
{
  jchar key0array[6];
  jchar key1array[6];
  jchar key2array[6];
  jchar key3array[6];
  jchar key4array[6];

  (*env)->GetCharArrayRegion(env, key0, 0, 6, key0array);
  (*env)->GetCharArrayRegion(env, key1, 0, 6, key1array);
  (*env)->GetCharArrayRegion(env, key2, 0, 6, key2array);
  (*env)->GetCharArrayRegion(env, key3, 0, 6, key3array);
  (*env)->GetCharArrayRegion(env, key4, 0, 6, key4array);
  
  return input_method[mCurrentIm]->tryMatchWordMore(key0array, key1array, key2array, key3array, key4array);
}
 
jint Java_com_diycircuits_cangjie_TableLoader_totalMatch(JNIEnv* env, jobject thiz)
{
  return input_method[mCurrentIm]->totalMatch();
}
 
jchar Java_com_diycircuits_cangjie_TableLoader_getMatchChar(JNIEnv* env, jobject thiz, jint index)
{
  return input_method[mCurrentIm]->getMatchChar(index);
}
 
jint Java_com_diycircuits_cangjie_TableLoader_getFrequency(JNIEnv* env, jobject thiz, jint index)
{
  return input_method[mCurrentIm]->getFrequency(index);
}
 
jint Java_com_diycircuits_cangjie_TableLoader_updateFrequency(JNIEnv* env, jobject thiz, jchar ch)
{
  return input_method[mCurrentIm]->updateFrequency(ch);
}
			
void Java_com_diycircuits_cangjie_TableLoader_saveMatch(JNIEnv* env, jobject thiz)
{
  input_method[QUICK]->saveMatch();
  input_method[CANGJIE]->saveMatch();
  save_phrase();
}

void Java_com_diycircuits_cangjie_TableLoader_clearAllFrequency(JNIEnv *env, jobject thiz)
{
  input_method[QUICK]->clearFrequency();
  input_method[CANGJIE]->clearFrequency();
  clear_phrase();
}

jint Java_com_diycircuits_cangjie_TableLoader_searchPhrase(JNIEnv *env, jobject thiz, jchar ch)
{
  /* jchar charray[16]; */
  /* (*env)->GetCharArrayRegion(env, ch, 0, 16, charray); */

  /* LOGE("Search Phrase"); */
  return search_phrase(ch);
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseCount(JNIEnv *env, jobject thiz)
{
  /* LOGE("Get Phrase Count"); */
  return get_phrase_count();
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseIndex(JNIEnv *env, jobject thiz)
{
  /* LOGE("Get Phrase Index"); */
  return get_phrase_index();
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseMax(JNIEnv *env, jobject thiz)
{
  /* LOGE("Get Phrase Max"); */
  return get_phrase_max();
}

jint Java_com_diycircuits_cangjie_TableLoader_getAllPhraseMax(JNIEnv *env, jobject thiz)
{
  return MAX_PHRASE_LENGTH;
}

void Java_com_diycircuits_cangjie_TableLoader_getPhrase(JNIEnv *env, jobject thiz, jint index, jobject sb)
{
  jclass cls = (*env)->GetObjectClass(env, sb);
  jmethodID id = (*env)->GetMethodID(env, cls, "append", "(C)Ljava/lang/StringBuffer;");
  if (id != 0) {
    /* LOGE("Method ID Found"); */
    int max = get_phrase_max();
    if (max > 0) {
      int count = 0;
      jchar *p = get_phrase(index);
      for (count = 0; count < max; count++) {
	if (p[count] == 0) return;
	(*env)->CallObjectMethod(env, sb, id, p[count]);
      }
    }
  /* } else { */
  /*   LOGE("Method ID Not Found"); */
  }
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseArray(JNIEnv *env, jobject thiz, jint index, jcharArray phrase)
{
  jint count = 0, total = 0;
  int max = get_phrase_max();
  jchar *input = get_phrase(index);
  jchar *buf = (*env)->GetCharArrayElements(env, phrase, NULL);
  jsize len = (*env)->GetArrayLength(env, phrase);

  for (count = 0; count < max; count++) {
    buf[count] = input[count];
    if (buf[count] == 0) {
      break;
    }
    total++;
  }

  (*env)->ReleaseCharArrayElements(env, phrase, buf, 0);

  return total;
}

jint Java_com_diycircuits_cangjie_TableLoader_measurePhraseRow(JNIEnv *env, jobject thiz,
							       jint width, jint charwidth, jint spacing, jint textFontSpacing, jintArray phraseRow)
{
  jint *buf = (*env)->GetIntArrayElements(env, phraseRow, NULL);
  jsize len = (*env)->GetArrayLength(env, phraseRow);

  int count = 0;
  int offset = 0;
  int row = 0;
  int plen = 0;

  for (count = 0; count < get_phrase_count(); count++) {
    if (offset + spacing + charwidth * get_phrase_length(get_phrase_index() + count) > (width - (1 * charwidth)) || count == 0) {
      /* LOGE("Phrase Row : %d = %d", row, get_phrase_index() + count); */
      buf[row] = get_phrase_index() + count;
      row++;
      offset = 0;
    }
    plen = get_phrase_length(get_phrase_index() + count);
    offset += spacing + charwidth * plen + textFontSpacing * (plen - 1);
  }

  /* if (offset > 0) row++; */
  buf[row] = get_phrase_index() + get_phrase_count();

  (*env)->ReleaseIntArrayElements(env, phraseRow, buf, 0);

  return row;
}

void Java_com_diycircuits_cangjie_TableLoader_updatePhraseFrequency(JNIEnv *env, jobject thiz, jint idx)
{
  update_phrase_frequency(idx);
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseFrequency(JNIEnv *env, jobject thiz, jint idx)
{
  return get_phrase_frequency(idx);
}
