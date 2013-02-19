package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;
import android.content.Context;
import android.app.Activity;
import android.util.AttributeSet;
import android.view.*;
import android.widget.*;
import android.graphics.*;
import android.util.Log;
import android.os.Handler.Callback;
import android.os.Handler;
import android.os.Message;
import android.view.ViewGroup.LayoutParams;
import android.view.ViewGroup.MarginLayoutParams;

public class CandidateSelect extends View implements Handler.Callback {

    public static final int CHARACTER      = 0;
    public static final int CHARACTER_MODE = 0;
    public static final int PHRASE_MODE    = 1;
    
    private int width = 0;
    private int height = 0;
    private int total = 0;
    private Paint paint = null;
    private float textWidth = 0.0f;
    private int offset = 0;
    private int charOffset = 0;
    private int spacing = 16;
    private int mParentWidth = 0;
    private float charWidth = 0;
    private int topOffset = 0;
    private float mFontSize = 50.0f;
    private Context context = null;
    private PopupWindow mPopup = null;
    private Handler mHandler = null;
    private Rect mRect = new Rect();
    private int mSelectIndex = -1;
    private TableLoader mTable = null;

    private int mState = CHARACTER_MODE;
    private StringBuffer mPhraseBuffer = new StringBuffer();
    private char[] mPhraseArray = null;
    private CandidateAdapter mAdapter = null;
    private View mPopupView = null;

    private final static int SPACING            = 4;
    private final static int STARTING_FONT_SIZE = 12;
    private final static int ENDING_FONT_SIZE   = 128;

    private CandidateListener listener = null;
    
    public static interface CandidateListener {
	void characterSelected(char c, int idx);
    }
    
    public CandidateSelect(Context context, AttributeSet attrs) {
	super(context, attrs);

	this.context = context;

	mState = CHARACTER_MODE;
	
	mFontSize = 50.0f;
	paint = new Paint();
	paint.setColor(Color.BLACK);
	paint.setAntiAlias(true);
	paint.setTextSize(mFontSize);
	paint.setStrokeWidth(0);

	paint.getTextBounds(context.getString(R.string.cangjie), 0, 1, mRect);
	textWidth = mRect.width();
	spacing   = (int) textWidth / 2;

	mHandler = new Handler(this);
    }

    public void setParentWidth(int w) {
	mParentWidth = w;

	int columnc = mParentWidth / ((int) textWidth + spacing);
	offset = columnc * ((int) textWidth + spacing);
	offset = (mParentWidth - offset) / 2;

    }
    
    public void setCandidateListener(CandidateListener listen) {
	listener = listen;
    }

    private class CandidateItem {
    }
    
    public class CandidateAdapter extends ArrayAdapter<CandidateItem> {

	private Context context    = null;
	private TableLoader  table = null;
	private int     total      = 0;
	private int     layoutRes  = 0;
	private float   fontSize   = 0.0f;
	private int     topOffset  = 0;
	private int     columnc    = 0;

	public CandidateAdapter(Context context, int layoutRes, CandidateItem[] row, int columnc, int total, float fs, int to) {
	    super(context, layoutRes, row);
	    this.context    = context;
	    this.layoutRes  = layoutRes;
	    this.total      = total;
	    this.fontSize   = fs;
	    this.topOffset  = to;
	    this.columnc    = columnc;
	}

	public CandidateAdapter(Context context, int layoutRes, CandidateItem[] row, TableLoader _table, int columnc, int total, float fs, int to) {
	    super(context, layoutRes, row);
	    this.context    = context;
	    this.table      = _table;
	    this.layoutRes  = layoutRes;
	    this.total      = total;
	    this.fontSize   = fs;
	    this.topOffset  = to;
	    this.columnc    = columnc;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
	    View row = convertView;
	    CandidateHolder holder = null;

	    if (row == null) {
		LayoutInflater inflater = (LayoutInflater)
		    context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		row = inflater.inflate(layoutRes, parent, false);

		holder = new CandidateHolder();
		holder.row = (CandidateRow) row.findViewById(R.id.candidateRow);
		row.setTag(holder);
	    } else {
		holder = (CandidateHolder) row.getTag();
	    }

	    holder.row.setHandler(mHandler);
	    holder.row.setFontSize(fontSize, topOffset);
	    holder.row.setMatch(position * columnc, total - (position * columnc) >= columnc ? columnc : (total - (position * columnc)), total);

	    return row;
	}

	class CandidateHolder {
	    CandidateRow row;
	}
	
    }

    public void closePopup() {
	if (mPopup == null) return;
	mPopup.dismiss();
	mPopup = null;
    }
    
    public boolean handleMessage(Message msg) {

	if (msg.what == CHARACTER) {
	    closePopup();
	    if (listener != null && msg.arg1 != 0) listener.characterSelected((char) msg.arg1, msg.arg2);
	}
	
	return true;
    }

    public void showCandidatePopup(View mParent, int w, int h) {
	if (total == 0) return;
	if (mState != CHARACTER_MODE)
	    return;

	if (mPopup == null) {
	    int columnc = w / ((int) textWidth + spacing);

	    int rowc = total / columnc;
	    if ((total % columnc) > 0) rowc++;

	    if (mPopupView == null) {
		LayoutInflater inflate = (LayoutInflater)
		    context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		mPopupView = inflate.inflate(R.layout.popup, null);

		CloseButton mButton = (CloseButton) mPopupView.findViewById(R.id.cancelButton);
		mButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
			    closePopup();
			}
		    });
	    }

	    CandidateItem[] row = new CandidateItem[rowc];
	    mAdapter = new CandidateAdapter(context, R.layout.candidate, row, mTable, columnc, total, mFontSize, topOffset);

	    ListView lv = (ListView) mPopupView.findViewById(R.id.sv);
	    lv.setAdapter(mAdapter);

	    mPopup = new PopupWindow(context);
	    
	    mPopup.setContentView(mPopupView);
	    mPopup.setWidth(w);
	    mPopup.setHeight(h);
	    mPopup.setSoftInputMode(PopupWindow.INPUT_METHOD_NOT_NEEDED);
	    mPopup.setTouchable(true);

	    mPopup.showAsDropDown(this, 0, 0);
	} else {
	    mPopup.dismiss();
	    mPopup = null;
	}

    }

    public float fontSize() {
	return mFontSize;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
	super.onSizeChanged(w, h, oldw, oldh);

	width  = w;
	height = h;

	for (int fontsize = STARTING_FONT_SIZE; fontsize < ENDING_FONT_SIZE; fontsize += 2) {
	    paint.setTextSize(fontsize);

	    Paint.FontMetrics metrics = paint.getFontMetrics();
	    int totalHeight = (int) (metrics.bottom - metrics.top);

	    if (totalHeight > height) {
		mFontSize = (float) (fontsize - 8);
		paint.setTextSize(mFontSize);
		paint.getTextBounds(context.getString(R.string.cangjie), 0, 1, mRect);
		textWidth = mRect.width();
		spacing   = (int) textWidth / 2;

		topOffset = mRect.height() - mRect.bottom;
		topOffset += (h - mRect.height()) / 2;

		break;
	    }
	}

    }

    @Override
    protected void onDraw(Canvas canvas) {
	super.onDraw(canvas);
	if (canvas == null || mTable == null) {
	    return;
	}

	if (mState == CHARACTER_MODE) {
	    char c[] = new char[1];
	    paint.setColor(0xff282828);
	    canvas.drawRect(0, 0, width, height - 0, paint);
	    paint.setColor(0xff33B5E5);
	
	    int start = offset + (spacing / 2), index = charOffset;
	    while (start < width && index < total) {
		if (mSelectIndex == index) {
		    int x = start - (spacing / 2);
		    paint.setColor(0xff33B5E5);
		    canvas.drawRect(x, 0, x + textWidth + spacing, height, paint);
		    c[0] = mTable.getMatchChar(index);
		    int color = 0xff282828;
		    if (mTable.getFrequency(index) > 0) color = 0xffff9000;
		    paint.setColor(color);
		    canvas.drawText(c, 0, 1, start, topOffset, paint);
		    paint.setColor(0xff33B5E5);
		} else {
		    c[0] = mTable.getMatchChar(index);
		    int color = 0xff33B5E5;
		    if (mTable.getFrequency(index) > 0) color = 0xffff9000;
		    paint.setColor(color);
		    canvas.drawText(c, 0, 1, start, topOffset, paint);
		}
		start = start + (int) textWidth + spacing;
		index++;
	    }
	} else if (mState == PHRASE_MODE) {
	    if (mPhraseArray == null) {
		mPhraseArray = new char[mTable.getPhraseMax()];
	    }

	    Log.i("Cangjie", "Phrase Mode Candidiate Select 0");
	    int plen = 0;
	    int start = (spacing / 2);
	    int index = 0;

	    paint.setColor(0xff282828);
	    canvas.drawRect(0, 0, width, height - 0, paint);
	    paint.setColor(0xff33B5E5);

	    while (start < width && index < mTable.getPhraseCount()) {
		plen = mTable.getPhraseArray(mTable.getPhraseIndex() + index, mPhraseArray);
		canvas.drawText(mPhraseArray, 0, plen, start, topOffset, paint);
		start += (plen * textWidth) + spacing;
		paint.setColor(0xffcccccc);
		canvas.drawLine(start, 5, start, height - 10, paint);
		paint.setColor(0xff33B5E5);
		start += (spacing / 2);
		index++;
	    }
	}
    }

    @Override
    public boolean onTouchEvent(MotionEvent me) {
	int action = me.getAction();
	int x = (int) me.getX();
	int y = (int) me.getY();
	int select = x - offset;
	int left = 0;
	char c = 0;
	int idx = -1;

	for (int count = charOffset; count < total; count++) {
	    if (count == charOffset) {
		if (select < (left + (int) textWidth + spacing)) {
		    c = mTable.getMatchChar(count);
		    idx = count;
		    break;
		}
	    } else if (select > left && select < (left + (int) textWidth + spacing)) {
		c = mTable.getMatchChar(count);
		idx = count;
		break;
	    }
	    left = left + (int) textWidth + spacing;
	}

	switch (action) {
	case MotionEvent.ACTION_DOWN:
	case MotionEvent.ACTION_MOVE:
	    mSelectIndex = idx;
	    invalidate();
	    break;
	case MotionEvent.ACTION_UP:
	    if (listener != null && c != 0 && mSelectIndex == idx) {
		listener.characterSelected(c, idx);
	    }
	case MotionEvent.ACTION_CANCEL:
	    mSelectIndex = -1;
	    invalidate();
	    break;
	}

	return true;
    }

    public void showNextPage() {
	if (mTable == null) return;
	if (total > 1 && charOffset < (total - 1)) {
	    charOffset++;
	    invalidate();
	}
    }
    
    public void showPrevPage() {
	if (mTable == null) return;
	if (charOffset > 0) {
	    charOffset--;
	    invalidate();
	}
    }
    
    public void updateTable(TableLoader loader) {
	mState = CHARACTER_MODE;
	mTable = loader;
	if (loader == null) total = 0; else total = loader.totalMatch();
        charOffset = 0;
	invalidate();
    }

    public void updatePhrase(TableLoader loader) {
	mState = PHRASE_MODE;
	mTable = loader;
	invalidate();
    }

}
