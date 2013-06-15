package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.util.Log;

import java.util.HashMap;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.UnsupportedEncodingException;

public class TableLoader {

    public final static int QUICK   = 0;
    public final static int CANGJIE = 1;
    public final static int STROKE  = 2;

    public static TableLoader mInstance = null;

    public native void setPath(byte[] path);
    public native void initialize();
    public native char getChar();
    public native int getMaxKey();
    public native char passCharArray(char[] array);
    public native int  updateFrequency(char ch);
    public native void enableHongKongChar(boolean hk);
    public native void setInputMethod(char im);
    public native void searchWord(char[] key, int len);
    public native void searchCangjie(char index0, char index1, char index2, char index3, char index4);
    public native void searchCangjieMore(char[] index0, char[] index1, char[] index2, char[] index3, char[] index4);
    public native boolean tryMatchCangjie(char index0, char index1, char index2, char index3, char index4);
    public native boolean tryMatchCangjieMore(char[] index0, char[] index1, char[] index2, char[] index3, char[] index4);
    public native boolean trySearchWord(char[] key, int len);
    public native int totalMatch();
    public native char getMatchChar(int index);
    public native int getFrequency(int index);
    public native void saveMatch();
    public native void clearAllFrequency();
    public native void reset();
    public native int searchPhrase(char c);
    public native int getPhraseCount();
    public native int getPhraseIndex();
    public native int getPhraseMax();
    public native int getAllPhraseMax();
    public native void getPhrase(int index, StringBuffer buffer);
    public native int  getPhraseArray(int index, char phrase[]);
    public native int  measurePhraseRow(int width, int charwidth, int spacing, int fontSpacing, int[] phraseRow);
    public native void updatePhraseFrequency(int idx);
    public native int getPhraseFrequency(int idx);
    public native void learnPhrase(char key, char value);
    public native void setSortingMethod(int method);
    public native void setupOnce();
    public native int getInputMethodCount();
    public native String[] getInputMethodNameList();
    public native String[] getInputMethodIndexList();

    static {
	System.loadLibrary("chinese_table");
	mInstance = new TableLoader();
	mInstance.setupOnce();
	// Log.i("Cangjie", "Table Loader Initialize");
    }

}

