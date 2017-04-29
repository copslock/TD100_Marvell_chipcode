"""Emit commands needed for Marvell devices during OTA installation
(installing uboot, obm and kernel image)."""

import common
import sha
import os
import copy

# the image name of each partition
RAMDISK_IMG =        'ramdisk.img'
KERNEL_IMG =         'uImage'
MAINTENANCE_IMG =    'uImage'
RECOVERY_IMG =       'ramdisk-recovery.img'
UBOOT_IMG =          'u-boot.bin'
ARBEL_IMG =          None
MSA_IMG =            None

# the mount point of each partition
RAMDISK_MOUNT =      '/ramdisk'
KERNEL_MOUNT =       '/kernel'
MAINTENANCE_MOUNT =  '/maintenance'
RECOVERY_MOUNT =     '/recovery'
MISC_MOUNT =         '/misc'
SYSTEM_MOUNT =       '/system'
DATA_MOUNT =         '/data'
NVM_MOUNT =          '/nvm'
TELEPHONY_MOUNT =    '/marvell'
UBOOT_MOUNT =        '/bootloader'
ARBEL_MOUNT =        '/arbelbinary'
MSA_MOUNT =          '/msabinary'

TELEPHONY_DIR =      'marvell/'
TELEPHONY_TARGETDIR =   'marvell/'

def IsSymlink(one_info):
  """Return true if the zipfile.ZipInfo object passed in represents a
  symlink."""
  return (one_info.external_attr >> 16) == 0120777

def CopyTelephonyFiles(info, input_name, output_name, ko_files, substitute=None):
  '''copy the telephony files from tmp zip package to OTA zip package, and store all the .ko files.
     input_name: the dir name in tmp zip package
     output_name: the dir name in OTA zip package
     ko_files: the list which stores all the .ko files under input_path
     substitute: the dict of {output filename: contents} to be output instead of certain input files.
  '''
  symlinks = []

  for one_info in info.input_zip.infolist():
    if one_info.filename.startswith(input_name):
      length = len(input_name)
      basefilename = one_info.filename[length:]
      if IsSymlink(one_info):
        symlinks.append((info.input_zip.read(one_info.filename), output_name + baasefilename))
      else:
        info2 = copy.copy(one_info)
        fn = info2.filename = output_name + basefilename
        if substitute and fn in substitute and substitute[fn] is None:
          continue
        if info.output_zip is not None:
          if substitute and fn in substitute:
            data = substitute[fn]
          else:
            data = info.input_zip.read(one_info.filename)
          if fn.endswith(".ko"):
            ko_files.append(fn)
          info.output_zip.writestr(info2, data)

  symlinks.sort()
  return symlinks

def SetTelephonyPermissions(script, mounted_dir, ko_files):
  '''set all the files under mounted_dir:
       all the .ko files with permission: system, system, 0644; ohters: system, system, 0755.
     script: the object of updater-script in OTA package
     mounted_dir: the mount point of the partition
     ko_files: the list stores all the .ko files
  '''
  script.SetPermissionsRecursive(mounted_dir, 1000, 1000, 0755, 0755)
  for f in ko_files:
    script.SetPermissions(f, 1000, 1000, 0644)

def UpdateTelephonyPartition(info, input_name, output_name, mounted_dir, partition_name):
  '''update the telephony partition
     info: the module infomation
     input_name: the directory name of telephony in temp zip package
     output_name: the direcotry name of telephony in OTA zip package
     mounted_dir: the mount point of the partition
     partition_name: the partition name in recovery.fstab
  '''
  ko_files = []
  symlinks = CopyTelephonyFiles(info, input_name, output_name, ko_files, None)

  info.script.Print("Start updating %s partition..." % (partition_name))
  info.script.FormatPartition(mounted_dir)
  info.script.Mount(mounted_dir)
  info.script.UnpackPackageDir(output_name, mounted_dir)
  info.script.MakeSymlinks(symlinks)
  SetTelephonyPermissions(info.script, mounted_dir, ko_files)
  info.script.UnmountAll()
  info.script.Print("Finish updating telephony Partition!")
  print "update %s successfully!" %(partition_name)

def UpdateRawPartition(info, img_name, partition_name, target_img, source_img=None):
  '''update the parition with type raw
     info: the module infomation
     img_name: the image's name of the partition
     partition_name: the name of the partition to be updated
     target_img: the target image data
     source_img: the source image data, default value is None
  '''
  tf = common.File(img_name, target_img)
  if source_img is None:
    tf.AddToZip(info.output_zip)
    info.script.Print("Writing %s partition..." % (partition_name))
    info.script.WriteRawImage(partition_name, tf.name)
    info.script.Print("Finish updating %s!"%(partition_name))
  else:
    sf = common.File(img_name, source_img);
    if tf.sha1 == sf.sha1:
      print "%s partition unchanged; skipping" % (partition_name)
    else:
      diff = common.Difference(tf, sf)
      _, _, d = diff.ComputePatch()
      if d is None or len(d) > tf.size * common.OPTIONS.patch_threshold:
        # computing difference failed, or difference is nearly as
        # big as the target:  simply send the target.
        tf.AddToZip(info.output_zip)
        info.script.Print("Writing %s partition..." % (partition_name))
        info.script.WriteRawImage(partition_name, tf.name)
        info.script.Print("Finish updating %s!"%(partition_name))
      else:
        print "%s   target: %d  source: %d  diff: %d" % (img_name, tf.size, sf.size, len(d))
        common.ZipWriteStr(info.output_zip, img_name + ".p", d)
        info.script.Print("Patching %s partition..." % (partition_name))
        tmp_type, tmp_device = common.GetTypeAndDevice(partition_name, info.info_dict)
        info.script.PatchCheck("%s:%s:%d:%s:%d:%s" % \
          (tmp_type, tmp_device, sf.size, sf.sha1, tf.size, tf.sha1))
        info.script.ApplyPatch("%s:%s:%d:%s:%d:%s" % \
          (tmp_type, tmp_device, sf.size, sf.sha1, tf.size, tf.sha1), \
          "-", tf.size, tf.sha1, sf.sha1, img_name + ".p")

  print "update %s successfully!" %(partition_name)

def WipePartition(info, mounted_dir, partition_name):
  '''erase the partition
     info: the module infomation
     mounted_dir: the mount point of the partition
     partition_name: the name of the partition to be updated
  '''
  info.script.Print("Start wiping %s partition..." % (partition_name))
  info.script.FormatPartition(mounted_dir)
  info.script.Print("Finish wiping %s!" % partition_name)
  print "wipe %s successfully!" % (partition_name)

def ReadImageData(zip_package, img_name):
  ''' read the image data from the zip package
      zip_package: info.input_zip or info.target_zip or infor.source_zip
      img_name: the image's name in the zip_package
  '''
  try:
    tmp_img = zip_package.read(img_name)
  except KeyError:
    print "no %s in update files; skip installing."%(img_name)
    return None
  else:
    return tmp_img

def FullOTA_InstallEnd(info):
  info.script.UnmountAll()

  # ----------update bootloader----------
  target_img = ReadImageData(info.input_zip, UBOOT_IMG)
  if target_img != None:
    UpdateRawPartition(info, UBOOT_IMG, UBOOT_MOUNT, target_img)

  # ----------update arbelbinary----------
  ARBEL_IMG = GetSpecialBinaryName(info.input_zip, "Arbel_", ".bin")
  if ARBEL_IMG != None:
    target_img = ReadImageData(info.input_zip, ARBEL_IMG)
    if target_img != None:
      UpdateRawPartition(info, ARBEL_IMG, ARBEL_MOUNT, target_img)

  # ----------update msabinary----------
  MSA_IMG = GetSpecialBinaryName(info.input_zip, "TT", "AI_A1_Flash.bin")
  if MSA_IMG != None:
    target_img = ReadImageData(info.input_zip, MSA_IMG)
    if target_img != None:
      UpdateRawPartition(info, MSA_IMG, MSA_MOUNT, target_img)

  # ----------update ramdisk----------
  target_img = ReadImageData(info.input_zip, RAMDISK_IMG)
  if target_img != None:
    UpdateRawPartition(info, RAMDISK_IMG, RAMDISK_MOUNT, target_img)

  # ----------update kernel----------
  target_img = ReadImageData(info.input_zip, KERNEL_IMG)
  if target_img != None:
    UpdateRawPartition(info, KERNEL_IMG, KERNEL_MOUNT, target_img)

  # ----------update maintenance----------
  target_img = ReadImageData(info.input_zip, MAINTENANCE_IMG)
  if target_img != None:
    UpdateRawPartition(info, MAINTENANCE_IMG, MAINTENANCE_MOUNT, target_img)

  # ----------update recovery----------
  target_img = ReadImageData(info.input_zip, RECOVERY_IMG)
  if target_img != None:
    UpdateRawPartition(info, RECOVERY_IMG, RECOVERY_MOUNT, target_img)

  # ----------wipe nvm----------
  WipePartition(info, NVM_MOUNT, NVM_MOUNT)

  # ----------wipe userdata----------
  WipePartition(info, DATA_MOUNT, DATA_MOUNT)

  # ----------wipe misc----------
  WipePartition(info, MISC_MOUNT, MISC_MOUNT)

  # ----------update telephony----------
  UpdateTelephonyPartition(info, TELEPHONY_DIR, TELEPHONY_TARGETDIR, TELEPHONY_MOUNT, TELEPHONY_MOUNT)

  info.script.ShowProgress(0.1, 1)

  info.script.Print("All done, just reboot and enjoy!")

def GetSpecialBinaryName(zip_file, prefix, suffix):
  for file_info in zip_file.infolist():
    if file_info.filename.startswith(prefix) and file_info.filename.endswith(suffix):
      return file_info.filename

  return None

def LoadTelephonyFile(zip_file, ko_files):
  out = {}
  for file_info in zip_file.infolist():
    if file_info.filename.startswith("marvell/"):
      data = zip_file.read(file_info.filename)
      out[file_info.filename] = common.File(file_info.filename, data)
    if ko_files and file_info.filename.endswith(".ko"):
      ko_files.append(file_info)

  return out

def MakeTelephonyPatch(info):
  verbatim_targets = []
  patch_list = []
  diffs = []
  ko_files = []
  largest_source_size = 0

  target_data = LoadTelephonyFile(info.target_zip, ko_files)
  source_data = LoadTelephonyFile(info.source_zip, None)

  for fn in sorted(target_data.keys()):
    tf = target_data[fn]
    assert fn == tf.name
    sf = source_data.get(fn, None)

    if sf is None:
      print "send" + fn + "verbatim"
      tf.AAddToZip(info.output_zip)
      verbatim_targets.append(fn, tf.size)
    elif tf.sha1 != sf.sha1:
      diffs.append(common.Difference(tf, sf))
    else:
      pass

  common.ComputeDifferences(diffs)

  for diff in diffs:
    tf, sf, d = diff.GetPatch()
    if d is None or len(d) > tf.size * common.OPTIONS.patch_threshold:
      tf.AddToZip(info.output_zip)
      verbatim_targets.append((tf.name, tf.size))
    else:
      common.ZipWriteStr(info.output_zip, "patch/" + tf.name + ".p" ,d)
      patch_list.append((tf.name, tf, sf, tf.size, sha.sha(d).hexdigest()))
      largest_source_size = max(largest_source_size, sf.size)

  if patch_list:
    info.script.CacheFreeSpaceCheck(largest_source_size)

  if verbatim_targets:
    info.script.Print("Unpacking new files...")
    info.script.UnpackPackageDir(TELEPHONY_TARGETDIR, TELEPHONY_MOUNT)

  info.script.Mount(TELEPHONY_MOUNT)
  info.script.Print("Removing unneeded files...")
  info.script.DeleteFiles(["/" + i[0] for i in verbatim_targets] +
                          ["/" + i for i in sorted(source_data) if i not in target_data])

  info.script.Print("Patching telephony files...")
  for fn, tf, sf, size, patch_sha in patch_list:
    info.script.PatchCheck("/" + fn, tf.sha1, sf.sha1)
    info.script.ApplyPatch("/" + fn, "-", tf.size, tf.sha1, sf.sha1, "patch/" + fn + ".p")

  SetTelephonyPermissions(info.script, TELEPHONY_MOUNT, ko_files)
  info.script.UnmountAll()
  info.script.Print("update telephony successfully!")

def IncrementalOTA_InstallEnd(info):
  info.script.UnmountAll()

  # ----------update uboot----------
  target_img = ReadImageData(info.target_zip, UBOOT_IMG)
  source_img = ReadImageData(info.source_zip, UBOOT_IMG)
  if target_img != None and source_img != None:
    UpdateRawPartition(info, UBOOT_IMG, UBOOT_MOUNT, target_img, source_img)

  # ----------update arbelbinary----------
  ARBEL_IMG = GetSpecialBinaryName(info.target_zip, "Arbel_", ".bin")
  if ARBEL_IMG != None:
    target_img = ReadImageData(info.target_zip, ARBEL_IMG)
    source_img = ReadImageData(info.source_zip, ARBEL_IMG)
    if target_img != None and source_img != None:
      UpdateRawPartition(info, ARBEL_IMG, ARBEL_MOUNT, target_img, source_img)

  # ----------update msabinary----------
  MSA_IMG = GetSpecialBinaryName(info.target_zip, "TT", "AI_A1_Flash.bin")
  if MSA_IMG != None:
    target_img = ReadImageData(info.target_zip, MSA_IMG)
    source_img = ReadImageData(info.source_zip, MSA_IMG)
    if target_img != None and source_img != None:
      UpdateRawPartition(info, MSA_IMG, MSA_MOUNT, target_img, source_img)

  # ----------update ramdisk----------
  target_img = ReadImageData(info.target_zip, RAMDISK_IMG)
  source_img = ReadImageData(info.source_zip, RAMDISK_IMG)
  if target_img != None and source_img != None:
    UpdateRawPartition(info, RAMDISK_IMG, RAMDISK_MOUNT, target_img, source_img)

  # ----------update kernel----------
  target_img = ReadImageData(info.target_zip, KERNEL_IMG)
  source_img = ReadImageData(info.source_zip, KERNEL_IMG)
  if target_img != None and source_img != None:
    UpdateRawPartition(info, KERNEL_IMG, KERNEL_MOUNT, target_img, source_img)

  # ----------update maintenance----------
  target_img = ReadImageData(info.target_zip, MAINTENANCE_IMG)
  source_img = ReadImageData(info.source_zip, MAINTENANCE_IMG)
  if target_img != None and source_img != None:
    UpdateRawPartition(info, MAINTENANCE_IMG, MAINTENANCE_MOUNT, target_img, source_img)

  # ----------update recovery----------
  target_img = ReadImageData(info.target_zip, RECOVERY_IMG)
  source_img = ReadImageData(info.source_zip, RECOVERY_IMG)
  if target_img != None and source_img != None:
    UpdateRawPartition(info, RECOVERY_IMG, RECOVERY_MOUNT, target_img, source_img)

  # ----------update telephony----------
  MakeTelephonyPatch(info)

  info.script.ShowProgress(0.1, 1)

  info.script.Print("All done, just reboot and enjoy!")
