package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;
import android.preference.PreferenceManager;
import android.content.Context;
import android.util.AttributeSet;
import android.view.*;
import android.view.View.OnClickListener;
import android.widget.*;
import android.graphics.*;
import android.view.LayoutInflater;
import android.util.Log;
import android.os.Handler;
import android.os.Message;

public class CandidateRow extends View implements View.OnClickListener, View.OnTouchListener {

    private final static int CHARACTER_MODE = 0;
    private final static int PHRASE_MODE    = 1;
    private static Paint mPaint = null;
    private static Rect mRect = new Rect();
    private static Rect mFontRect = new Rect();
    private static int cspacing = 0;
    private static int mLeftOffset = 0;
    private static int mTextWidth = 0;
    private static int mTextFontSpacing = 0;
    private int pspacing = 0;
    private int mWidth = 100;
    private int mHeight = 64;
    private int mPreferredHeight = 64;
    private int mTotal = 0;
    private int mOffset = 0;
    private int mTopOffset = 0;
    private float mFontSize = 50.0f;
    private Context context = null;
    private Handler mHandler = null;
    private int mAllTotal = 0;
    private int mLastX = -1;
    private int mLastY = -1;
    private int mSelectIndex = -1;
    private int mState = CHARACTER_MODE;
    private char[] mPhrase = null;
    private TableLoader mTable = null;

    public CandidateRow(Context context, AttributeSet attrs) {
	super(context, attrs);

	this.context = context;

	setupPaint();

	setClickable(true);
	setOnClickListener(this);
	setOnTouchListener(this);
    }

    private int getNormalColor() {
	return PreferenceManager.getDefaultSharedPreferences(context).getInt("normal_character_color", 0xff33B5E5);
    }
    
    private int getFrequentColor() {
	return PreferenceManager.getDefaultSharedPreferences(context).getInt("frequent_character_color", 0xffff9000);
    }

    public void setPreferredHeight(int height) {
	mPreferredHeight = height;
    }
    
    private synchronized void setupPaint() {
	if (mPaint == null) {
	    mPaint = new Paint();
	    mPaint.setColor(Color.BLACK);
	    mPaint.setAntiAlias(true);
	    mPaint.setTextSize(mFontSize);
	    mPaint.setStrokeWidth(0);
    	    mPaint.getTextBounds(context.getString(R.string.cangjie), 0, 1, mRect);
    	    mPaint.getTextBounds(context.getString(R.string.cangjie_full), 0, 2, mFontRect);
	}
    }

    public void setHandler(Handler handler) {
	mHandler = handler;
    }
    
    public void setFontSize(float fs, int off) {
	mFontSize = fs;
	mTopOffset = off;
	synchronized(mPaint) {
	    if (mPaint.getTextSize() != mFontSize) {
		mPaint.setTextSize(mFontSize);
		mPaint.getTextBounds(context.getString(R.string.cangjie), 0, 1, mRect);
		mPaint.getTextBounds(context.getString(R.string.cangjie_full), 0, 2, mFontRect);
	    }
	}
    }
    
    public void setMatch(int offset, int total, int alltotal) {
	mState = CHARACTER_MODE;
	mOffset = offset;
	mTotal  = total;
	if (mAllTotal != alltotal) cspacing = 0;
	mAllTotal = alltotal;
    }
    
    public void setMatch(TableLoader table, int offset, int total, int alltotal) {
	mState = CHARACTER_MODE;
	mTable = table;
	mOffset = offset;
	mTotal  = total;
	if (mAllTotal != alltotal) cspacing = 0;
	mAllTotal = alltotal;

	synchronized(this) {
	    if (mPhrase == null) mPhrase = new char[mTable.getAllPhraseMax()];
	}
    }

    public void setPhrase(TableLoader table, int offset, int total) {
	mState = PHRASE_MODE;
	mTable = table;
	mOffset = offset;
	mTotal = total;

	synchronized(this) {
	    if (mPhrase == null) mPhrase = new char[mTable.getAllPhraseMax()];
	}
    }
    
    @Override
    public void onClick(View v) {
	if (mLastX != -1 && mLastY != -1) {
	    if (mState == CHARACTER_MODE) {
		int x = mLastX;
		int pos = x - mLeftOffset;

		pos = pos / (mTextWidth + cspacing);
		if (x < mLeftOffset + mTextWidth + cspacing) {
		    pos = 0;
		}
	    
		if ((mOffset + pos) < mAllTotal) {
		    Message msg = mHandler.obtainMessage(CandidateSelect.CHARACTER, mTable.getMatchChar(mOffset + pos), mOffset + pos);
		    mHandler.sendMessage(msg);
		}
	    } else if (mState == PHRASE_MODE) {
		int x = mLastX;

		// int start = (pspacing / 2), index = 0, count = 0, pos = -1, end;
		int start = 0, index = 0, count = 0, pos = -1, end;
		for (count = 0; count < mTotal; count++) {
		    int len = mTable.getPhraseArray(mOffset + count, mPhrase);
		    end = start + pspacing + (mTextWidth * len) + (mTextFontSpacing * (len - 1));
		    // if (count == 0) start = 0;
		    // Log.i("Cangjie", "Candidate Row " + start + " " + end + " " + len + " " + new String(mPhrase, 0, len) + " " + x);
		    if (x >= start && x < end) {
			pos = count;
			break;
		    }
		    start = end;
		}
		if (pos >= 0) {
		    // Log.i("Cangjie", "Candidate Row 0 " + pos);
		    int len = mTable.getPhraseArray(mOffset + pos, mPhrase);
		    Message msg = mHandler.obtainMessage(CandidateSelect.PHRASE, 0, mOffset + pos, new String(mPhrase, 0, len));
		    mHandler.sendMessage(msg);
		}
	    }
	}
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {

    	if (event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_MOVE) {
	    mLastX = -1;
	    mLastY = -1;
	    if (mState == CHARACTER_MODE) {
		int pos = (int) event.getX() - mLeftOffset;

		pos = pos / (mTextWidth + cspacing);
		if ((int) event.getX() < mLeftOffset + mTextWidth + cspacing) {
		    pos = 0;
		}

		// Log.i("Cangjie", "CandidateRow onTouch C " + (int) event.getX() + " " + mLeftOffset + " " + mTextWidth + " " + cspacing + " " + pos);

		mSelectIndex = pos;
	    } else if (mState == PHRASE_MODE) {
		int start = 0, index = 0, count = 0, pos = -1, end;
		for (count = 0; count < mTotal; count++) {
		    int len = mTable.getPhraseArray(mOffset + count, mPhrase);
		    end = start + pspacing + (mTextWidth * len) + (mTextFontSpacing * (len - 1));
		    // if (count == 0) start = 0;
		    // Log.i("Cangjie", "Candidate onTouch P " + count + " " + start + " " + end + " " + len + " " + new String(mPhrase, 0, len) + " " + (int) event.getX() + " " + mTextFontSpacing + " " + len + " " + mTextWidth + " " + pspacing);
		    // if ((int) event.getX() >= start && (int) event.getX() < end) {
		    if ((int) event.getX() < end) {
			pos = count;
			break;
		    }
		    start = end;
		}
		// Log.i("Cangjie", "CandidateRow onTouch P " + (int) event.getX() + " " + mLeftOffset + " " + mTextWidth + " " + cspacing + " " + pos);
		// Log.i("Cangjie", "Candidate Row 0 " + pos);
		if (pos >= 0) mSelectIndex = pos;
	    }
	    invalidate();
	} else if (event.getAction() == MotionEvent.ACTION_UP || event.getAction() == MotionEvent.ACTION_CANCEL) {
	    mLastX = (int) event.getX();
	    mLastY = (int) event.getY();

	    mSelectIndex = -1;
	    invalidate();
    	}
	
	// Log.i("Cangjie", "OnTouch " + event.getX() + " " + event.getY() + " " + mSelectIndex);
	
	return false;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	int desireWidth = resolveSize(mWidth, widthMeasureSpec);
	int desiredHeight = resolveSize(mPreferredHeight, heightMeasureSpec);

	mWidth  = desireWidth;
	mHeight = desiredHeight;

	setMeasuredDimension(desireWidth, desiredHeight);
    }

    private void calculateSpacingAndOffset() {
	if (mState == PHRASE_MODE) {
	    int total = 0;

	    mTextWidth = mRect.width();
	    mTextFontSpacing = mFontRect.width() - (2 * mTextWidth);
	    for (int count = 0; count < mTotal; count++) {
		int len = mTable.getPhraseArray(mOffset + count, mPhrase);
		total += len * mTextWidth + (len - 1) * mTextFontSpacing;
		// Log.i("Cangjie", "Len " + len + " " + total);
	    }

	    // cspacing = mRect.width() / 2;
	
	    // mLeftOffset = (mTotal * (int) mTextWidth) + ((mTotal - 0) * cspacing);

	    // mLeftOffset = (mWidth - mLeftOffset) / 2;

	    pspacing = (mWidth - total) / mTotal;
	    // Log.i("Cangjie", "Total " + total + " " + mTextWidth + " " + mTextFontSpacing + " " + mWidth + " " + pspacing);
	    
	    // mLeftOffset = (pspacing / 4);
	    mLeftOffset = 0;

	} else if (mState == CHARACTER_MODE) {
	    mTextWidth = mRect.width();
	    cspacing = (mWidth - mTextWidth / 2) / mTotal;

	    cspacing = cspacing - mTextWidth;

	    mLeftOffset = (mTextWidth / 4);
	}
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
	super.onDraw(canvas);
	if (canvas == null) return;
	
	if (mState == PHRASE_MODE && pspacing == 0)
	    calculateSpacingAndOffset();
	else if (mState == CHARACTER_MODE && cspacing == 0)
	    calculateSpacingAndOffset();

	canvas.save();
	canvas.clipRect(0, 0, mWidth, mHeight);

	mPaint.setColor(0xff3c3c3c);
	canvas.drawRect(0, 0, getWidth(), getHeight(), mPaint);
	mPaint.setColor(0xff282828);
	canvas.drawRect(0, 0, getWidth(), getHeight() - 1, mPaint);
	mPaint.setColor(getNormalColor());
	if (mTable != null) {
	    if (mState == CHARACTER_MODE) {
		char c[] = new char[1];
		int spacing = mLeftOffset + (cspacing / 2);
		int topOffset = (mRect.height() - mRect.bottom);
		topOffset = topOffset + ((mHeight - mRect.height()) / 2);
		for (int count = mOffset; count < mOffset + mTotal; count++) {
		    c[0] = mTable.getMatchChar(count);
		    int color = getNormalColor();
		    if (mTable.getFrequency(count) > 0) color = getFrequentColor();
		    mPaint.setColor(color);
		    if (mSelectIndex != (count - mOffset)) canvas.drawText(c, 0, 1, spacing, topOffset, mPaint);

		    if (count < (mOffset + mTotal - 1)) {
			mPaint.setColor(displaySeparator() ? 0xffcccccc : 0xff272727);
			canvas.drawLine(spacing + (cspacing / 2) + mTextWidth, 5,
					spacing + (cspacing / 2) + mTextWidth, getHeight() - 10, mPaint);
		    }

		    spacing += cspacing + mTextWidth;
		}
		if (mSelectIndex >= 0 && mSelectIndex < mTotal) {
		    spacing = mLeftOffset + mSelectIndex * (cspacing + mTextWidth);

		    // canvas.clipRect(spacing, 0, spacing + mTextWidth + cspacing, getHeight() - 1, Region.Op.REPLACE);
		    mPaint.setColor(getNormalColor());
		    canvas.drawRect(spacing, 0, spacing + mTextWidth + cspacing, getHeight() - 1, mPaint);

		    int color = 0xffffffff;
		    if (mTable.getFrequency(mOffset + mSelectIndex) > 0) color = getFrequentColor();
		    mPaint.setColor(color);
		    c[0] = mTable.getMatchChar(mOffset + mSelectIndex);
		    canvas.drawText(c, 0, 1, spacing + (cspacing / 2), topOffset, mPaint);

		    // canvas.clipRect(0, 0, mWidth, mHeight, Region.Op.REPLACE);
		    mPaint.setColor(0xffffffff);
		}
	    } else if (mState == PHRASE_MODE) {
		int count = 0;
		int spacing = (pspacing / 2);
		int topOffset = (mRect.height() - mRect.bottom);
		topOffset = topOffset + ((mHeight - mRect.height()) / 2);

		// Log.i("Cangjie", "CandidateRow 0 " + mOffset + " " + mTotal + " " + pspacing + " " + mOffset + " " + mTotal + " " + mTextWidth + " " + mTextFontSpacing);
		for (count = 0; count < mTotal; count++) {
		    int len = mTable.getPhraseArray(mOffset + count, mPhrase);
		    int mapoffset = (mOffset + count) - mTable.getPhraseIndex();
		    if (mTable.getPhraseFrequency(mOffset + count) > 0)
			mPaint.setColor(getFrequentColor());
		    else
			mPaint.setColor(getNormalColor());
		    canvas.drawText(mPhrase, 0, len, spacing, topOffset, mPaint);
		    spacing += (pspacing / 2) + (mTextWidth * len) + (mTextFontSpacing * (len - 1));
		    // canvas.clipRect(spacing - 1, 0, spacing + 1, getHeight(), Region.Op.REPLACE);
		    if (count < (mTotal - 1)) {
			mPaint.setColor(displaySeparator() ? 0xffcccccc : 0xff272727);
			canvas.drawLine(spacing, 5, spacing, getHeight() - 10, mPaint);
		    }
		    // canvas.clipRect(0, 0, mWidth, mHeight, Region.Op.REPLACE);
		    spacing += (pspacing / 2);
		}
	    }
	}
	canvas.restore();
    }

    public boolean displaySeparator() {
	return PreferenceManager.getDefaultSharedPreferences(context).getBoolean("show_candidate_separator", true);	
    }
    
}
