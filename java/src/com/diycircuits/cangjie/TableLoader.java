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

    public native void setPath(byte[] path);
    public native void initialize();
    public native char getChar();
    public native int getMaxKey();
    public native char passCharArray(char[] array);
    public native int  updateFrequency(char ch);
    public native void enableHongKongChar(boolean hk);
    public native void setInputMethod(int im);
    public native void searchCangjie(char index0, char index1, char index2, char index3, char index4);
    public native void searchCangjieMore(char[] index0, char[] index1, char[] index2, char[] index3, char[] index4);
    public native boolean tryMatchCangjie(char index0, char index1, char index2, char index3, char index4);
    public native boolean tryMatchCangjieMore(char[] index0, char[] index1, char[] index2, char[] index3, char[] index4);
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
    public native void getPhrase(int index, StringBuffer buffer);
    public native int  getPhraseArray(int index, char phrase[]);

    static {
	System.loadLibrary("chinese_table");
    }

}

