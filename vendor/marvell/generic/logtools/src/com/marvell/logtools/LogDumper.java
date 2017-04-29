package com.marvell.logtools;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Pattern;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

public class LogDumper {
    private Prefs mPrefs;

    public LogDumper(Context context) {
        mPrefs = new Prefs(context);
    }

    public String dump(boolean html) {
        StringBuilder sb = new StringBuilder();
        BufferedReader br = null;
        Process p = null;

        try {
            p = Runtime.getRuntime().exec(
                    new String[] { "logcat", "-d", "-v",
                            mPrefs.getFormat().getValue(), "-b",
                            mPrefs.getBuffer().getValue(),
                            "*:" + mPrefs.getLevel() });

            br = new BufferedReader(new InputStreamReader(p.getInputStream()),
                    1024);

            String line;
            Pattern filterPattern = mPrefs.getFilterPattern();
            Format format = mPrefs.getFormat();

            Level lastLevel = Level.V;

            while ((line = br.readLine()) != null) {
                if (filterPattern != null
                        && !filterPattern.matcher(line).find()) {
                    continue;
                }

                if (!html) {
                    sb.append(line);
                    sb.append('\n');
                } else {
                    Level level = format.getLevel(line);
                    if (level == null) {
                        level = lastLevel;
                    } else {
                        lastLevel = level;
                    }
                    sb.append("<font color=\"" + level.getHexColor()
                            + "\" face=\"sans-serif\"><b>");
                    sb.append(TextUtils.htmlEncode(line));
                    sb.append("</b></font><br/>\n");

                }
            }
            String s = sb.toString();
            return s;
        } catch (IOException e) {
            Log.e("alogcat", "error reading log", e);
            return null;
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    Log.e("alogcat", "error closing stream", e);
                }
            }
            if (p != null) {
                p.destroy();
            }
        }
    }
    
    public String dumpCommand(boolean html, String command) {
        StringBuilder sb = new StringBuilder();
        BufferedReader br = null;
        Process p = null;

        try {
            p = Runtime.getRuntime().exec(command);

            br = new BufferedReader(new InputStreamReader(p.getInputStream()), 1024);

            String line;

            while ((line = br.readLine()) != null) {
                if (!html) {
                    sb.append(line);
                    sb.append('\n');
                } else {
                    sb.append("<font face=\"sans-serif\"><b>");
                    sb.append(TextUtils.htmlEncode(line));
                    sb.append("</b></font><br/>\n");

                }
            }
            String s = sb.toString();
            return s;
        } catch (IOException e) {
            Log.e("alogcat", "error reading kernel log", e);
            return null;
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    Log.e("alogcat", "error closing stream", e);
                }
            }
            if (p != null) {
                p.destroy();
            }
        }
    }
        
    public String dumpState(boolean html) {
        StringBuilder sb = new StringBuilder();
        BufferedReader br = null;
        Process p = null;

        try {
            p = Runtime.getRuntime().exec(
                    new String[] { "dumpstate", "-z" });

            br = new BufferedReader(new InputStreamReader(p.getInputStream()),
                    1024);

            String line;

            while ((line = br.readLine()) != null) {

                if (!html) {
                    sb.append(line);
                    sb.append('\n');
                } else {
                    sb.append("<font face=\"sans-serif\"><b>");
                    sb.append(TextUtils.htmlEncode(line));
                    sb.append("</b></font><br/>\n");

                }
            }
            String s = sb.toString();
            return s;
        } catch (IOException e) {
            Log.e("LogDumper", "dump state", e);
            return null;
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    Log.e("LogDumper", "error closing stream", e);
                }
            }
            if (p != null) {
                p.destroy();
            }
        }
    }   
    
    public void nativeExecCommands(String[] command) {
        try{
            Runtime.getRuntime().exec(command);
        } catch (IOException e) {
            Log.e("LogDumper","dumpCommands exception", e);
        } 
    }
    
}
