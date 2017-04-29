/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Aric
 **  Create Date Aug 29 2011      
 */
package com.marvell.logtools;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class NVMEditorService extends Service {
    private final String LOGTAG = "NVMEditorService";
    
    private final String NVMFolder = "/data/Linux/Marvell/NVM";
    private final String NVMFileName = "EEHandlerConfig_Linux.nvm";
    private static final int IO_BUFFER_SIZE = 4 * 1024;

    private static File mNVMFile;
    private static boolean mExist;
    private static BufferedReader mBuf;
    
    private char mNVMstruct[] = null;
    private char mheader[] = new char[264];
    
    private NVMEditorBinder mBinder = new NVMEditorBinder();
    
    public int getIntFromBytes(char[] charArray)
    {
        int ret = 0;
        ret |= charArray[3] & 0xFF;
        ret <<= 8;
        ret |= charArray[2] & 0xFF;
        ret <<= 8;
        ret |= charArray[1] & 0xFF;
        ret <<= 8;
        ret |= charArray[0] & 0xFF;
        
        return ret;
    }
    
    private boolean getNVMStruct() {
        mNVMFile = new File(NVMFolder, NVMFileName);
        if (!mNVMFile.exists()) {
            Log.w(LOGTAG, "File " + mNVMFile + " doesn't exist!");
            mExist = false;
            return false;
            
        } else {
            try {
                mBuf = new BufferedReader(new InputStreamReader(new FileInputStream(mNVMFile),"ISO-8859-1"),
                        IO_BUFFER_SIZE);
            }catch(FileNotFoundException ex)
            {
                Log.w(LOGTAG, "File " + mNVMFile + " Can not Open!");
                mExist = false;
                return false;
            }catch( UnsupportedEncodingException ex)
            {
                Log.w(LOGTAG, "File " + mNVMFile + " Can not Open!");
                mExist = false;
                return false;
            }
            finally {
                mExist = true;
                Log.i(LOGTAG, "File " + mNVMFile + " is loaded in buf");                
            }
            
        }       
        
        //read files and save value
        if(mExist)
        {
            //read NVM files
            //NVM_Header_ts,,,,,0,0,,
            //NVM_Header_ts,,StructSize,unsigned,unsigned long,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,NumofStructs,unsigned,unsigned long,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,StructName,char,char,64,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,Date,char,char,32,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,time,char,char,32,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,Version,char,char,64,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,HW_ID,char,char,32,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            //NVM_Header_ts,,CalibVersion,char,char,32,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
            
            //header size is 4+4+64+32+32+64+32+32 = 264
            
            int readlen;
            int structSize, structNum;
            
            try{
                readlen = mBuf.read(mheader,0,264);
                
                if(readlen != 264)
                {
                    Log.w(LOGTAG, "NVM header read error!");
                    mBuf.close();      
                    return false;
                }
                
                //read struct size
                structSize = getIntFromBytes(mheader);
                structNum = 1; //TODO: parser it
                
                Log.i(LOGTAG, "structSize is " + structSize );
                
                if(structSize > 0  && structNum > 0)
                {
                    mNVMstruct = new char[structSize*structNum];
                    
                    readlen = mBuf.read(mNVMstruct,0,structSize*structNum);
                    
                    if(readlen != structSize*structNum)
                    {
                        Log.w(LOGTAG, "NVM Struct read error!");
                        mBuf.close();
                        return false;

                    }
                
                    //EE_Configuration_t,,,,,0,0,,
                    //EE_Configuration_t,,AssertHandlerOn,EE_OnOff_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,ExcepHandlerOn,EE_OnOff_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,WarningHandlerOn,EE_WarningOn_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,powerUpLogOn,EE_OnOff_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,extHandlersOn,EE_OnOff_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,printRecentLogOnStartup,EE_OnOff_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,finalAction,FinalAct_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,EELogFileSize,UINT32,unsigned long,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,delayOnStartup,UINT16,unsigned short,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,assertContextBufType,enum,EE_ContextType_t,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,exceptionContextBufType,enum,EE_ContextType_t,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,warningContextBufType,enum,EE_ContextType_t,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_DeferredCfg_t,EE_Configuration_t,,,deferredCfg,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,wdtConfigTime,EE_WdtTimeCfg_t,unsigned short,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,sysEeHandlerLimit,UINT16,unsigned short,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,dumpDdrSizeBytes,UINT32,unsigned long,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,finalActionAP,FinalAct_t,unsigned char,1,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    //EE_Configuration_t,,reserved,UINT8,unsigned char,23,0,,@ITEM_MODE@@ITEM_UNIT@@ITEM_DEPENDENCY@
                    mBuf.close();
                    return true;
                }
                else
                {
                    Log.w(LOGTAG, "NVM Struct empty!");
                    mBuf.close();
                    return false;
                }                   
                
            }catch(IOException ex)
            {
                Log.w(LOGTAG, "NVM File read error!");
                return false;
            }
          } else {
              Log.i(LOGTAG, "NVM File not exist");
              return false;
          }
    }
    
    @Override
    public IBinder onBind(Intent intent) {
        Log.d(LOGTAG, "onBind");
        return mBinder;
    }
    
    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(LOGTAG, "onUnbind");
        return super.onUnbind(intent);
    }
    
    public class NVMEditorBinder extends Binder {
        NVMEditorService getService() {
            return NVMEditorService.this;
        }
    }
    
    public String getFinalAct() {
        
          if(getNVMStruct()) {          
              //finalAction is at 1+1+1+1+1+1 = 6
              char finalAction = mNVMstruct[6];

              //FinalAct_t_,EE_RESET,0,
              //FinalAct_t_,EE_STALL,1,
              //FinalAct_t_,EE_RESERVED,2,
              //FinalAct_t_,EE_EXTERNAL,3,
              //FinalAct_t_,EE_RESET_START_BASIC,4,
              //FinalAct_t_,EE_RESET_COMM_SILENT,5,

              //EE_RESERVED/EE_RESET_START_BASIC/EE_RESET_FULL_UI not support now
              //2,4,7 never happen. Keep code for easy expansion
              switch(finalAction)
              {
                  case 0:
                      Log.i(LOGTAG, "finalAction old setting is EE_RESET");
                      return "0";
                  case 1:
                      Log.i(LOGTAG, "finalAction old setting is EE_STALL");
                      return "1";
                  case 2:
                      Log.i(LOGTAG, "finalAction old setting is EE_RESERVED");
                      return "2";
                  case 3:
                      Log.i(LOGTAG, "finalAction old setting is EE_EXTERNAL");
                      return "3";
                  case 4:
                      Log.i(LOGTAG, "finalAction old setting is EE_RESET_START_BASIC");
                      return "4";
                  case 5:
                      Log.i(LOGTAG, "finalAction old setting is EE_RESET_COMM_SILENT");
                      return "5";
                  case 6:
                      Log.i(LOGTAG, "finalAction old setting is EE_RESET_GPIO_RESET");
                      return "6";
                  case 7:
                      Log.i(LOGTAG, "finalAction old setting is EE_RESET_FULL_UI");
                      return "7";
                  case 8:
                      Log.i(LOGTAG, "finalAction old setting is EE_DUMP");
                      return "8";   
                  default:
                      Log.i(LOGTAG, "finalAction old setting is unknown" + finalAction);
                      return "0";
              }
          } else {
           // stop service until next wakeup
              Log.i(LOGTAG, "stopping self");
              stopSelf(); 
              return "0";
          }                 
        }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(LOGTAG, ": onStart" + " flags is " + flags );
        super.onStartCommand(intent, flags, startId);
        
        int startFlag = intent.getFlags();
        Log.i(LOGTAG, "start flags is " + startFlag );
        
        CharSequence text;
        int duration = Toast.LENGTH_LONG;
        //try to save new setting
            /*if(startFlag == 7450)
                mNVMstruct[6] = 5;
            else
                mNVMstruct[6] = 1;*/
        if(getNVMStruct()) {
            
            switch(startFlag - 7450){
            case 0:
                mNVMstruct[6] = 0;
                text = "Final Action is set to EE Reset!";
                break;
            case 1:
                mNVMstruct[6] = 1;
                text = "Final Action is set to ST All!";
                break;
            //not support now, UI can't show this option
            case 2:
                mNVMstruct[6] = 2;
                text = "Final Action is set to EE Reserved!";
                break;
            case 3:
                mNVMstruct[6] = 3;
                text = "Final Action is set to EE External!";
                break;
            //not support now, UI can't show this option
            case 4:
                mNVMstruct[6] = 4;
                text = "Final Action is set to EE Reset Start Basic!";
                break;
            case 5:
                mNVMstruct[6] = 5;
                text = "Final Action is set to EE Reset Comm Silent";
                break;
            case 6:
                mNVMstruct[6] = 6;
                text = "Final Action is set to EE Reset GPIO Reset";
                break;
            //not support now, UI can't show this option
            case 7:
                mNVMstruct[6] = 7;
                text = "Final Action is set to EE Reset Full UI";
                break;
            case 8:
                mNVMstruct[6] = 8;
                text = "Final Action is set to EE RamDump";
                break;
            default:
                text = "Final Action value is not correct!";
                break;
            }
            
            OutputStreamWriter mWriter;
            
            try {
                mWriter = new OutputStreamWriter(new FileOutputStream(mNVMFile), "ISO-8859-1");
            
                mWriter.write(mheader);
                mWriter.write(mNVMstruct);
                mWriter.flush();
                mWriter.close();
                
                Context context = getApplicationContext();
                /*if(startFlag == 7450)
                    text = "Final Action is set to Silent Reset!";
                else
                    text = "Final Action is set to STALL!";
                */            
                Toast toast = Toast.makeText(context, text, duration);
                toast.show();

            }catch(FileNotFoundException ex)
            {
                Log.w(LOGTAG, "File " + mNVMFile + " Can not Found!");

            }catch( IOException ex)
            {
                Log.w(LOGTAG, "File " + mNVMFile + " Can not Write!");
                
            }
        }               
        // stop service until next wakeup
        Log.i(LOGTAG, "stopping self");
        stopSelf();   
        return 0;
    }
    
}
