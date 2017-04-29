/*
 * Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */
package com.marvell.wifidirect.demoapp.files;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.os.Environment;
import android.util.Log;

public class FileManager {

    private String filename;
    private String foldername;

    private File file;

    private OutputStream os;
    private BufferedOutputStream bos;
    private DataOutputStream dos;

    private InputStream is;
    private BufferedInputStream bis;
    private DataInputStream dis;

    public FileManager(String folder, String file) throws FileManagerException {
        this.foldername = folder;
        this.filename = file;
        try {
            this.file = this.prepareStorageAndFiles();
        } catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Storage and/or files are not prepared to be used");
        }

    }

    public String getFilePath(){
        return this.file.getAbsolutePath();
    }

    public void openFileToWrite() throws FileManagerException{
        try {
            os = new FileOutputStream(file);
            bos = new BufferedOutputStream(os);
            dos = new DataOutputStream(bos);

        } catch (FileNotFoundException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("File " + this.filename + " could not be opened");

        }
    }

    public void openFileToRead() throws FileManagerException {
        try {
            is = new FileInputStream(file);
            bis = new BufferedInputStream(is);
            dis = new DataInputStream(bis);

        } catch (FileNotFoundException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("File " + this.filename + " could not be opened");

        }
    }

    public void close(){

        try {
            if(this.os != null){
                this.dos.close();
                this.bos.close();
                this.os.close();
            }
            if(this.is != null){
                this.dis.close();
                this.bis.close();
                this.is.close();
            }

        } catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
        }
    }

    public void write(byte[] buffer) throws FileManagerException{

        try {
                dos.write(buffer);
        }  catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Buffer can not be written in the file");
        }
    }

    public void write(byte[] buffer, int offset, int size) throws FileManagerException{

        try {
            for (int i = 0+offset; i < size; i++)
                dos.writeByte(buffer[i]);
        }  catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Buffer can not be written in the file");
        }
    }

    public void write(short[] buffer, int offset, int size) throws FileManagerException{

        try {

            for (int i = 0+offset; i < size; i++)
                dos.writeShort(buffer[i]);
        }  catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Buffer can not be written in the file");
        }
    }

    public byte readByte() throws FileManagerException{
        try {
            return dis.readByte();
        }  catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Short can not be read from the file");
        }
    }

    public short readShort() throws FileManagerException{

        try {
            return dis.readShort();
        }  catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Short can not be read from the file");
        }
    }

    public int avaliable() throws FileManagerException{
        try {
            return dis.available();
        } catch (IOException e) {
            Log.e("Syso", e.toString());
            e.printStackTrace();
            throw new FileManagerException("Impossible to get the estimated number of bytes to read");
        }
    }


    private File prepareStorageAndFiles() throws FileManagerException{
        //Verify that the SD Card is mounted
        String state = android.os.Environment.getExternalStorageState();
        if(!state.equals(android.os.Environment.MEDIA_MOUNTED))  {
            throw new FileManagerException("SD Card is not mounted.  It is " + state + ".");
        }

        String totalpath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + this.foldername + "/" + this.filename;
        //Make sure the directory we plan to store the recording in exists
        File myfile = new File(totalpath);
        File myparentfile = myfile.getParentFile();

        if(!myparentfile.exists()){
            if(!myparentfile.mkdirs()){
                throw new FileManagerException("Path to file could not be created.");
            }
        }

        if(!myfile.exists()){
            try {
                myfile.createNewFile();
            } catch (IOException e) {
                Log.e("Syso", e.toString());
                e.printStackTrace();
                throw new FileManagerException("Failed to create " + myfile.toString() + "file");
            }
        }

        return myfile;
    }

    public void createNewFile() throws FileManagerException{

        //Delete any previous recording and creates a new empty file.
        if (this.file.exists()){
            this.file.delete();
            try {
                //Create the new file.
                this.file.createNewFile();
            } catch (IOException e) {
                Log.e("Syso", e.toString());
                e.printStackTrace();
                throw new FileManagerException("Failed to create " + this.file.toString());
            }

        }
    }
}
