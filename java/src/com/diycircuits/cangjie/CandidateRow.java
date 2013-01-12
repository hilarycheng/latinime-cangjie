package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;
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

    private static Paint mPaint = null;
    private static Rect mRect = new Rect();
    private static int cspacing = 0;
    private static int mLeftOffset = 0;
    private static int mTextWidth = 0;
    private int mWidth = 100;
    private int mHeight = 64;
    private char[] mMatch = null;
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
    private TableLoader mTable = null;

    public CandidateRow(Context context, AttributeSet attrs) {
	super(context, attrs);

	this.context = context;

	setupPaint();

	setClickable(true);
	setOnClickListener(this);
	setOnTouchListener(this);
    }

    private synchronized void setupPaint() {
	if (mPaint == null) {
	    mPaint = new Paint();
	    mPaint.setColor(Color.BLACK);
	    mPaint.setAntiAlias(true);
	    mPaint.setTextSize(mFontSize);
	    mPaint.setStrokeWidth(0);
    	    mPaint.getTextBounds(context.getString(R.string.cangjie), 0, 1, mRect);
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
	    }
	}
    }
    
    public void setMatch(char[] match, int offset, int total, int alltotal) {
	mMatch  = match;
	mOffset = offset;
	mTotal  = total;
	if (mAllTotal != alltotal) cspacing = 0;
	mAllTotal = alltotal;
    }
    
    public void setMatch(TableLoader table, int offset, int total, int alltotal) {
	mTable = table;
	mOffset = offset;
	mTotal  = total;
	if (mAllTotal != alltotal) cspacing = 0;
	mAllTotal = alltotal;
    }
    
    @Override
    public void onClick(View v) {
	if (mLastX != -1 && mLastY != -1) {
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
	}
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {

    	if (event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_MOVE) {
	    mLastX = -1;
	    mLastY = -1;
	    int pos = (int) event.getX() - mLeftOffset;

	    pos = pos / (mTextWidth + cspacing);
	    if ((int) event.getX() < mLeftOffset + mTextWidth + cspacing) {
		pos = 0;
	    }

	    mSelectIndex = pos;
	    invalidate();
	} else if (event.getAction() == MotionEvent.ACTION_UP || event.getAction() == MotionEvent.ACTION_CANCEL) {
	    mLastX = (int) event.getX();
	    mLastY = (int) event.getY();

	    mSelectIndex = -1;
	    invalidate();
    	}
	
	return false;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	int desireWidth = resolveSize(mWidth, widthMeasureSpec);
	int desiredHeight = resolveSize(mHeight, heightMeasureSpec);

	mWidth  = desireWidth;
	mHeight = desiredHeight;

	setMeasuredDimension(desireWidth, desiredHeight);
    }

    private void calculateSpacingAndOffset() {
	cspacing = mRect.width() / 2;
	mTextWidth = mRect.width();
	
	mLeftOffset = (mTotal * (int) mTextWidth) + ((mTotal - 0) * cspacing);

	mLeftOffset = (mWidth - mLeftOffset) / 2;
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
	if (canvas == null) return;
	
	if (cspacing == 0) calculateSpacingAndOffset();

	mPaint.setColor(0xff3c3c3c);
	canvas.drawRect(0, 0, getWidth(), getHeight(), mPaint);
	mPaint.setColor(0xff282828);
	canvas.drawRect(0, 0, getWidth(), getHeight() - 1, mPaint);
	mPaint.setColor(0xffeeeeee);
	if (mTable != null) {
	    char c[] = new char[1];
	    int spacing = mLeftOffset + (cspacing / 2);
	    int topOffset = (mRect.height() - mRect.bottom);
	    topOffset = topOffset + ((mHeight - mRect.height()) / 2);
	    for (int count = mOffset; count < mOffset + mTotal; count++) {
		c[0] = mTable.getMatchChar(count);
		int color = 0xffeeeeee;
		if (mTable.getFrequency(count) > 0) color = 0xffff9000;
		mPaint.setColor(color);
		if (mSelectIndex != (count - mOffset)) canvas.drawText(c, 0, 1, spacing, topOffset, mPaint);
		spacing += cspacing + mTextWidth;
	    }
	    if (mSelectIndex >= 0) {
		spacing = mLeftOffset + mSelectIndex * (cspacing + mTextWidth);

		canvas.clipRect(spacing, 0, spacing + mTextWidth + cspacing, mHeight - 1, Region.Op.REPLACE);
		mPaint.setColor(0xff33B5E5);
		canvas.drawRect(spacing, 0, spacing + mTextWidth + cspacing, mHeight - 1, mPaint);

		int color = 0xffffffff;
		if (mTable.getFrequency(mOffset + mSelectIndex) > 0) color = 0xffff9000;
		mPaint.setColor(color);
		c[0] = mTable.getMatchChar(mOffset + mSelectIndex);
		canvas.drawText(c, 0, 1, spacing + (cspacing / 2), topOffset, mPaint);

		canvas.clipRect(0, 0, mWidth, mHeight, Region.Op.REPLACE);
		mPaint.setColor(0xffffffff);
	    }
	}
    }

}
