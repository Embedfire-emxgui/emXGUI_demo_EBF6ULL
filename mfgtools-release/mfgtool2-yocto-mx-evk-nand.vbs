Set wshShell = CreateObject("WScript.shell")
wshShell.run "mfgtool2.exe -c ""linux"" -l ""NAND Flash"" -s ""sxuboot=sabreauto""  -s ""sxdtb=sabreauto"" -s ""6uluboot=14x14evk"" -s ""6uldtb=14x14-evk"" -s ""board=sabreauto"" -s ""nand=nand"" -s ""nanddtb=gpmi-weim"" -s ""part_uboot=0"" -s ""part_kernel=1"" -s ""part_dtb=2"" -s ""part_rootfs=3""   "
Set wshShell = Nothing
