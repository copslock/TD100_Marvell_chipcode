package com.marvell.cmmb.manager;

import java.io.IOException;
import java.io.InputStream;
import java.io.Reader;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.InflateException;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.LayoutInflater.Factory;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.CMMBSettingActivity;
import com.marvell.cmmb.activity.EbMsgManageActivity;
import com.marvell.cmmb.activity.MBBMSSettingActivity;
import com.marvell.cmmb.activity.ProgramActivity;
import com.marvell.cmmb.activity.PurchaseManageActivity;
import com.marvell.cmmb.activity.RefreshManageActivity;
import com.marvell.cmmb.activity.SearchProgramActivity;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.SubMenuDialog;
import android.widget.Toast;
import android.media.AudioSystem;

/**
 * Inflat menu in different activity.
 * 
 */

public class MenuManager {
	private Context mContext;
	public static MenuManager sInstance;
	private MenuInflater mInflater;
	private SubMenuDialog mSubMenuDialog;// the dialog on MainActivity
	private Menu mMenu;

	private Handler mPlayHandler = null;// handler of PlayingActivity;
	private Handler mEbMsgHandler = null;
	private Toast mShortPromptToast = null;
	
	private static final int INDEX_SPEAKER = 3;

	public MenuManager(Context context) {
		mContext = context;
		init();

	}

	private void init() {
		mInflater = new MenuInflater(mContext);

	}

	public void InflatMenu(int menuRes, Menu menu) {

		setMenuBackground();
		mInflater.inflate(menuRes, menu);
		mMenu = menu;

	}

	public void setPlayHandler(Handler handler) {
		if (mPlayHandler == null) {
			mPlayHandler = handler;
		}

	}

	public void setEbMsgHandler(Handler handler) {
		if (mEbMsgHandler == null) {
			mEbMsgHandler = handler;
		}

	}

	protected void setMenuBackground() {
		((Activity) mContext).getLayoutInflater().setFactory(new Factory() {

			public View onCreateView(final String name, final Context context, final AttributeSet attrs) {
				if (name.equalsIgnoreCase("com.android.internal.view.menu.IconMenuItemView")) {
					try { // Ask our inflater to create the view
						final LayoutInflater f = ((Activity) mContext).getLayoutInflater();
						final View[] view = new View[1];
						try {
							view[0] = f.createView(name, null, attrs);
						} catch (InflateException e) {
							hackAndroid23(name, attrs, f, view);
						}
						// Kind of apply our own background
						new Handler().post(new Runnable() {
							public void run() {
								view[0].setBackgroundResource(R.drawable.menu_bg);
							}
						});
						return view[0];
					} catch (InflateException e) {
					} catch (ClassNotFoundException e) {

					}
				}
				return null;
			}
		});
	}

	static void hackAndroid23(final String name,
			final android.util.AttributeSet attrs, final LayoutInflater f,
			final View[] view) {
		// mConstructorArgs[0] is only non-null during a running call to
		// inflate()
		// so we make a call to inflate() and inside that call our dully
		// XmlPullParser get's called
		// and inside that it will work to call
		// "f.createView( name, null, attrs );"!
		try {
			f.inflate(new XmlPullParser() {
				public int next() throws XmlPullParserException, IOException {
					try {
						view[0] = f.createView(name, null, attrs);
					} catch (InflateException e) {
					} catch (ClassNotFoundException e) {
					}
					throw new XmlPullParserException("exit");
				}

				public void defineEntityReplacementText(String entityName,
						String replacementText) throws XmlPullParserException {
					// TODO Auto-generated method stub

				}

				public int getAttributeCount() {
					// TODO Auto-generated method stub
					return 0;
				}

				public String getAttributeName(int index) {
					// TODO Auto-generated method stub
					return null;
				}

				public String getAttributeNamespace(int index) {
					// TODO Auto-generated method stub
					return null;
				}

				public String getAttributePrefix(int index) {
					// TODO Auto-generated method stub
					return null;
				}

				public String getAttributeType(int index) {
					// TODO Auto-generated method stub
					return null;
				}

				public String getAttributeValue(int index) {
					// TODO Auto-generated method stub
					return null;
				}

				public String getAttributeValue(String namespace, String name) {
					// TODO Auto-generated method stub
					return null;
				}

				public int getColumnNumber() {
					// TODO Auto-generated method stub
					return 0;
				}

				public int getDepth() {
					// TODO Auto-generated method stub
					return 0;
				}

				public int getEventType() throws XmlPullParserException {
					// TODO Auto-generated method stub
					return 0;
				}

				public boolean getFeature(String name) {
					// TODO Auto-generated method stub
					return false;
				}

				public String getInputEncoding() {
					// TODO Auto-generated method stub
					return null;
				}

				public int getLineNumber() {
					// TODO Auto-generated method stub
					return 0;
				}

				public String getName() {
					// TODO Auto-generated method stub
					return null;
				}

				public String getNamespace() {
					// TODO Auto-generated method stub
					return null;
				}

				public String getNamespace(String prefix) {
					// TODO Auto-generated method stub
					return null;
				}

				public int getNamespaceCount(int depth)
						throws XmlPullParserException {
					// TODO Auto-generated method stub
					return 0;
				}

				public String getNamespacePrefix(int pos) throws XmlPullParserException {
					// TODO Auto-generated method stub
					return null;
				}

				public String getNamespaceUri(int pos) throws XmlPullParserException {
					// TODO Auto-generated method stub
					return null;
				}

				public String getPositionDescription() {
					// TODO Auto-generated method stub
					return null;
				}

				public String getPrefix() {
					// TODO Auto-generated method stub
					return null;
				}

				public Object getProperty(String name) {
					// TODO Auto-generated method stub
					return null;
				}

				public String getText() {
					// TODO Auto-generated method stub
					return null;
				}

				public char[] getTextCharacters(int[] holderForStartAndLength) {
					// TODO Auto-generated method stub
					return null;
				}

				public boolean isAttributeDefault(int index) {
					// TODO Auto-generated method stub
					return false;
				}

				public boolean isEmptyElementTag()
						throws XmlPullParserException {
					// TODO Auto-generated method stub
					return false;
				}

				public boolean isWhitespace() throws XmlPullParserException {
					// TODO Auto-generated method stub
					return false;
				}

				public int nextTag() throws XmlPullParserException, IOException {
					// TODO Auto-generated method stub
					return 0;
				}

				public String nextText() throws XmlPullParserException,
						IOException {
					// TODO Auto-generated method stub
					return null;
				}

				public int nextToken() throws XmlPullParserException,
						IOException {
					// TODO Auto-generated method stub
					return 0;
				}

				public void require(int type, String namespace, String name)
						throws XmlPullParserException, IOException {
					// TODO Auto-generated method stub

				}

				public void setFeature(String name, boolean state)
						throws XmlPullParserException {
					// TODO Auto-generated method stub

				}

				public void setInput(Reader in) throws XmlPullParserException {
					// TODO Auto-generated method stub

				}

				public void setInput(InputStream inputStream,
						String inputEncoding) throws XmlPullParserException {
					// TODO Auto-generated method stub

				}

				public void setProperty(String name, Object value)
						throws XmlPullParserException {
					// TODO Auto-generated method stub

				}
			}, null, false);
		} catch (InflateException e1) {
			// "exit" ignored
		}
	}

	public void onMenuClick(int menuId) {
		switch (menuId) {
		case R.id.menu_program:
			Intent programIntent = new Intent();
			programIntent.putExtra("Position", MBBMSService.sCurrentPlayPosition);
			programIntent.setClass(mContext, ProgramActivity.class);
			mContext.startActivity(programIntent);
			break;
		case R.id.menu_rescan:
			MBBMSService.sInstance.startScan();
			break;
		case R.id.menu_subscribe:
			if (MBBMSService.sInstance != null) {
				if (MBBMSService.sInstance.isOperationFinish()) {
					Intent purchaseIntent = new Intent();
					purchaseIntent.setClass(mContext, PurchaseManageActivity.class);
					purchaseIntent.putExtra("SubscribeStatus", Define.PURCHASED);
					mContext.startActivity(purchaseIntent);
				} else {
					this.getShortPromptToast(R.string.waitsync).show();
				}
			}

			break;
		case R.id.menu_search:
			Intent search = new Intent();
			search.setClass(mContext, SearchProgramActivity.class);
			mContext.startActivity(search);
			break;
		case R.id.menu_cmmb_setting:
			Intent setting = new Intent();
			setting.setClass(mContext, CMMBSettingActivity.class);
			mContext.startActivity(setting);
			break;
		case R.id.menu_ebmsg:
			Intent ebmsg = new Intent();
			ebmsg.setClass(mContext, EbMsgManageActivity.class);
			mContext.startActivity(ebmsg);
			break;
		case R.id.menu_more:
			mSubMenuDialog = new SubMenuDialog(mContext);
			mSubMenuDialog.show();
			break;
		case R.id.menu_fullscreen:
			if (mPlayHandler != null) {
				mPlayHandler.sendEmptyMessage(Define.MESSAGE_CHANGE_FULL_SCREEN);
			}

			break;
		case R.id.menu_delall:// delete all ebmsg
			if (mEbMsgHandler != null) {
				mEbMsgHandler.sendEmptyMessage(Define.DIALOG_CONFIRM_EMERG_DELETE_ALL);
			}
			break;
		case R.id.menu_mbbs_setting:
			Intent intent = new Intent();
			intent.setClass(mContext, MBBMSSettingActivity.class);
			mContext.startActivity(intent);
			break;

		case R.id.menu_refresh:
			if (MBBMSService.sInstance != null) {
				if (MBBMSService.sInstance.isOperationFinish()) {
					Intent refresh = new Intent();
					refresh.setClass(mContext, RefreshManageActivity.class);
					mContext.startActivity(refresh);
				} else {
					this.getShortPromptToast(R.string.waitsync).show();
				}
			}

			break;
		case R.id.menu_speaker:	
		if(MBBMSService.sInstance != null) {
			if(MBBMSService.sInstance.isSpeakerOn == false) {
				MBBMSService.sInstance.setForceUseSpeaker();
				updateMenu(INDEX_SPEAKER, R.string.headphone);
				updateIcon(INDEX_SPEAKER, R.drawable.icon_headphone);
			}else {
				MBBMSService.sInstance.setForceUseHeadPhone();
				updateMenu(INDEX_SPEAKER, R.string.speaker);
				updateIcon(INDEX_SPEAKER, R.drawable.icon_speaker);
			}
			
		}
			break;
		default:
			break;
		}

	}

	public void updateMenu(int index, int resId) {
		if (mMenu != null) {
			int n = mMenu.size();
			if (index < n) {
				MenuItem item = mMenu.getItem(index);
				item.setTitle(resId);
			}
		}
	}

	public Toast getShortPromptToast(int resid) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(mContext, resid, Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(resid);
		return mShortPromptToast;
	}
		
	public void setEnabled(int index, boolean isEnabled){
		if(mMenu!=null){
			int n = mMenu.size();
			if (index < n) {
				MenuItem item = mMenu.getItem(index);
				item.setEnabled(isEnabled);
			}
		}		
	}
	
	public void updateIcon(int index, int iconId ) {
		if (mMenu != null) {
			int n = mMenu.size();
			if (index < n) {
				MenuItem item = mMenu.getItem(index);
				item.setIcon(iconId);
			}
		}
	}	
	public boolean isMenuInflated() {
		if(mMenu != null) {
			return true;
		}else {
			return false;
		}		
	}

}
