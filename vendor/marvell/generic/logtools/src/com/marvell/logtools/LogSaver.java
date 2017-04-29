/*****************************
 ** (C) Copyright 2011 Marvell International Ltd.
 ** All Rights Reserved
 ** Author Linda
 ** Create Date Jul 28 2011
 */
package com.marvell.logtools;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.content.Context;
import android.util.Log;

public class LogSaver {
    static final boolean DBG = true;
    static final String TAG = "LogSaver";
    static final SimpleDateFormat LOG_FILE_FORMAT = new SimpleDateFormat(
            "yyyy-MM-dd-HH-mm-ssZ");

    private Context mContext;
    private Prefs mPrefs;
    private LogDumper mLogDumper;

    public LogSaver(Context context) {
        mContext = context;
        mPrefs = new Prefs(mContext);

        mLogDumper = new LogDumper(mContext);
    }

    public File save() {
        final File path = new File("/sdcard/Mlog");
        final File file = new File(path + "/alogcat."
                + LOG_FILE_FORMAT.format(new Date()) + ".txt");

        String msg = "saving log to: " + file.toString();
        Log.d("alogcat", msg);

        new Thread(new Runnable() {
            public void run() {
                String dump = mLogDumper.dump(false);

                if (!path.exists()) {
                    path.mkdir();
                }

                BufferedWriter bw = null;
                try {
                    file.createNewFile();
                    bw = new BufferedWriter(new FileWriter(file), 1024);
                    bw.write(dump);
                } catch (IOException e) {
                    Log.e("alogcat", "error saving log", e);
                } finally {
                    if (bw != null) {
                        try {
                            bw.close();
                        } catch (IOException e) {
                            Log.e("alogcat", "error closing log", e);
                        }
                    }
                }
            }
        }).start();
        
        return file;
    }

    public boolean saveall() {
        if(DBG)
            Log.d(TAG, "start saveall");
        File path = new File("/sdcard/Mlog/All" + LOG_FILE_FORMAT.format(new Date()));
        File cpAssertPath = new File(path + "/CpAssert");
        File androidPath = new File(path + "/androidLog");
        File stackTracePath = new File(path + "/stackTrace");
        File powerRegDumpPath = new File(path + "/power"); 
        File tombstonesPath = new File(path +"/tombstones");
        try {                                   
            copyAndroidLogs(androidPath);
            copyDumpState(androidPath);
            copyKernelLogs(androidPath);
            copyStackTraceLogs(stackTracePath);
            copyPowerRegDump(powerRegDumpPath);
            copyCpAssertLogs(cpAssertPath);
            copyTombstonesLogs(tombstonesPath);

        } catch (Exception e) {
            Log.e(TAG, "saveall exception:" + e);
            e.printStackTrace();
        } 
        if(DBG)
            Log.d(TAG, "complete saveall");
        return true;
    }
    
    private void copyPowerRegDump(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyPowerRegDump");
        File regdumpFile = new File("/sys/power/regdump");
        if(regdumpFile.exists()) {
            try {
                if(!TargetPath.exists())
                    TargetPath.mkdirs();
                //copyFile(regdumpFile, new File(TargetPath+"/"+regdumpFile.getName()));
                copyFileToPath(regdumpFile, TargetPath.getPath());
            } catch (IOException e) {
                Log.e(TAG, "copyPowerRegDump exception:" + e);
                e.printStackTrace();
            }
        } else {
            //do nothing, there is no power reg dump
            if(DBG)
                Log.d(TAG, "There is no power reg dump");
        }
        if(DBG)
            Log.d(TAG, "start copyPowerRegDump");
    }
    
    private void copyCpAssertLogs(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyCpAssertLogs");
        File cpAssertFile = new File("/data/com_DDR_RW.bin");
        if(cpAssertFile.exists()){
            String sourcePath = "/data/Linux/Marvell/NVM";
            File out_file = new File(sourcePath);
            File[]n_out = out_file.listFiles();
            if(DBG)
                Log.d(TAG, "copyCpAssertLogs n_out length" + n_out.length);
            for (int i = 0; i < n_out.length; i++) {
                if (n_out[i].isFile()) {
                    try {
                        if(!TargetPath.exists())
                            TargetPath.mkdirs();
                        //copyFile(n_out[i],new File(TargetPath+"/"+n_out[i].getName()));
                        copyFileToPath(n_out[i], TargetPath.getPath());
                    } catch (IOException e) {
                        Log.e(TAG, "copyCpAssertLogs exception1: " + e);
                        e.printStackTrace();
                    }
                }
               if (n_out[i].isDirectory()) {
                   String sourceDir=sourcePath+n_out[i].getName()+"/";
                   String targetDir=TargetPath+"/"+n_out[i].getName();
                   try {
                        copyDirectory(sourceDir, targetDir);
                    } catch (IOException e) {
                        Log.e(TAG, "copyCpAssertLogs exception2: " + e);
                        e.printStackTrace();
                    }
                }
            }
            try {
                //copyFile(cpAssertFile, new File(TargetPath+"/"+cpAssertFile.getName()));
                copyFileToPath(cpAssertFile, TargetPath.getPath());
            } catch (IOException e) {
                Log.e(TAG, "copyCpAssertLogs exception3: " + e);
                e.printStackTrace();
            }
        } else {
            //do nothing, there is no CpAssertFile
            if(DBG)
                Log.d(TAG, "There is no Cp Assert Files");
        }
        
        if(DBG)
            Log.d(TAG, "complete copyCpAssertLogs");
        
    }
    
    private void copyAndroidLogs(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyAndroidLogs");
        File androidLogsPath = new File("/data/Mlog");
        if(androidLogsPath.exists()){
            if(DBG)
                Log.d(TAG, "androidLogsPath is exist");
            mLogDumper.nativeExecCommands(new String[]{"chmod", "755", androidLogsPath.getPath(),"/*"});
            String sourcePath = androidLogsPath.getPath();
            File out_file = new File(sourcePath);
            File[]n_out = out_file.listFiles();

            
            try { 
                ProcessBuilder builder = new ProcessBuilder (new String[] { "chmod", "666", "/data/Mlog/*"}); 
                Process process = builder.start ( ) ; 
                if(DBG)
                Log.d(TAG, "copyAndroidLogs change mode");
            } catch ( IOException e ) { 
                Log.e(TAG, "copyAndroidLogs chmod exception: " + e);
                e.printStackTrace ( ) ; 
            }
            
            for (int i = 0; i < n_out.length; i++) {
                if (n_out[i].isFile()) {
                    if(DBG)
                        Log.d(TAG, "n_out[" + i + "] name is" + n_out[i].getName());
                   try {
                       if(!TargetPath.exists())
                            TargetPath.mkdirs();
                       //copyFile(n_out[i],new File(TargetPath+"/"+n_out[i].getName()));
                       copyFileToPath(n_out[i], TargetPath.getPath());
                    } catch (IOException e) {
                        Log.e(TAG, "copyAndroidLogs exception1: " + e);
                        e.printStackTrace();
                    }
                }
               if (n_out[i].isDirectory()) {
                   String sourceDir=sourcePath+n_out[i].getName()+"/";
                   String targetDir=TargetPath+"/"+n_out[i].getName();
                   try {
                        copyDirectory(sourceDir, targetDir);
                    } catch (IOException e) {
                        Log.e(TAG, "copyAndroidLogs exception2: " + e);
                        e.printStackTrace();
                    }
               }
           }
        } else {
            //do nothing, there is no androidLogs
            if(DBG)
                Log.d(TAG, "There is no androidLogs");
        }
        
    }
    
    private void copyDumpState(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyDumpState");
        final File dumpfile = new File(TargetPath + "/dumpstate");
        final File path = TargetPath;
        new Thread(new Runnable() {
            public void run() {
                String dump = mLogDumper.dumpState(false);

                if(!path.exists())
                    path.mkdirs();
                BufferedWriter bw = null;
                try {
                    dumpfile.createNewFile();
                    bw = new BufferedWriter(new FileWriter(dumpfile), 1024);
                    bw.write(dump);
                } catch (IOException e) {
                        Log.e(TAG, "copyDumpState exception1:", e);
                } finally {
                    if (bw != null) {
                        try {
                            bw.close();
                        } catch (IOException e) {
                            Log.e(TAG, "copyDumpState exception2:", e);
                        }
                    }
                }
            }
        }).start();
    }
    
    private void copyKernelLogs(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyKernelLogs");
        final File dumpfile = new File(TargetPath + "/dmesg");
        final File path = TargetPath;
        new Thread(new Runnable() {
            public void run() {
                String dump = mLogDumper.dumpCommand(false, "dmesg");
                if(!path.exists())
                    path.mkdirs();
                BufferedWriter bw = null;
                try {
                    dumpfile.createNewFile();
                    bw = new BufferedWriter(new FileWriter(dumpfile), 1024);
                    bw.write(dump);
                } catch (IOException e) {
                    Log.e(TAG, "copyKernelLogs exception1:", e);
                } finally {
                    if (bw != null) {
                        try {
                            bw.close();
                        } catch (IOException e) {
                            Log.e(TAG, "copyKernelLogs exception2:", e);
                        }
                    }
                }
            }
        }).start();
    }

    private void copyTombstonesLogs(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyTombstonesLogs");
        File TombstonesLogsPath = new File("/data/tombstones");
        if(TombstonesLogsPath.exists()){
            if(!TargetPath.exists())
                TargetPath.mkdirs();
            String sourcePath = TombstonesLogsPath.getPath();
            File out_file = new File(sourcePath);
            File[]n_out = out_file.listFiles();
            
            for (int i = 0; i < n_out.length; i++) {
                if (n_out[i].isFile()) {
                   try {
                        //copyFile(n_out[i],new File(TargetPath+"/"+n_out[i].getName()));
                       copyFileToPath(n_out[i], TargetPath.getPath());
                    } catch (IOException e) {
                        Log.e(TAG, "copyTombstonesLogs exception1:", e);
                        e.printStackTrace();
                    }
                }
               if (n_out[i].isDirectory()) {
                   String sourceDir=sourcePath+"/"+n_out[i].getName()+"/";
                   String targetDir=TargetPath+"/"+n_out[i].getName();
                   try {
                        copyDirectory(sourceDir, targetDir);
                    } catch (IOException e) {
                        Log.e(TAG, "copyTombstonesLogs exception2:", e);
                        e.printStackTrace();
                    }
               }
           }
        } else {
            //do nothing, there is no stack trace Logs
            if(DBG)
                Log.d(TAG, "There is no tombstones logs");
        }
        
    }
    
    private void copyStackTraceLogs(File TargetPath) {
        if(DBG)
            Log.d(TAG, "start copyStackTraceLogs");
        File StackTraceLogsPath = new File("/data/anr");
        if(StackTraceLogsPath.exists()){
            if(!TargetPath.exists())
                TargetPath.mkdirs();
            String sourcePath = StackTraceLogsPath.getPath();
            File out_file = new File(sourcePath);
            File[]n_out = out_file.listFiles();
            if(n_out == null)
                return;
            for (int i = 0; i < n_out.length; i++) {
                if (n_out[i].isFile()) {
                   try {
                        //copyFile(n_out[i],new File(TargetPath+"/"+n_out[i].getName()));
                       copyFileToPath(n_out[i], TargetPath.getPath());
                    } catch (IOException e) {
                        Log.e(TAG, "copyStackTraceLogs exception1:", e);
                        e.printStackTrace();
                    }
                }
               if (n_out[i].isDirectory()) {
                   String sourceDir=sourcePath+"/"+n_out[i].getName()+"/";
                   String targetDir=TargetPath+"/"+n_out[i].getName();
                   try {
                        copyDirectory(sourceDir, targetDir);
                    } catch (IOException e) {
                        Log.e(TAG, "copyStackTraceLogs exception2:", e);
                        e.printStackTrace();
                    }
               }
           }
        } else {
            //do nothing, there is no stack trace Logs
            if(DBG)
                Log.d(TAG, "There is no statck trace logs");
        }
        
    }
    
    private void copyFile(File sourceFile, File targetFile) throws IOException {
        FileInputStream input = new FileInputStream(sourceFile);        
        FileOutputStream output = new FileOutputStream(targetFile); 
        BufferedInputStream   bis   =   new   BufferedInputStream(input);  
        BufferedOutputStream   bos   =   new   BufferedOutputStream(output);            
        int count ;
        while ((count = bis.read())!=-1)
            bos.write(count);   
        bos.flush();
        bis.close();
        bos.close();
    }
    
    private void copyFileToPath(File sourceFile, String path) throws IOException {
        mLogDumper.nativeExecCommands(new String[]{"cp", sourceFile.getPath(), path});
    }
    
    private void copyDirectory(String sourceDir, String targetDir) throws IOException {
        if(DBG) Log.d(TAG, "copyDirectory sourceDir is " + sourceDir + " targetDir is " + targetDir);
        (new File(targetDir)).mkdirs(); 
        File[] file = (new File(sourceDir)).listFiles();
        if(file == null) 
            return;
        for (int i = 0; i < file.length; i++) {
            if (file[i].isFile()) {
                File sourceFile = file[i];
                String targetFile = new File(targetDir).getAbsolutePath();
                if(DBG) Log.d(TAG, "targetFile is " + targetFile);
                copyFileToPath(sourceFile, targetFile);
            }
            if (file[i].isDirectory()) {                 
                String dir1 = sourceDir + "/" + file[i].getName();
                String dir2 = targetDir + "/" + file[i].getName();
                copyDirectory(dir1, dir2);
            }
        }
    }
    

}
